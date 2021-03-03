/*  file: getWiFiCreds.cpp
// Configure SoftAP (direct wifi ESP-client) characteristics


*/

#include "getWiFiCreds.h"
#include "publishvars.h"
#include "debug.h"
#include "ArduinoTrace.h"  //	DUMP(nDbgLvl);TRACE();
  /*
    This module establish connection to the  wifi accesspoint (WAP or internet WiFi router)
    If no connection is made it will enter a mode for setting up WAP credentials using a temporary
    soft accesspoint (SoftAP or Direct WiFi)
 */
//--------      Global VARIABLES

//  Parameters for the WiFiAccessPoint , will be get/set from SPIFFS
String AP_SSID = "Insert ROUTER SSID";  // your router's SSID here
String AP_PASS = "router password";     // your router's password here
String LM_URL = "http://thorsen.it/public/lm2020/draft_webapp";
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

//IP Address settings

//      Flags of statemachine. set by async calls  (see flowchart)
bool InitSoftAPOk=false;  //Set true by InitSoftAP when user  has inserted SSID&PWD
bool isMyStaticIPSet=false;

bool isWiFiStationConnected=false;  //Is device connected to softAP?
bool doGetFreeIP=false;
bool doStopSoftAP=false;          //Disconnect wifi
bool startAPP=false;
bool isWiFiSetupDone=false;//Ready to launch main LM_program
/*********        interface     ********/
//--------			SUB functions	----------------
bool mWIFISetup(AsyncWebServer & gserver){//Setup SOFT AP FOR CONFIGURING WIFI (Flowchart 2)
    bool ret=false;
    WiFi.disconnect();
    //Connect via SoftAP 192.168.1.1/setup
    ret=mSoftAPStart(gserver);//Sets AP_SSID, AP_PASS by Setup a soft accesspoint   and ask the user for credentials
    while (!isWiFiSetupDone){
      //connect to network and get the IP
      if (doGetFreeIP) ret=mGetFreeIP();//(AP_SSID, AP_PASS,MyStaticIP);
      ret=mSoftAPStart(gserver);//Sets AP_SSID, AP_PASS by Setup a soft accesspoint   and ask the user for credentials
      if (ret)   mSetCredentials();   //AP_SSID, AP_PASS,MyStaticIP); //Save the data
    } //Repeat the process until we are done
    return isWiFiSetupDone;
}
bool mGetFreeIP(){//Get a free  IP address and make it static
  //Flowchart: connect to network and get the IP
  // Set WiFi to station mode and disconnect from an AP if it was previously connected
  WiFi.disconnect();delay(100);
  WiFi.mode(WIFI_STA);
  WiFi.config(0U, 0U, 0U);//rt210121 reset the configuration so a fresh IP can be obtained - https://stackoverflow.com/a/54470525/2582833
  WiFi.begin(getAP_SSID().c_str(), AP_PASS.c_str());delay(100);
  DEBUG(2,"Connecting to internet WIFI to get IP\n");
  DEBUG(2,("|"+getAP_SSID() +"| , |"+AP_PASS+"|\n").c_str());
  for (int i=0;i<20;i++){ //Loop until timeout
    if (WiFi.status() == WL_CONNECTED) { //Wifi connection good get the IP
      AP_StaticIP=IpAddress2String(WiFi.localIP()).c_str();
      DEBUG(1,"\nIP address obtained: %s \n ===============================\n", ("|"+getAP_SSID() +"|"+getAP_PASS()+"|"+AP_StaticIP+"|").c_str());
      mSetCredentials();
      WiFi.disconnect();
      return true;
    }
    Serial.print(".");  delay(200);
  }
  //We have a timeout
    DEBUG(3,"Can not get AP_StaticIP in mGetFreeIP");
    return false;
  }

bool mWaitUntilTrueOrTimeout(bool &bFlag){
  for (int i=0;i<100000;i++){    //try until timeout
    if (bFlag) return true;
    mChangeDebugLevel();
    delay(100);
  }
  DEBUG(4,"Timeout mWaitUntilTrueOrTimeout");
  return false;
}


void mSetCredentials(){//Global params:(String AP_SSID,String AP_PASS,IPAddress  ) ){   //Get credentials from spiff
  //Saving credentials to SPIFFS
  DEBUG(8,"Saving credentials in FLASH");
  writeFile(SPIFFS, "/SSID.txt", AP_SSID.c_str());
  writeFile(SPIFFS, "/Password.txt", AP_PASS.c_str());
  writeFile(SPIFFS, "/Url.txt", LM_URL.c_str());
  writeFile(SPIFFS, "/IP.txt", AP_StaticIP.c_str());
  if (mGetCredentials()) {  //Readback
    return;
  }else{
    BRK(1,"Error in mSetCredentials");
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
    AP_PASS="";
    return false;   //Invalid file
  } else{           //We have credentials
    isMyStaticIPSet=true;
    return true;
  }
}


//  ***   WIFI EVENTS
void WiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info){
  DEBUG(1,"Station connected \n");
  isWiFiStationConnected=true;
}

void WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info){
  isWiFiStationConnected=false;
  Serial.println("Station DISCONNECTED ");
}

//Page not found
void notFound(AsyncWebServerRequest *request) {
   request->send(SPIFFS, "/creds.html", "text/html");
   DEBUG(1,"invalid request notFound");
}
bool mSoftAPStart(AsyncWebServer & gserver) {  //Get credentials from user
  //Params are byref (will return new values)
  //Return the parameter values
  //return true/false when parameters are obtained
  //WiFi.softAP(ssid, password, channel, hidden, max_connection); // Remove the password parameter, if you want the AP (Access Point) to be open
  InitSoftAPOk=false;     //Blocking flag to wait for user to accept settings
  doStopSoftAP=false;     //This function will block/wait until this flag becomes true
  isWiFiSetupDone=false;  //Will become true when user calls /startapp
  WiFi.softAP(SoftAP_SSID);
  delay(200);     //Allow time to connect
  IPAddress SoftAP_IP(192,168,1,1);
  IPAddress subnet(255,255,255,0);
  bool ret=WiFi.softAPConfig(SoftAP_IP, SoftAP_IP, subnet);
  Serial.print("Setting direct wifi (soft-AP) server:");
  Serial.println(ret ? "Ready" : "Failed!");
  WiFi.onEvent(WiFiStationConnected, SYSTEM_EVENT_AP_STACONNECTED);
  WiFi.onEvent(WiFiStationDisconnected, SYSTEM_EVENT_AP_STADISCONNECTED);

   //Setting Wifi specifications
  //Verify MeCFES IP Address
  DEBUG(2,"Waiting for user to open %s &  insert credentials\n",WiFi.softAPIP().toString().c_str());
  mWaitUntilTrueOrTimeout(isWiFiStationConnected);
   // Send web page with input fields to client
  gserver.on("/setup", HTTP_GET, [](AsyncWebServerRequest *request){
    DEBUG(2,"serving creds.html    -  ");
    request->send(SPIFFS, "/creds.html", "text/html");
  });
  // Serve style.css file
  gserver.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    DEBUG(4,"serving style.css\n");
    request->send(SPIFFS, "/style.css", "text/css");
  });
  //  AJAX Responses:
    gserver.on("/ssid", HTTP_GET, [](AsyncWebServerRequest *request){
      String PARAM1="value";
      DEBUG(1,"serving ssid\n");
      if (request->hasParam(PARAM1)) {
        AP_SSID=request->getParam(PARAM1)->value();
        DEBUG(1,"AP_SSID=|%s|\n",getAP_SSID().c_str());
      }
      DEBUG(4,"Sending AP_SSID to client\n");
      request->send(200, "text/plain", getAP_SSID().c_str());
    });
    gserver.on("/ip", HTTP_GET, [](AsyncWebServerRequest *request){
      DEBUG(2,"\nServing MyStaticIP =  %s \n",AP_StaticIP.c_str());
      String PARAM1="value";
      if (request->hasParam(PARAM1)) {    //Getting value from client
        AP_StaticIP=request->getParam(PARAM1)->value();
        mSetCredentials();//AP_SSID, AP_PASS,MyStaticIP);
      } else {
        DEBUG(1,"Sending current IP\n");
      }
      request->send(200, "text/plain",AP_StaticIP.c_str());
    });
    gserver.on("/url", HTTP_GET, [](AsyncWebServerRequest *request){
       String PARAM1="value";
       if (request->hasParam(PARAM1)) {
         LM_URL=request->getParam(PARAM1)->value();
         LM_URL.trim();
       }
       request->send(200, "text/plain", LM_URL.c_str());
    });
    gserver.on("/pass", HTTP_GET, [](AsyncWebServerRequest *request){
      String PARAM1="value";
      if (request->hasParam(PARAM1)) {
        AP_PASS=request->getParam(PARAM1)->value();
        DEBUG(2,"New password  %s \n",AP_PASS.c_str());
        mSetCredentials();//AP_SSID, AP_PASS,MyStaticIP);
      }
      request->send(200, "text/plain",AP_PASS.c_str());
    });
    gserver.on("/startapp", HTTP_GET, [](AsyncWebServerRequest *request){
           //Ready to goto server page
          request->send(200, "text/plain", "Starting websocket, please change network" );
          doStopSoftAP=true;
          isWiFiSetupDone=true;
    });
    //User accepted values
    gserver.on("/getIP", HTTP_GET, [] (AsyncWebServerRequest *request) {
      doGetFreeIP=true;      //Flag to start application and close softAP
      doStopSoftAP=true;  //Flag to stop the softAP
        //mSetCredentials();
        DEBUG(1,"Get IP for credentials: |%s|%s|%s|\n",getAP_SSID().c_str() , AP_PASS.c_str(),LM_URL.c_str());
        request->send(200, "text/plain", "INITIALIZING" );
  });
  gserver.onNotFound(notFound);
  gserver.begin();
  //"Continue if good. else if Failed to get credentials, abort");
  ret= mWaitUntilTrueOrTimeout(doStopSoftAP);
  DEBUG(2,"InitSoftAP, Disconnecting Soft AP\n");
  WiFi.disconnect();
  WiFi.softAPdisconnect (true);
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
  if (nDbgLvl>2) Serial.printf("Writing file: %s  |%s|  \r\n", path,message);
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
