

  /*
    This module establish connection to the  wifi accesspoint (WAP or internet WiFi router)
    If no connection is made it will enter a mode for setting up WAP credentials using a temporary
    soft accesspoint (SoftAP or Direct WiFi)

    Index.html -  user can configure the WIFI send the AP_SSID, AP_PASS,MyStaticIP to the connection from {InitSoftAP}
   this time show the MyStaticIP, tell the user to switch NETWORK            ----> Data correctly shown!!, it misses a link to start the Server in AP Mode (server.on("/startap", HTTP_GET, [](AsyncWebServerRequest *request) ---> launch AP mode setup and ws)
   ask user to:
   1.  copy MyStaticIP -----> DONE!
   2.  click link to serversite -----> DONE!
   3.  paste the MyStaticIP for the websocket MainSetup (todo: insert this in websocket program) -----> DONE!
 */

#include <Arduino.h>
#include "WiFi.h"
#include "SPIFFS.h"
#include "ESPAsyncWebServer.h"
#include <AsyncTCP.h>
extern "C" {  //Note- neccessary to implement C files
}
#include "getWiFiCreds.h"

//AsyncWebServer server(80);



// function prototypes for HTTP handlers (sigh!)
//............


String ssidvalue;







//SoftAP variables
AsyncWebSocket ws("/ws");
AsyncWebSocketClient * globalClient = NULL;




//Todo5: refactor/rename {InitSoftAP} to {mGetSetupViaSoftAP}



bool mStartWebSocket(IPAddress MyStaticIP,String AP_SSID,String AP_PASS){//This is the LM communication protocol
    IPAddress gateway(192, 168, 1, 1);
    IPAddress subnet(255, 255, 0, 0);
    WiFi.config(MyStaticIP, gateway, subnet);  // if using static IP, enter parameters at the top
    WiFi.begin(AP_SSID.c_str(), AP_PASS.c_str());
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.println("Connecting to WiFi..");
    }
    Serial.println(WiFi.localIP());
    //todo: copy code from lm_esp.ino line 38
    AsyncWebServer server(80);
    IPAddress staticIP(MyStaticIP[0],MyStaticIP[1],MyStaticIP[2],MyStaticIP[3]); // parameters below are for your static IP address, if used


    AsyncWebSocket ws("/ws");
    AsyncWebSocketClient * globalClient = NULL;
    for (int i=0;i<100;i++){    //try until timeout
      if (globalClient!=NULL) return true;
      delay(1000);
    }
    mDebugMsg("Timeout mWaitUntilTrueOrTimeout");
    return false;
  }


//const int MyStaticIP[4]={192, 168, 1, 51};
void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){
  //rt210114 Moved out of function.
    //AwsEventType describes what event has happened, like receive data or disconnetc
  // data is the payload
  if(type == WS_EVT_CONNECT){
    Serial.println("Websocket client connection received");
    globalClient = client;
  } else if(type == WS_EVT_DISCONNECT){
    Serial.println("Websocket client connection finished");
    globalClient = NULL;
  } else if(type == WS_EVT_DATA){  //Data was received from client
      mDebugHalt("mReceive is defined in lm_esp program");
    // mReceive(data,len);
  }
}



/*******   The two   STANDARD ARDUINO functions**********/
bool bWebSocketConnection =false;     //true when {mWIFIConnect == true}
void setup() {
 Serial.begin(115200);
 mDebugMsg("\nUnit testing\n");
 mDebugMsg("In Arduino remember to: Tools - Sketch upload ");
 if(!SPIFFS.begin(true)){
  Serial.println("An Error has occurred while mounting SPIFFS");
  return;
  }
  bWebSocketConnection=false;
  while (!bWebSocketConnection){
    //bWebSocketConnection=mWIFIConnect();
    bWebSocketConnection=mWIFIConnect();
  };      //Blocking until connection is made
//- (moved) InitSoftAP(AP_SSID, AP_PASS);  //Setup a soft accesspoint 192.168.4.1 and ask the user for credentials
} //Now we proceed to {loop}
void loop() {

  if (!bWebSocketConnection) return;  //Only loop if on internetwifi


}
/* ENDOF ******   The two   STANDARD ARDUINO functions**********/


/*******************************************/
//  Debugging  message and continue
void mPrint(String msg){
    delay(100);
    Serial.printf("%s\n",msg.c_str() );
    delay(100);
}
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
    Serial.print("\n------------HALT Command (stopped with endless loop) ------");
    Serial.println(msg );
    delay(300000);
  }
}
