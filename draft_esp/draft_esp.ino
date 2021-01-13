/*
Flowchart of mWIFIConnect()
mWIFIConnect will return true if connection is established and the program can
proceed to main

(0) credentials&IP=Read flash (SPIFF)
*
*
(1) Connect to network

|-fail—> Setup SoftAP (2) {InitSoftAP}

|-success-->Wait for client

    on connect

    |--> LM Program

    |--(timeout) —> Jump to (2)
*
*
*
*
(2) Setup SoftAP {InitSoftAP}

  |--> Connect to client via Soft AP

    * Get credentials from client. User writes SSID & Password

    * connect to network and get the IP

    * reconnect to client via Soft AP

    * send IP to client. Now user will know the IP, create a link to click

    * save credentials&IP to FLASH (SPIFF)

  |<------

  Jump to (1)



To do so use fragments from the ARDUINO - FILE- Examples - Wifi & Spiffs

Spiffs_test - read/write from flash

WiFiAccessPoint - SoftAP part (gia sperimentato)

Simple Wifiserver
 *
 *
 *
 *
 */

#include <Arduino.h>
#include "WiFi.h"
#include "SPIFFS.h"
#include "ESPAsyncWebServer.h"
#include <AsyncTCP.h>
  void mDebugMsg(char msg[]);
  void mDebugHalt(char msg[]);
  void mDebugInt(char msg[],int data);

AsyncWebServer server(80);
//  Parameters for the WiFiAccessPoint , will be get/set from SPIFFS
//Todo: should really be a data object/structure
String AP_SSID  ;  // your internet wifi  SSID
bool isSet_AP_SSID=false;   //rt210113 true when AP_SSID is set a
String AP_PASS ;   // your internet wifi  password
int MyStaticIP[4]={192, 168, 1, 51};  //The static IP address when using internet wifi router
bool bWebSocketConnection =false;     //true when {mWIFIConnect == true}
// Configure SoftAP (direct wifi ESP-client) characteristics
const char* ssid_softap = "MeCFES_Config";


String IPvalue="12.0.0.1";
String ssidvalue;
bool startAPP=false;
//- obsolete:String passwordvalue;

const char* PARAM_INPUT_1 = "SSID";
const char* PARAM_INPUT_2 = "Password";


//IP Address settings
IPAddress local_IP(192,168,4,1);
IPAddress gateway(192,168,4,9);
IPAddress subnet(255,255,255,0);


//Page not found
void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Pagina non trovata");
}


//Read credentials from files (SSID.txt,Password.txt and IP.txt)
String readFile(fs::FS &fs, const char * path){
  Serial.printf("Reading file: %s\r\n", path);
  File file = fs.open(path, "r");
    if(!file || file.isDirectory()){
            Serial.println("- empty file or failed to open file");
            return String();
  }
  Serial.println("- read from file:");
  String fileContent;
  while(file.available()){
    fileContent+=String((char)file.read());
  }
  Serial.println(fileContent);
  return fileContent;
}



//Write credentials from files (SSID.txt,Password.txt and IP.txt)
void writeFile(fs::FS &fs, const char * path, const char * message){
Serial.printf("Writing file: %s\r\n", path);
File file = fs.open(path, "w");
if(!file){
Serial.println("- failed to open file for writing");
return;
}
if(file.print(message)){
Serial.println("- file written");
} else {
Serial.println("- write failed");
}
}

//Todo5: refactor/rename {InitSoftAP} to {mGetSetupViaSoftAP}
bool InitSoftAP() {
  //Params are byref (will return new values)
  //Return the parameter values
  //return true/false when parameters are obtained
  //WiFi.softAP(ssid, password, channel, hidden, max_connection); // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAP(ssid_softap);
  delay(100);
   //Setting Wifi specifications
  Serial.print("Setting soft-AP configuration ... ");
  Serial.println(WiFi.softAPConfig(local_IP, gateway, subnet) ? "Ready" : "Failed!");
  //Verify MeCFES IP Address
  Serial.print("Soft-AP available on IP address = ");
  Serial.println(WiFi.softAPIP());
   // Send web page with input fields to client
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", "text/html");
  });

  // Send a GET request to <ESP_IP>/get?input1=<inputMessage>
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
    // GET input1 value on <ESP_IP>/get?input1=<inputMessage>
    if (request->hasParam(PARAM_INPUT_1)) {
      AP_SSID = request->getParam(PARAM_INPUT_1)->value();
      AP_PASS = request->getParam(PARAM_INPUT_2)->value();
      isSet_AP_SSID=true; //Proceed in flowchart
      /*  Obsolete by mSetCredentials
      writeFile(SPIFFS, "/SSID.txt", ssidvalue.c_str());
      writeFile(SPIFFS, "/Password.txt", passwordvalue.c_str());
      */
      return true; //Signal to caller to proceed with...
                  //                mSetCredentials(AP_SSID,AP_PASS,0);
    }
    else {
      mDebugMsg("No message sent");
    }
    // Send web page with input fields to client (Here only for debugging purposes, moved to mUserFeedbackViaSoftAP)
    request->send(SPIFFS, "/onConnection.html", "text/html");
   server.on("/ssid", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", AP_SSID.c_str());
    });
   server.on("/ip", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", IPvalue.c_str());
    });
  });
  server.onNotFound(notFound);
  server.begin();
  mDebugMsg("Waiting for user to insert credentials");
  for (int i=0;i<100;i++){  //Wait for async call to complete
    delay(1000);
    if (isSet_AP_SSID) 
    {
      return true;
    }

  }
  delay(300000);
  mDebugHalt("Failed to get credentials, abort");
  return false;
}


bool mUserFeedbackViaSoftAP(String AP_SSID,String AP_PASS,int MyStaticIP[4]) {
  //Flowchart:   * reconnect to client via Soft AP
    //* send IP to client. Now user will know the IP, create a link to click
    mDebugHalt("Implement mUserFeedbackViaSoftAP");
    //Start SoftAP mode again
     WiFi.softAP(ssid_softap);
     delay(100);
    //Setting Wifi specifications
    Serial.print("Setting soft-AP configuration ... ");
    Serial.println(WiFi.softAPConfig(local_IP, gateway, subnet) ? "Ready" : "Failed!");
    //Verify MeCFES IP Address (only for debuggin purposes for the moment)
    Serial.print("Soft-AP available on IP address = ");
    Serial.println(WiFi.softAPIP());
     //Page dedicated to data shown for user
     server.on("/", HTTP_GET, [] (AsyncWebServerRequest *request) {
    // Send web page with SSID and IP fields to client
    request->send(SPIFFS, "/onConnection.html", "text/html");
    server.on("/ssid", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", AP_SSID.c_str());
    });
    server.on("/ip", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", MyStaticIP.c_str());
    });
    server.on("/startapp", HTTP_GET, [](AsyncWebServerRequest *request){
   //Connect to AP mode
   //Launch AP mode
   //Send MeCFES bridgeapp
   request->send(SPIFFS, "/bridgeAPP.html", "text/html");
   startAPP=true;
 });
  });

    //TODO0
    /*send the AP_SSID, AP_PASS,MyStaticIP to the connection from {InitSoftAP}
      this time show the MyStaticIP, tell the user to switch NETWORK            ----> Data correctly shown!!, it misses a link to start the Server in AP Mode (server.on("/startap", HTTP_GET, [](AsyncWebServerRequest *request) ---> launch AP mode setup and ws)
      ask user to:
      1.  copy MyStaticIP -----> DONE!
      2.  click link to serversite -----> DONE!
      3.  paste the MyStaticIP for the websocket MainSetup (todo: insert this in websocket program) -----> DONE!
    */
}

bool mGetMyStaticIP(char* AP_SSID,char* AP_PASS,int MyStaticIP[4]) {
  //Flowchart: connect to network and get the IP
  //TODO0 DEBUG
  // Set WiFi to station mode and disconnect from an AP if it was previously connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  WiFi.begin(AP_SSID, AP_PASS);
  mDebugMsg("Connecting to WIFI to get IP");
  for (int i=0;i<20;i++){ //Loop until timeout
    delay(500);
    Serial.print(".");
    if (WiFi.status() == WL_CONNECTED) { //Wifi connection good
      // get the IP
      MyStaticIP=WiFi.localIP();
      Serial.println("IP address obtained: ");
      Serial.println(WiFi.localIP());
      return true;
    }
  }
  //We have a timeout
    mDebugHalt("Cant get MyStaticIP in mGetMyStaticIP");
    return false;
  }
}

bool mWIFIConnect(){//RT210112 Refactoring code by FC
  //Get credentials from SPIFFS (Flowchart 0)
  bool ret=mGetCredentials(AP_SSID,AP_PASS,MyStaticIP);
  //If  credentals  try to connect (Flowchart 1)
  if (ret){
    mDebugMsg("Setting up the websocket, connect to MyStaticIP");
    bool ret=mStartWebSocket(AP_SSID, AP_PASS,MyStaticIP); //Setup the static IP obtained
    if (ret) return true; //Tell caller to proceed
  } else {  //Fail in websocket connection, get credentials via SoftAP
            //(Flowchart 2)
    bool ret=InitSoftAP();//Sets AP_SSID, AP_PASS by Setup a soft accesspoint 192.168.4.1 and ask the user for credentials
      //The InitSoftAP will return the parameters
      //connect to network and get the IP
    if (ret) ret=mGetMyStaticIP(AP_SSID, AP_PASS,MyStaticIP)
    if (ret){ //We got our credentials, save and restart
        //Setup the SoftAP from before, refresh client with full credentials
        mUserFeedbackViaSoftAP(AP_SSID, AP_PASS,MyStaticIP);
        mSetCredentials(AP_SSID, AP_PASS,MyStaticIP);
        return mWIFIConnect();
    } else {  //Fail in getting credentials
        mDebugMsg("Fail in getting credentials, retry")
        return false;
    }

  }
  return false; //Tell caller that we are waiting for a client to connect
}

//const int MyStaticIP[4]={192, 168, 1, 51};
void mStartWebSocket( int MyStaticIP[4]){
  WiFi.config(staticIP, gateway, subnet);  // if using static IP, enter parameters at the top
  WiFi.begin(AP_SSID, AP_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println(WiFi.localIP());
  //todo: copy code from lm_esp.ino line 38
  AsyncWebServer server(80);
  IPAddress staticIP(MyStaticIP[0],MyStaticIP[1],MyStaticIP[2],MyStaticIP[3]); // parameters below are for your static IP address, if used
  IPAddress gateway(192, 168, 1, 1);
  IPAddress subnet(255, 255, 0, 0);

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
}


bool mGetCredentials(char* AP_SSID,char* AP_PASS,char*MyStaticIP ) ){   //Get credentials from spiff
  //RT210112: Moved code into method
  AP_SSID=readFile(SPIFFS, "/SSID.txt");
  Serial.print("Your ssid: ");
  Serial.println(AP_SSID);
  AP_PASS=readFile(SPIFFS, "/Password.txt");
  Serial.print("Your password: ");
  Serial.println(AP_PASS);
  MyStaticIP = readFile(SPIFFS, "/IP.txt");
  Serial.print("Your IP: ");
  Serial.println(MyStaticIP);
  return true;
}


bool mSetCredentials(char* AP_SSID,char* AP_PASS,char*MyStaticIP ) ){   //Get credentials from spiff
  //RT210112: Moved code into method
  writeFile(SPIFFS, "/SSID.txt", ssidvalue.c_str());
  writeFile(SPIFFS, "/Password.txt", AP_PASS.c_str())
  if (MyStaticIP!=void){
    writeFile(SPIFFS, "/IP.txt", MyStaticIP.c_str())
  }
  return true;
}

void setup() {
 Serial.begin(115200);
 if(!SPIFFS.begin(true)){
  Serial.println("An Error has occurred while mounting SPIFFS");
  return;
  }
  bWebSocketConnection=false;
  while (!bWebSocketConnection){
    bWebSocketConnection=mWIFIConnect();
  };      //Blocking until connection is made
//- (moved) InitSoftAP(AP_SSID, AP_PASS);  //Setup a soft accesspoint 192.168.4.1 and ask the user for credentials
} //Now we proceed to {loop}
void loop() {
  if (!bWebSocketConnection) return;  //Only loop if on internetwifi


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
