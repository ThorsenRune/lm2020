/*
		Handling wireless transmissions
			websocket
			todo: bluetooth

*/
#include "transmit.h"
#include "publishvars.h"
#include "ArduinoTrace.h"  //	DUMP(nDbgLvl);TRACE();
extern "C" {  //Note- neccessary to implement C files
  #include "system.h"
  #include "inoProtocol.h"      //Including h file allows you to access the functions
  #include "publishvars.h"
}
#include "debug.h"				//	DEBUG(1,"something %i",nDbgLvl);
extern int TimeOutClient;
extern int TimeoutWifi;
extern bool bRelayLM2018 ;
AsyncWebSocket ws("/ws");
AsyncWebSocketClient * globalClient = NULL;
AsyncWebServer server2(80);
void onWsEvent2(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){
		//AwsEventType describes what event has happened, like receive data or disconnetc
	// data is the payload
	if(type == WS_EVT_CONNECT){
		Serial.print("Websocket connection client IP:");      Serial.println(client->remoteIP());
		globalClient = client;
	} else if(type == WS_EVT_DISCONNECT){
		Serial.println("Websocket client connection finished");
		globalClient = NULL;
	} else if(type == WS_EVT_DATA){  //Data was received from client
//+		 mReceive(data,len);
	} else {
		DEBUG(1,"unhandled onWsEvent");
	}
}

bool mStartWebSocket2(){
	DEBUG(1,"Starting Websocket %i\n",nDbgLvl);
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
  DEBUG(1,"WiFi is not available, check credentials");

  WiFi.disconnect();
  return false;   //WiFi not available maybe creaden
}
// todo1:release and replace mWaitForWSClient wit this
bool mWaitForWSClient2(int TimeOutClient){
  //DEBUG(0,"WiFi.localIP() %s\n","IpAddress2String(WiFi.localIP()");
  ws.onEvent(onWsEvent2);
  server2.addHandler(&ws);
  //  Why is this neccessary???
  server2.on("/html", HTTP_GET, [](AsyncWebServerRequest *request){
    DEBUG(2,"html served");
    request->send(SPIFFS, "/ws.html", "text/html");
  });
  server2.begin();
  DEBUG(2,"Waiting for client to connect");
  for (int i=0;i<TimeOutClient;i++){    //try until timeout
    if (globalClient!=NULL) return true;
    Serial.print(".");
    delay(1000);
  }
  DEBUG(2,"Timeout mWaitUntilTrueOrTimeout");
  return false;
}
//
/*************INTERFACE FOR SEND AND RECEIVE FROM CLIENT*************************/
// Get and Send data to client (the browser)
extern AsyncWebSocketClient * globalClient;
#define MAXL 800
char mSendData[MAXL] ;	//'efg
int SendDataBuf=0;
void mTransmit(){   //Transmit internal protocol data to client
  //todo0:   oTX&RX are initialized in setup-->---->	MainSetup --->mCommInitialize
    mProtocolProcess();						//Process RX/TX buffers
  if (nDbgLvl&(2<<6))   if (!mFIFO_isEmpty(oTX)) mDebugMsg("mTransmit to client");
  while (!mFIFO_isEmpty(oTX)){
    uint8_t sendbyte=mPopTXFIFO();    //Get byte from  protocol
    mSendData[SendDataBuf]=sendbyte;    //Get a byte from serial port
    SendDataBuf++;
    if (nDbgLvl&(2<<5))  mDebugInt("sending",sendbyte);
  }
  //This is where  the data exchange with the client happenes
   if(globalClient != NULL && globalClient->status() == WS_CONNECTED){
     if (SendDataBuf>0){
         globalClient->binary(mSendData,SendDataBuf );
         if (nDbgLvl&(2<<3))  Serial.printf("TX2 -> Client %d data\n ",SendDataBuf);
         nTestVar[3]=SendDataBuf;
         SendDataBuf=0;
     }
    }
    if (nDbgLvl&(2<<2))  delay(1); //Max messages per second =15 dont go below 100    	//note 201111
}


void mReceive(uint8_t *data, size_t len){ //Get data from client
	int i=0;
  if (nDbgLvl&(2<<6))  mDebugInt("mReceive",len);
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
