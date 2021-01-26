#include "getWiFiCreds.h"
//#include "credentials.h"        //Default WIFI CREDENTIALS IN THIS FILE
#include "publishvars.h"

  /*
    This module establish connection to the  wifi accesspoint (WAP or internet WiFi router)
    If no connection is made it will enter a mode for setting up WAP credentials using a temporary
    soft accesspoint (SoftAP or Direct WiFi)
 */

//--------      Global VARIABLES
// Configure SoftAP (direct wifi ESP-client) characteristics


//  Parameters for the WiFiAccessPoint , will be get/set from SPIFFS

/*    MOVED TO creadentials.h:   */
//String AP_SSID="";  // your internet wifi  SSID
//String AP_PASS="";   // your internet wifi  password
String AP_SSID = "Rune";  // your router's SSID here
String AP_PASS = "telefon1";     // your router's password here
String LM_URL = "url";
String AP_StaticIP="192.168.1.238";
const char* SoftAP_SSID = "Arduino_LM";  //Name of the SoftAP - Arduino gets nicely first in the network list
//IPAddress MyStaticIP;  //The static IP address when using internet wifi router
//  ******  Make public getter/setters       *****
String getAP_SSID(){
  AP_SSID.trim();
  return AP_SSID;
}
String getAP_PASS(){
  AP_PASS.trim();
  return AP_PASS;
}
IPAddress getIP(){    //I think IPAddress is just an array
  AP_StaticIP.trim();
  return  String2IpAddress(AP_StaticIP);
}



//    Params from HTML pages
const char* PARAM_INPUT_1 = "SSID";
const char* PARAM_INPUT_2 = "Password";
const char* PARAM_INPUT_3 = "LM_URL";
//IP Address settings

//      Flags of statemachine. set by async calls  (see flowchart)
bool InitSoftAPOk=false;  //Set true by InitSoftAP when user  has inserted SSID&PWD
bool isMyStaticIPSet=false;
bool startAPP=false;    //Ready to launch main LM_program
bool stopsoftAP=false;    //Ready to launch main LM_program
bool isWiFiStationConnected=false;  //Is device connected to softAP?
bool mWIFISetup(AsyncWebServer & gserver){//Setup SOFT AP FOR CONFIGURING WIFI
            //(Flowchart 2)
    bool ret=false;
    WiFi.disconnect();
    mDebugMsgcpp("Calling InitSoftAP ");
    ret=InitSoftAP(gserver);//Sets AP_SSID, AP_PASS by Setup a soft accesspoint   and ask the user for credentials
      //The InitSoftAP will return the parameters
      //connect to network and get the IP
      mDebugMsgcpp("Calling mGetMyStaticIP ");
    if (ret) ret=mGetMyStaticIP();//(AP_SSID, AP_PASS,MyStaticIP);
    if (ret){ //We got our credentials, save and restart
      if (nDbgLvl>2) Serial.printf("MyStaticIP  %s\r\n", AP_StaticIP);
      mSetCredentials();//AP_SSID, AP_PASS,MyStaticIP);
      mDebugMsgcpp("Calling: InitSoftAP second time");
        //Setup the SoftAP from before, refresh client with full credentials
      ret=InitSoftAP(gserver);//Second call to update IP and allow to proceed to server
        //ret=mUserFeedbackViaSoftAP(); //Arguments AP_SSID, AP_PASS,MyStaticIP as globals
        return ret ; //If credentials have been saved now the recursive call should end width
          //  --->mStartWebSocket
    } else {  //Fail in getting credentials
        mDebugMsgcpp("Fail in getting credentials, retry");
    }
    return false;
}


bool mWIFIConnect(){//Main entry point - a blocking call
  mDebugMsgcpp("Executing: mWIFIConnect1");
  //Get credentials from SPIFFS (Flowchart 0)
  bool ret=mGetCredentials();
  //If  credentals  try to connect (Flowchart 1)
  if (ret){
    mDebugMsgcpp("Setting up the websocket, connect to StaticIP");
    //bool ret=mStartWebSocket(MyStaticIP, AP_SSID, AP_PASS) ;//(char*(AP_SSID), char*(AP_PASS), ); //Setup the static IP obtained
    //if (ret)
    return ret; //Tell caller to proceed
  } else {  //Fail in websocket connection, get credentials via SoftAP
            //(Flowchart 2)
    mDebugMsgcpp("Calling InitSoftAP ");
    //bool ret=InitSoftAP();//Sets AP_SSID, AP_PASS by Setup a soft accesspoint  and ask the user for credentials
      //The InitSoftAP will return the parameters
      //connect to network and get the IP
      mDebugMsgcpp("Done InitSoftAP, Calling mGetMyStaticIP ");
    if (ret) ret=mGetMyStaticIP();//(AP_SSID, AP_PASS,MyStaticIP);
    if (ret){ //We got our credentials, save and restart
      mDebugMsgcpp("Calling: mUserFeedbackViaSoftAP");
        //Setup the SoftAP from before, refresh client with full credentials
        ret=mUserFeedbackViaSoftAP(); //Arguments AP_SSID, AP_PASS,MyStaticIP as globals
        if (ret) { // credentials ready save them
          mSetCredentials();//AP_SSID, AP_PASS,MyStaticIP);
        }
        return mWIFIConnect(); //If credentials have been saved now the recursive call should end width
          //  --->mStart#include
    } else {  //Fail in getting credentials
        mDebugMsgcpp("Fail in getting credentials, retry");
        return false;
    }

  }
  return false; //Tell caller that we are waiting for a client to connect
}

//--------			SUB functions	----------------
bool mGetMyStaticIP(){//Get a free  IP address and make it static
  //Flowchart: connect to network and get the IP
  //TODO0 DEBUG
  // Set WiFi to station mode and disconnect from an AP if it was previously connected
  mDebugMsgcpp("mGetMyStaticIP");delay(1000);
  WiFi.disconnect();
  mDebugMsgcpp("WIFI_STA");delay(1000);
  WiFi.mode(WIFI_STA);
  WiFi.config(0U, 0U, 0U);//rt210121 reset the configuration so a fresh IP can be obtained - https://stackoverflow.com/a/54470525/2582833
  mDebugMsgcpp("WiFi.begin(AP_SSID.c_str()");delay(1000);
  WiFi.begin(AP_SSID.c_str(), AP_PASS.c_str());delay(1000);
  mDebugMsgcpp("Connecting to internet WIFI to get IP");
  Serial.println(("|"+AP_SSID +"| , |"+AP_PASS+"|").c_str());
  for (int i=0;i<20;i++){ //Loop until timeout
    delay(500);
    Serial.print(".");
    if (WiFi.status() == WL_CONNECTED) { //Wifi connection good
      // get the IP
      //Serial.printf("IP address obtained: %s",WiFi.localIP());
      AP_StaticIP=IpAddress2String(WiFi.localIP()).c_str();
      Serial.println("IP address obtained: ");
      Serial.println(AP_StaticIP);
      WiFi.disconnect();
      return true;
    }
  }
  //We have a timeout
    mDebugHaltcpp("Can not get AP_StaticIP in mGetMyStaticIP");
    return false;
  }

bool mWaitUntilTrueOrTimeout(bool &bFlag){
  for (int i=0;i<100000;i++){    //try until timeout
    if (bFlag) return true;

    delay(100);
  }
  mDebugMsgcpp("Timeout mWaitUntilTrueOrTimeout");
  return false;
}


void mSetCredentials(){//Global params:(String AP_SSID,String AP_PASS,IPAddress  ) ){   //Get credentials from spiff
  //Saving credentials to SPIFFS
  mDebugMsgcpp("Saving credentials in FLASH");
  writeFile(SPIFFS, "/SSID.txt", AP_SSID.c_str());
  writeFile(SPIFFS, "/Password.txt", AP_PASS.c_str());
  writeFile(SPIFFS, "/Url.txt", LM_URL.c_str());

  //Todo1: what if an invaid ip is given to  String2IpAddress?
  writeFile(SPIFFS, "/IP.txt", AP_StaticIP.c_str());
  if (mGetCredentials()) {  //Readback
    return;
  }else{
    mDebugHaltcpp("Error in mSetCredentials");
  }
}

bool mGetCredentials(){//Blocking. Will return true if credentials are in FLASH
  //RT210112: Moved code into method
  AP_SSID=readFile(SPIFFS, "/SSID.txt");
  if (nDbgLvl>4){ Serial.print("Saved ssid (AP_SSID): ");  Serial.println(AP_SSID);}
  AP_PASS=readFile(SPIFFS, "/Password.txt");
  if (nDbgLvl>4){ Serial.print("Password (AP_PASS): ");  Serial.println(AP_PASS);}
  LM_URL=readFile(SPIFFS, "/Url.txt");
  if (nDbgLvl>4){ Serial.print("server (LM_URL): ");  Serial.println(LM_URL);}
  AP_StaticIP=readFile(SPIFFS, "/IP.txt");
  if (nDbgLvl>4){ Serial.print("IP.txt (AP_StaticIP): ");  Serial.println(AP_StaticIP);}
  const String bEmptyString=String();
  if ((bEmptyString==AP_SSID)||(bEmptyString==AP_PASS)) {
    mDebugMsgcpp("Missing credentals files");
    return false;   //Invalid file
  } else{           //We have credentials
    isMyStaticIPSet=true;
    return true;
  }
}


//  ***   WIFI EVENTS
void WiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info){
  Serial.println("Station connected ");
  isWiFiStationConnected=true;
}

void WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info){
  isWiFiStationConnected=false;
  Serial.println("Station DISCONNECTED ");
}

//Page not found
void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Pagina non trovata");
}
bool InitSoftAP(AsyncWebServer & gserver) {  //Get credentials from user
  //Params are byref (will return new values)
  //Return the parameter values
  //return true/false when parameters are obtained
  //WiFi.softAP(ssid, password, channel, hidden, max_connection); // Remove the password parameter, if you want the AP (Access Point) to be open
  InitSoftAPOk=false;     //Blocking flag to wait for user to accept settings
  WiFi.softAP(SoftAP_SSID);
  delay(500);
  IPAddress SoftAP_IP(192,168,1,1);
  IPAddress subnet(255,255,255,0);
  bool ret=WiFi.softAPConfig(SoftAP_IP, SoftAP_IP, subnet);
  Serial.print("Setting direct wifi (soft-AP) server:");
  Serial.println(ret ? "Ready" : "Failed!");
  WiFi.onEvent(WiFiStationConnected, SYSTEM_EVENT_AP_STACONNECTED);
  WiFi.onEvent(WiFiStationDisconnected, SYSTEM_EVENT_AP_STADISCONNECTED);

   //Setting Wifi specifications
  //Verify MeCFES IP Address
  Serial.print("Soft-AP available on IP address = ");
  Serial.println(WiFi.softAPIP());
  mDebugMsgcpp("Waiting for user to connect to direct wifi");
  mWaitUntilTrueOrTimeout(isWiFiStationConnected);
  mDebugMsgcpp("Waiting for user to open page");
   // Send web page with input fields to client
  gserver.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    mDebugMsgcpp("serving index.html");
    request->send(SPIFFS, "/index.html", "text/html");
  });
  gserver.on("/get", HTTP_GET, [](AsyncWebServerRequest *request){
    mDebugMsgcpp("serving index.html second time");
    request->send(SPIFFS, "/index.html", "text/html");
  });
  // Serve style.css file
  gserver.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    mDebugMsgcpp("serving style.css");
    request->send(SPIFFS, "/style.css", "text/css");
  });
    gserver.on("/ssid", HTTP_GET, [](AsyncWebServerRequest *request){
      mDebugMsgcpp("Sending AP_SSID to client");
      request->send(200, "text/plain", AP_SSID.c_str());
    });
    gserver.on("/ip", HTTP_GET, [](AsyncWebServerRequest *request){
      mDebugMsgcpp("Sending sMyStaticIP to client");
      request->send(200, "text/plain",AP_StaticIP.c_str());
    });
    gserver.on("/url", HTTP_GET, [](AsyncWebServerRequest *request){
       mDebugMsgcpp("Sending url to client");
       request->send(200, "text/plain", LM_URL.c_str());
    });
    gserver.on("/pass", HTTP_GET, [](AsyncWebServerRequest *request){
         mDebugMsgcpp("index.html Sending AP_PASS to client");
        request->send(200, "text/plain",AP_PASS.c_str());
    });
    gserver.on("/startapp", HTTP_GET, [](AsyncWebServerRequest *request){
           //Send MeCFES bridgeapp
          request->send(200, "text/plain", LM_URL.c_str());
          // request->send(SPIFFS, "/bridgeAPP.html", "text/html");
          InitSoftAPOk=true;
          startAPP=true;
    });
    gserver.on("/get", HTTP_POST, [] (AsyncWebServerRequest *request) {
      if (request->hasParam(PARAM_INPUT_1,true)) {
        mDebugMsgcpp("Credentials received:");
        AP_SSID = request->getParam(PARAM_INPUT_1,true)->value();
        AP_PASS = request->getParam(PARAM_INPUT_2,true)->value();
        LM_URL = request->getParam(PARAM_INPUT_3,true)->value();
        AP_SSID.trim();   //Remove spaces
        AP_PASS.trim();
        LM_URL.trim();
        Serial.println(("|"+AP_SSID +"| , |"+AP_PASS+"|").c_str());
        InitSoftAPOk=true;
        request->send(SPIFFS, "/index.html", "text/html");
        //request->send(SPIFFS, "/onConnection.html", "text/html");
      }
    });
  gserver.onNotFound(notFound);
  gserver.begin();
  Serial.println("HTTP server started");
  mDebugMsgcpp("Waiting for user to insert credentials");
  //"Continue if good. else if Failed to get credentials, abort");
  ret= mWaitUntilTrueOrTimeout(InitSoftAPOk);
  mDebugMsgcpp("InitSoftAP Done");
  delay(5000);  //Wait for transactions to finish before closing?
  WiFi.disconnect();
  WiFi.softAPdisconnect (true);
  delay(1000);  //Wait for server to close?
  return ret;
}

//  *** FLASH files *************

//Read credentials from files (SSID.txt,Password.txt and IP.txt)
String readFile(fs::FS &fs, const char * path){
  if (nDbgLvl==6) Serial.printf("Reading file: %s\r\n", path);
  File file = fs.open(path, "r");
    if(!file || file.isDirectory()){
            Serial.println("- empty file or failed to open file");
            return String();
  }
if (nDbgLvl==6)  Serial.println("- read from file:");
  String fileContent;
  while(file.available()){
    fileContent+=String((char)file.read());
  }
if (nDbgLvl==6)  Serial.println(fileContent);
  return fileContent;
}

//Write credentials from files (SSID.txt,Password.txt and IP.txt)
void writeFile(fs::FS &fs, const char * path, const char * message){
    if (nDbgLvl>2) Serial.printf("Writing file: %s\r\n", path);
  File file = fs.open(path, "w");
  if(!file){
    Serial.println("- failed to open file for writing");
  return;
  }
  if(file.print(message)){
  if (nDbgLvl>7)  Serial.println("- file written");
  } else {
  Serial.println("- write failed");
  }
}


//Convert IP address to String
String IpAddress2String(const IPAddress& MyStaticIP)
{
  return String(MyStaticIP[0]) + String(".") +\
  String(MyStaticIP[1]) + String(".") +\
  String(MyStaticIP[2]) + String(".") +\
  String(MyStaticIP[3])  ;
}

IPAddress String2IpAddress(String sMyStaticIP)
{
  int Parts[4] = {0,0,0,0};
  int Part = 0;
  for ( int i=0; i<sMyStaticIP.length(); i++ )
  {
  	char c = sMyStaticIP[i];
  	if ( c == '.' )
  	{
  		Part++;
  		continue;
  	}
  	Parts[Part] *= 10;
  	Parts[Part] += c - '0';
  }
  IPAddress ip ( Parts[0], Parts[1], Parts[2], Parts[3] );
  return ip;
}
