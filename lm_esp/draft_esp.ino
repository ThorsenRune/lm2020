/*  Unit testing for lm_esp
    This is our playground where we test new functionality

*/

//#include lm_esp.ino
/*
    COMMUNICATION
      mReceive: receive data from client and send to internal protocol or passes to subsystem (LM)
      mTransmit: inverse of mReceive, sends to client
      use : publishvars.c to expose variables to the client
*/
#define DEBUG_ON   0          //Conditional compilation for DEBUGGING
//  0-  release
//  1- No wifi for quicker compile and test purpose
int TimeoutWifi   =20;  //Seconds of timeout to get wifi connection
int TimeOutClient =120;//Seconds of timeout mWaitForWSClient, time for user to connect


//  END OF DEBUGGING STuFF
#include <stdint.h>           //Define standard types uint32_t etc
#include "getWiFiCreds.h" //establish connection to the  wifi accesspoint (WAP or internet WiFi router)
#if (DEBUG_ON!=1)
  #include "SPIFFS.h"
  #include "WiFi.h"
  #include "ESPAsyncWebServer.h"

#endif

extern "C" {  //Note- neccessary to implement C files
  #include "system.h"
  #include "inoProtocol.h"      //Including h file allows you to access the functions
  #include "publishvars.h"
}

extern String AP_SSID;
//Define the pins for  U2UXD
#define RXD2 16
#define TXD2 17


bool bRelayLM2018 = false;    //Apply protocol to arduino FW or relay to LM_FW
#if  ( DEBUG_ON!=1)
//todo replace  with the call to mStartWebSocket in draft_esp.ino
  AsyncWebServer server(80);
  AsyncWebSocket ws("/ws");
  AsyncWebSocketClient * globalClient = NULL;

  void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){
  		//AwsEventType describes what event has happened, like receive data or disconnetc
  	// data is the payload
    if(type == WS_EVT_CONNECT){
      Serial.print("Websocket connection client IP:");      Serial.println(client->remoteIP());
      globalClient = client;
    } else if(type == WS_EVT_DISCONNECT){
      Serial.println("Websocket client connection finished");
      globalClient = NULL;
    } else if(type == WS_EVT_DATA){  //Data was received from client
  	   mReceive(data,len);
    } else {
      mDebugMsg("unhandled onWsEvent");
    }
  }
#endif

/*******   The two STANDARD ARDUINO functions (setup and loop)   **********/
/*  ----------   ARDUINO ENTRY POINT  ---------------------*/

void setup(){
  mESPSetup();
  mDebugMsg("-------------running Android setup ------------");
  bool ret;
  ret=mGetCredentials(); //now use getAP_SSID,getAP_PASS,getIP
  if (ret) ret=mStartWebSocket1(); //Use credentials to attempt connection
  if (ret){ //Connection good wait for client to activate WS
    Serial.println(("Connected to WiFi |"+getAP_SSID() +"|"+getAP_PASS()+"|"+IpAddress2String(getIP())+"|").c_str());
    ret=mWaitForWSClient(TimeOutClient);
  }
  if (!ret){
    mWIFISetup(server);
    setup();  //Repeat until wifi & WS is working
  }
  //Websocket running ok
  MainSetup();		//Setup the system, protocol & .. (rt210107)
  mDebugMsg("calling main loop in LM_ESP.ino");
#if ( DEBUG_ON==1)
  mTesting1();
#endif
//- Serial.printf("Receive buffer reset. Free: %i ",mFIFO_Free(oRX));
  mTesting1();
}// This returns to an intrinsic call to loop()

/*******************ARDUINO MAIN WHILE LOOP *******************************/
//      AUTOMATICALLTY CALLED BT ARDUINO IDE
void loop(){		//The main loop of the
    mWaitCycleStart();							// 1.  Wait for a block start using system clocks
   if (nDbgLvl>0) mGenerateSignal();
/*************      BUSINESS LOGIC FOR THE SIGNAL PROCESSING AND COMMUNICATION  *****/
//  mSignalProcessing();					//3. Signal Processing
//  mSystemActions();						//4.System management
    mTransmit();                //send data to client. Corresponding  mReceive is interrupt handled
    mTesting2();
    mChangeDebugLevel();
}
/******************************* END MAIN WHILE(1) *******************************/


long rssi=0;	//signal strength

bool mStartWebSocket1(){
  IPAddress staticIP=getIP();
  IPAddress gateway(192, 168, 1, 1);
  IPAddress subnet(255, 255, 255, 0);
  WiFi.config(staticIP,staticIP,subnet);  // if using static IP, enter parameters at the top
  Serial.println(("Starting   WiFi|"+getAP_SSID() +"| , |"+getAP_PASS()+"|"+IpAddress2String(getIP())+"|").c_str());
  WiFi.begin(getAP_SSID().c_str(),getAP_PASS().c_str());
  for (int i=0;i<TimeoutWifi;i++){ //Loop until timeout
    if (WiFi.status() == WL_CONNECTED) return true;   //Happily connected to wifi
    delay(1000); //Sleep to let connect
    Serial.print("."); //Make some waiting dots
  }
  mDebugMsg("WiFi is not available, check credentials");
  WiFi.disconnect();
  return false;   //WiFi not available maybe creaden
}


bool mWaitForWSClient(int TimeOutClient){
  Serial.print("WiFi.localIP() ");  Serial.println(WiFi.localIP());
  ws.onEvent(onWsEvent);
  server.addHandler(&ws);
  /*  Why is this neccessary???*/
  server.on("/html", HTTP_GET, [](AsyncWebServerRequest *request){
    mDebugMsg("html served");
    request->send(SPIFFS, "/ws.html", "text/html");
  });
  server.begin();
  mDebugMsg("Waiting for client to connect");
  for (int i=0;i<TimeOutClient;i++){    //try until timeout
    if (globalClient!=NULL) return true;
    Serial.print(".");
    delay(1000);
  }
  mDebugMsg("Timeout mWaitUntilTrueOrTimeout");
  return false;
}

/*************INTERFACE FOR SEND AND RECEIVE FROM CLIENT*************************/
// Get and Send data to client (the browser)

#define MAXL 800
char mSendData[MAXL] ;	//'efg
int SendDataBuf=0;
void mTransmit(){   //Transmit internal protocol data to client
  //todo0:   oTX&RX are initialized in setup-->---->	MainSetup --->mCommInitialize
    mProtocolProcess();						//Process RX/TX buffers
  if (nDbgLvl>6)  if (!mFIFO_isEmpty(oTX)) mDebugMsg("mTransmit to client");
  while (!mFIFO_isEmpty(oTX)){
    uint8_t sendbyte=mPopTXFIFO();    //Get byte from  protocol
    mSendData[SendDataBuf]=sendbyte;    //Get a byte from serial port
    SendDataBuf++;
    if (nDbgLvl>5) mDebugInt("sending",sendbyte);
  }
  //This is where  the data exchange with the client happenes
   if(globalClient != NULL && globalClient->status() == WS_CONNECTED){
     if (SendDataBuf>0){
         globalClient->binary(mSendData,SendDataBuf );
         if (nDbgLvl&&0x3)  Serial.printf("TX2 -> Client %d data\n ",SendDataBuf);
         nTestVar[3]=SendDataBuf;
         SendDataBuf=0;
     }
    }
    delay(1); //Max messages per second =15 dont go below 100    	//note 201111
}


void mReceive(uint8_t *data, size_t len){ //Get data from client
	int i=0;
  if (nDbgLvl>6) mDebugInt("mReceive",len);
    for( i=0; i < len; i++) {
        if (nDbgLvl>7) mDebugInt("data",data[i]);
        if (bRelayLM2018){  //Pass through to subdevice (LM)
          Serial2.write(data[i]);		//Send incoming data  to LM
        } else {  //push the data onto buffer
          mPushRX2FIFO((char) data[i]);
      }
    }
	//if (i>0)Serial.printf("  Received data- pack length : %s \n" ,i);
//	Serial.print("RSSI:");	Serial.println(rssi);
}

/*END ************INTERFACE FOR SEND AND RECEIVE FROM CLIENT*************************/
//	rssi = WiFi.RSSI();


void serialEvent2() {  //automatic event from LM serial connection
  //Figure another way to relay to client
/*  while (Serial2.available()&&(SendDataBuf<MAXL)) {
    int sendbyte=Serial2.read();
    mSendData[SendDataBuf]=sendbyte;    //Get a byte from serial port
    SendDataBuf++;
  }
  */
}




void mChangeDebugLevel(){ //Takes a number from the arduino console and set the nDbgLvl accordingly
  if (Serial.available()>0){
    int debuglevel = Serial.parseInt();
    if (debuglevel>0){
      Serial.printf("Changed debugging level to : %i",debuglevel);
      nDbgLvl=debuglevel;
    }
  }
}

void mESPSetup(){
  Serial.begin(115200);
  Serial.println("Serial Txd is on pin: "+String(TX));
  Serial.println("Serial Rxd is on pin: "+String(RX));
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);  //Connection to LM subsystem
  if(!SPIFFS.begin()){
     Serial.println("An Error has occurred while mounting SPIFFS");
     mDebugHalt("Fatal problem in spiffs");
  }
}
//  Debugging  message and continue
  void mDebugMsg(char msg[]){
      Serial.print("Debugger says: \t ");
      Serial.printf("%s\n",msg );

  }
  void mDebugInt(char msg[],int data){
    Serial.printf("%s  \t:\t %i\t %c\n",msg,data,(char)data);
  }
//  Stop and do endless loop
void mDebugHalt(char msg[]){
  while (1){
    Serial.print("\nHALT Command (stopped with endless loop) ");
    Serial.printf("\n%s",msg );
    delay(300000);
  }
}
void  mDebugMsgcpp(char msg[]){
      Serial.print("Debugger says: \t ");
      Serial.printf("%s\n",msg );
    //  delay(100);
  }
  void mDebugHaltcpp(char msg[]){
      Serial.print("\n------------HALT  ------\n");
      Serial.println(msg );
    while (1){
      delay(300000);
    }
  }
  void mPrint(String msg){
      //delay(100);
      Serial.printf("%s\n",msg.c_str() );
      //delay(100);
  }
