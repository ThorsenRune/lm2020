
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
int nDbgLvl=5;   //Verbosity level for debuggin messages
int TimeoutWifi=5;  //Seconds of timeout
int TimeOutClient=5;//Seconds of timeout mWaitForWSClient
int TimeOutStaticIDFetch=5;//Seconds of timeout mGetMyStaticIP

//  END OF DEBUGGING STuFF
#include <stdint.h>           //Define standard types uint32_t etc
#include "credentials.h"        //EDIT THE WIFI CREDENTIALS IN THIS FILE
#include "getWiFiCreds.h" //establish connection to the  wifi accesspoint (WAP or internet WiFi router)
#if (DEBUG_ON!=1)
  #include "SPIFFS.h"
  #include "WiFi.h"
  #include "ESPAsyncWebServer.h"
#endif

extern "C" {  //Note- neccessary to implement C files
  #include "system.h"
  #include "inoProtocol.h"      //Including h file allows you to access the functions
}
extern const int defaultStaticIP[4];
extern const char* defAP_SSID ;
extern const char* defAP_PASS;
extern String AP_SSID;
//Define the pins for  U2UXD
#define RXD2 16
#define TXD2 17
int nTimerInMs[3]={0};							// Milliseconds 1= T0 start of cycle 2=actual ms, 3How many clockcycles are available as resource


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
      Serial.println("Websocket client connection received");
      globalClient = client;
    } else if(type == WS_EVT_DISCONNECT){
      Serial.println("Websocket client connection finished");
      globalClient = NULL;
    } else if(type == WS_EVT_DATA){  //Data was received from client
  	   mReceive(data,len);
    }
  }
#endif

/*******   The two STANDARD ARDUINO functions (setup and loop)   **********/
/*  ----------   ARDUINO ENTRY POINT  ---------------------*/

void setup(){
  mDebugMsg("Calling mESPSetup");
#if  ( DEBUG_ON!=1)
  mESPSetup();
  mDebugMsg("Calling mGetCredentials");
  bool ret=mGetCredentials(); //now use getAP_SSID,getAP_PASS,getIP
  if (!ret) mDebugHalt("Insert call to mWIFIConnect that calls setup");
  else {
    ret=mStartWebSocket1();
  }
  if (!ret){      //Error in connection setup wifi
    mWIFISetup();
    setup();  //Repeat until wifi & WS is working
  }
  Serial.printf("Connected to WiFi: %s pwd:%s \n",getAP_SSID().c_str(),getAP_PASS().c_str());

#endif
  ret=mWaitForWSClient(TimeOutClient);
  if (!ret){
    mDebugMsg("Client not there,,,,going into mWIFISetup mode");
    mWIFISetup();
    setup();  //Repeat until wifi & WS is working
  }
  MainSetup();		//Setup the system, protocol & .. (rt210107)
  mDebugMsg("calling main loop in LM_ESP.ino");
#if ( DEBUG_ON==1)
  mTesting();
#endif
//- Serial.printf("Receive buffer reset. Free: %i ",mFIFO_Free(oRX));

}// This returns to an intrinsic call to loop()

/*******************ARDUINO MAIN WHILE LOOP *******************************/
//      AUTOMATICALLTY CALLED BT ARDUINO IDE
void loop(){		//The main loop of the
    nTimerInMs[2]++;    //Increase a timer just for fun
    mSerialReceive();
	//delay(1);


/*************      BUSINESS LOGIC FOR THE SIGNAL PROCESSING AND COMMUNICATION  *****/
//  TODO5 : enable / implement following calls
//+  mWaitCycleStart();							// 1.  Wait for a block start using system clocks
//+  mADCAux_Start();				//Todo1 maybe this corrupts EMG?
//+  if ( nMode.bits.SINEGENERATOR) mOutputSineWave();
//    mADCRestart(bFlip);
//    mOutputStimFSM_Debug(kReset);					//2. Reset statemachine for stimulaion and signals timing
//  mSignalProcessing();					//3. Signal Processing
//  mSystemActions();						//4.System management
    mTransmit();  //send data to client. Corresponding  mReceive is interrupt handled
    #if ( DEBUG_ON==1)
      delay(1000);  //todo9: remove
      mTesting2();
    #endif
}
/******************************* END MAIN WHILE(1) *******************************/





int nCounter=0;
String sRcvdData;
long rssi=0;	//signal strength

bool mStartWebSocket1(){
  IPAddress staticIP=getIP();
  IPAddress gateway(192, 168, 1, 1);
  IPAddress subnet(255, 255, 0, 0);
  WiFi.config(staticIP, gateway, subnet);  // if using static IP, enter parameters at the top
  if (nDbgLvl>2) Serial.printf("staticIP: %s \n", IpAddress2String(getIP()).c_str());
  if (nDbgLvl>2) Serial.printf("Starting   WiFi: %s pwd:%s \n",getAP_SSID().c_str(),getAP_PASS().c_str());
  WiFi.begin(getAP_SSID().c_str(),getAP_PASS().c_str());
  for (int i=0;i<TimeoutWifi;i++){ //Loop until timeout
    if (WiFi.status() == WL_CONNECTED) return true;   //Happily connected to wifi
    delay(1000); //Sleep to let connect
    if (nDbgLvl>2) Serial.print("."); //Make some waiting dots
  }
  mDebugMsg("WiFi is not available, check credentials");
  WiFi.disconnect();
  return false;   //WiFi not available maybe creaden
}
bool mWaitForWSClient(int TimeOutClient){
  Serial.print("WiFi.localIP() ");  Serial.println(WiFi.localIP());
  ws.onEvent(onWsEvent);
  /*  Why is this neccessary???*/
  server.addHandler(&ws);
  server.on("/html", HTTP_GET, [](AsyncWebServerRequest *request){
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

void mTransmit(){   //Transmit internal protocol data to client
  //todo0:   oTX&RX are initialized in setup-->---->	MainSetup --->mCommInitialize
  mCommunication();						//Process RX/TX buffers
  if (!mFIFO_isEmpty(oTX)) mDebugMsg("mTransmit to client");
  while (!mFIFO_isEmpty(oTX)){
    uint8_t sendbyte=mPopTXFIFO();    //Get byte from  protocol
    mSendToClient(sendbyte);
    mDebugInt("sending",sendbyte);
  }
}
void mReceive(uint8_t *data, size_t len){ //Get data from client
	sRcvdData="";
	int i=0;
  mDebugInt("mReceive",len);
    for( i=0; i < len; i++) {
        mDebugInt("data",data[i]);
        if (bRelayLM2018){  //Pass through to subdevice (LM)
          sRcvdData=sRcvdData+(char) data[i];	//Fill up a buffer from RX0
          Serial2.write(data[i]);		//Send incoming data  to LM
        } else {  //push the data onto buffer
          mPushRX2FIFO((char) data[i]);
      }
    }
	//if (i>0)Serial.printf("  Received data- pack length : %s \n" ,i);
//	Serial.print("RSSI:");	Serial.println(rssi);
    //Serial.println(sRcvdData);		//onWsEvent read end
}
/*END ************INTERFACE FOR SEND AND RECEIVE FROM CLIENT*************************/
//	rssi = WiFi.RSSI();
#define MAXL 500
char mSendData[MAXL] ;	//'efg
int SendDataBuf=0;
void mSendToClient(uint8_t sendbyte ){
  //Todo:merge 201222
  //Put a byte on the queue to send to the client (wifi/bluetooth)
  mSendData[SendDataBuf]=sendbyte;    //Get a byte from serial port
  //Serial.print(SendDataBuf,DEC) ;Serial.print(": ");
  //Serial.print(mSendData[SendDataBuf],DEC)  ;Serial.print(" \t ");
  SendDataBuf++;
}
void mSerialReceive(){
  #if ( DEBUG_ON==0)
     if(globalClient != NULL && globalClient->status() == WS_CONNECTED){
  	if (SendDataBuf>0){
  		globalClient->binary(mSendData,SendDataBuf );
  		Serial.printf("TX2 -> Client %d data\n ",SendDataBuf);
  		SendDataBuf=0;
  		delay(1); //Max messages per second =15 dont go below 100
      	//note 201111
  	  }
     }
  #endif
	while (Serial2.available()&&(SendDataBuf<MAXL)) {
    int sendbyte=Serial2.read();
    mSendToClient(sendbyte);
/* todo:delete		mSendData[SendDataBuf]=Serial2.read();    //Get a byte from serial port
		//Serial.print(SendDataBuf,DEC) ;Serial.print(": ");
		//Serial.print(mSendData[SendDataBuf],DEC)  ;Serial.print(" \t ");
		SendDataBuf++;*/
	}
//	if (SendDataBuf>0) Serial.printf("RX0 data: %d \n ",SendDataBuf);
}
void serialEvent2() {  //automatic event
	mSerialReceive();
}




/*
	note 201111
	Experimental findings:
	The serial port overflows at 256 received data . A 100 element vectorpack is 403 bytes
	No overflow, no serial data interrupt has been found, therefore the loop cant be
	delayed but the mSerialReceive must be polled frequently
*/

void mCheckSpiffs(){
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
      delay(100);
  }
  void mDebugInt(char msg[],int data){
    Serial.printf("%s  \t:\t %i\t %c\n",msg,data,(char)data);
    delay(100);
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
      delay(100);
  }
  void mDebugHaltcpp(char msg[]){
    while (1){
      Serial.print("\n------------HALT Command (stopped with endless loop) ------");
      Serial.println(msg );
      delay(300000);
    }
  }
  void mPrint(String msg){
      delay(100);
      Serial.printf("%s\n",msg.c_str() );
      delay(100);
  }
