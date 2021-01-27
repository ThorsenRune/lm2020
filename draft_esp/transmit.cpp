/*
		Handling wireless transmissions
			websocket
			todo: bluetooth

*/
#include "transmit.h"

extern "C" {  //Note- neccessary to implement C files
  #include "system.h"
  #include "inoProtocol.h"      //Including h file allows you to access the functions
  #include "publishvars.h"
}
extern int TimeOutClient;
extern int TimeoutWifi;
extern bool bRelayLM2018 ;
AsyncWebSocket ws("/ws");
AsyncWebSocketClient * globalClient = NULL;
AsyncWebServer server2(80);

AsyncWebServer *_server;
AsyncWebSocket *_ws;


void mWifiSetupMain(){      //Setup wifi
    bool ret;
    if (_server==NULL)_server = new AsyncWebServer(80);
    ret=mGetCredentials(); //now use getAP_SSID,getAP_PASS,getIP
    if (ret) ret=mStartWebSocket3(); //Use credentials to attempt connection
    if (ret){ //Connection good wait for client to activate WS
      DEBUG(1,("\nConnected to WiFi |"+getAP_SSID() +"|"+getAP_PASS()+"|"+IpAddress2String(getIP())+"|").c_str());
      ret=mWaitForWSClient(TimeOutClient);
    }
    if (!ret){
      mWIFISetup(*_server);
      TimeOutClient=200;
      DEBUG(1,"WIFI SETUP AND READY\n\n");
      mWifiSetupMain();  //Repeat until wifi & WS is working
    }
}
//-----------------------PRIVATE STUFF --------------------
bool mStartWebSocket3(){  //Returns true when connection is established
	DEBUG(1,"Starting Websocket \n");
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
  DEBUG(1,"WiFi is not available, check credentials\n");

  WiFi.disconnect();
  return false;   //WiFi not available maybe creaden
}

bool mWaitForWSClient(int TimeOutClient){
  DEBUG(0,"\n----------WiFi.localIP() %s\n",WiFi.localIP().toString().c_str());
  if (_server==NULL)_server = new AsyncWebServer(80);
  TRACE();
  _ws=new AsyncWebSocket("/ws");
  TRACE();
  _ws->onEvent(_WsEvent);
    TRACE();
  _server->addHandler(_ws);
    TRACE();
  //  Why is this neccessary???
  _server->on("/html", HTTP_GET, [](AsyncWebServerRequest *request){
    DEBUG(2,"html served");
    request->send(SPIFFS, "/ws.html", "text/html");
  });
  _server->begin();
  DEBUG(2,"Waiting for client to connect");
  for (int i=0;i<TimeOutClient;i++){    //try until timeout
    if (globalClient!=NULL) return true;
    if (Serial.available()>0) break;    //Goto wifi setup
    Serial.print(".");  delay(1000);    //Wait for user to connect which will set globalClient
  }
  DEBUG(2,"Timeout mWaitUntilTrueOrTimeout");
  return false;
}
//
/*************INTERFACE FOR SEND AND RECEIVE FROM CLIENT*************************/
// Get and Send data to client (the browser)

#define MAXL 800
char mSendData[MAXL] ;	// A temporary send buffer todo4: remove
int SendDataBuf=0;
void mTransmit(){   //Transmit internal protocol data to client
  //todo0:   oTX&RX are initialized in setup-->---->	MainSetup --->mCommInitialize
    mProtocolProcess();						//Process RX/TX buffers
  DEBUG(5,"mTransmit to client\n");
  while (!mFIFO_isEmpty(oTX)){
    uint8_t sendbyte=mPopTXFIFO();    //Get byte from  protocol
    mSendData[SendDataBuf]=sendbyte;    //Get a byte from serial port
    SendDataBuf++;
  }
  //This is where  the data exchange with the client happenes
   if(globalClient != NULL && globalClient->status() == WS_CONNECTED){
     if (SendDataBuf>0){
         globalClient->binary(mSendData,SendDataBuf ); //Todo4: bypass th txfifo
         DEBUG(4,"TX2 -> Client %d data\n ",SendDataBuf);
         nTestVar[3]=SendDataBuf;
         SendDataBuf=0;
     }  else (DEBUG(9,"Nothing to send\n"));
   } else DEBUG(8,"Not Connected\n");
    if (nDbgLvl&(2<<2))  delay(1); //Max messages per second =15 dont go below 100    	//note 201111
}

void _WsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){
		//AwsEventType describes what event has happened, like receive data or disconnetc
	// data is the payload
	if(type == WS_EVT_CONNECT){
		DEBUG(1,"Websocket connection client IP:%s\n",client->remoteIP().toString().c_str());
	   globalClient = client;
	} else if(type == WS_EVT_DISCONNECT){
		DEBUG(1,"Websocket client connection finished\n");
 	  globalClient = NULL;
	} else if(type == WS_EVT_DATA){  //Data was received from client
     DEBUG(7,"Length len:  %i\n",(int)len);
   	 mReceive2(data,len);
	} else {
		DEBUG(1,"unhandled onWsEvent\n");
	}
}
void mReceive2(uint8_t *data, size_t len){ //Get data from client
	int i=0;
  DEBUG(8,"mReceive len:  %i\n",(int)len);
    for( i=0; i < len; i++) {
        DEBUG(7,"data %i\n",(int)data[i]);
        if (bRelayLM2018){  //Pass through to subdevice (LM)
              TRACE();
          Serial2.write(data[i]);		//Send incoming data  to LM
        } else {  //push the data onto buffer
          mPushRX2FIFO((char) data[i]);
      }
    }
	//if (i>0)Serial.printf("  Received data- pack length : %s \n" ,i);
//	Serial.print("RSSI:");	Serial.println(rssi);
}

/*END ************INTERFACE FOR SEND AND RECEIVE FROM CLIENT*************************/
