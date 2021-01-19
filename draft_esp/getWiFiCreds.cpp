#include "getWiFiCreds.h"
#include "credentials.h"        //Default WIFI CREDENTIALS IN THIS FILE

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
//const char* SoftAP_SSID = "Arduino_LM";  //Name of the SoftAP - Arduino gets nicely first in the network list



IPAddress MyStaticIP;  //The static IP address when using internet wifi router

//    Params from HTML pages
const char* PARAM_INPUT_1 = "SSID";
const char* PARAM_INPUT_2 = "Password";
const char* PARAM_INPUT_3 = "Urlserver";
//IP Address settings
IPAddress SoftAP_IP(192,168,4,1);
IPAddress gateway(192,168,4,9);
IPAddress subnet(255,255,255,0);
AsyncWebServer server(80);

//      Flags of statemachine. set by async calls  (see flowchart)
bool InitSoftAPOk=false;  //Set true by InitSoftAP when user  has inserted SSID&PWD
bool isMyStaticIPSet=false;
bool startAPP=false;    //Ready to launch main LM_program
bool stopsoftAP=false;    //Ready to launch main LM_program
bool isWiFiStationConnected=false;  //Is device connected to softAP?

bool mWIFIConnect(){//Main entry point - a blocking call
  mDebugMsgcpp("Executing: mWIFIConnect1");
  //Get credentials from SPIFFS (Flowchart 0)
  bool ret=mGetCredentials();
  //If  credentals  try to connect (Flowchart 1)
  if (ret){
    mDebugMsgcpp("Setting up the websocket, connect to MyStaticIP");
    bool ret=mStartWebSocket(MyStaticIP, AP_SSID, AP_PASS) ;//(char*(AP_SSID), char*(AP_PASS),MyStaticIP); //Setup the static IP obtained
    if (ret) return true; //Tell caller to proceed
  } else {  //Fail in websocket connection, get credentials via SoftAP
            //(Flowchart 2)
    mDebugMsgcpp("Calling InitSoftAP ");
    bool ret=InitSoftAP();//Sets AP_SSID, AP_PASS by Setup a soft accesspoint 192.168.4.1 and ask the user for credentials
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
          //  --->mStartWebSocket
    } else {  //Fail in getting credentials
        mDebugMsgcpp("Fail in getting credentials, retry");
        return false;
    }

  }
  return false; //Tell caller that we are waiting for a client to connect
}

//--------			SUB functions	----------------
bool mGetMyStaticIP(){//Global params:{
  //Flowchart: connect to network and get the IP
  //TODO0 DEBUG
  // Set WiFi to station mode and disconnect from an AP if it was previously connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  WiFi.begin(AP_SSID.c_str(), AP_PASS.c_str());
  mDebugMsgcpp("Connecting to internet WIFI to get IP");
  Serial.println(("|"+AP_SSID +"| , |"+AP_PASS+"|").c_str());
  for (int i=0;i<20;i++){ //Loop until timeout
    delay(500);
    Serial.print(".");
    if (WiFi.status() == WL_CONNECTED) { //Wifi connection good
      // get the IP
      MyStaticIP=WiFi.localIP();
      Serial.println("IP address obtained: ");
      Serial.println(WiFi.localIP());
      WiFi.disconnect();
      return true;
    }
  }
  //We have a timeout
    mDebugHaltcpp("Can not get MyStaticIP in mGetMyStaticIP");
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

bool mUserFeedbackViaSoftAP(){//Global params:(String AP_SSID,String AP_PASS,IPAddress MyStaticIP) {
//Flowchart:   * reconnect to client via Soft AP
  //* send IP to client. Now user will know the IP, create a link to click
  //Start SoftAP mode again
   WiFi.softAP(SoftAP_SSID);
   delay(100);
  //Setting Wifi specifications
  Serial.print("User feedback Setting soft-AP configuration ... ");
  Serial.println(WiFi.softAPConfig(SoftAP_IP, gateway, subnet) ? "Ready" : "Failed!");
  //Verify MeCFES IP Address (only for debuggin purposes for the moment)
  Serial.print("User feedback Soft-AP available on IP address = ");
  Serial.println(WiFi.softAPIP());
  // Route to load style.css file
   server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
     request->send(SPIFFS, "/style.css", "text/css");
   });
   //Page dedicated to data shown for user
  server.on("/", HTTP_GET, [] (AsyncWebServerRequest *request) {
          // Send web page with SSID and IP fields to client
          request->send(SPIFFS, "/onConnection.html", "text/html");
          server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
   request->send(SPIFFS, "/style.css", "text/css");
 });
          server.on("/ssid", HTTP_GET, [](AsyncWebServerRequest *request){
                  request->send(200, "text/plain", AP_SSID.c_str());
          });
          isMyStaticIPSet=true;
          server.on("/ip", HTTP_GET, [](AsyncWebServerRequest *request){
                  request->send(200, "text/plain", IpAddress2String(MyStaticIP).c_str());
                  startAPP=true;
          });
          server.on("/url", HTTP_GET, [](AsyncWebServerRequest *request){
                  request->send(200, "text/plain", LM_URL.c_str());
                  startAPP=true;
          });

        });
           server.on("/startapp", HTTP_GET, [](AsyncWebServerRequest *request){
                 //Send MeCFES bridgeapp
                 request->send(SPIFFS, "/bridgeAPP.html", "text/html");
                 startAPP=true;
                 stopsoftAP=true;

          });
              //Wait here until user has submitted the response in startapp (startAPP==true)
    mDebugMsgcpp("Waiting for user in mUserFeedbackViaSoftAP");
    return mWaitUntilTrueOrTimeout(startAPP);
}

void mSetCredentials(){//Global params:(String AP_SSID,String AP_PASS,IPAddress MyStaticIP ) ){   //Get credentials from spiff
  //Saving credentials to SPIFFS
  mDebugMsgcpp("Saving credentials in FLASH");
  writeFile(SPIFFS, "/SSID.txt", AP_SSID.c_str());
  writeFile(SPIFFS, "/Password.txt", AP_PASS.c_str());
  writeFile(SPIFFS, "/Url.txt", LM_URL.c_str());

  //Todo1: what if an invaid ip is given to  String2IpAddress?
  writeFile(SPIFFS, "/IP.txt", IpAddress2String(MyStaticIP).c_str());
  if (mGetCredentials()) {  //Readback
    return;
  }else{
    mDebugHaltcpp("Error in mSetCredentials");
  }
}

bool mGetCredentials(){//Blocking. Will return true if credentials are in FLASH
  //RT210112: Moved code into method
  AP_SSID=readFile(SPIFFS, "/SSID.txt");
  Serial.print("Your ssid: ");
  Serial.println(AP_SSID);
  AP_PASS=readFile(SPIFFS, "/Password.txt");
  Serial.print("Your password: ");
  Serial.println(AP_PASS);
  LM_URL=readFile(SPIFFS, "/Url.txt");
  Serial.print("Your server: ");
  Serial.println(LM_URL);
  MyStaticIP=String2IpAddress(readFile(SPIFFS, "/IP.txt"));
  Serial.print("Your IP: ");
  Serial.println(MyStaticIP);
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
bool InitSoftAP() {  //Get credentials from user
  //Params are byref (will return new values)
  //Return the parameter values
  //return true/false when parameters are obtained
  //WiFi.softAP(ssid, password, channel, hidden, max_connection); // Remove the password parameter, if you want the AP (Access Point) to be open

  WiFi.softAP(SoftAP_SSID);
  delay(100);
  WiFi.onEvent(WiFiStationConnected, SYSTEM_EVENT_AP_STACONNECTED);
  WiFi.onEvent(WiFiStationDisconnected, SYSTEM_EVENT_AP_STADISCONNECTED);

   //Setting Wifi specifications
  Serial.print("Setting direct wifi (soft-AP) server");
  Serial.println(WiFi.softAPConfig(SoftAP_IP, gateway, subnet) ? "Ready" : "Failed!");
  //Verify MeCFES IP Address
  Serial.print("Soft-AP available on IP address = ");
  Serial.println(WiFi.softAPIP());
  mDebugMsgcpp("Waiting for user to connect to direct wifi");
  mWaitUntilTrueOrTimeout(isWiFiStationConnected);
  mDebugMsgcpp("Waiting for user to open page");
   // Send web page with input fields to client
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    mDebugMsgcpp("/index.html");
    request->send(SPIFFS, "/index.html", "text/html");
  });
  // Route to load style.css file
 server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
   request->send(SPIFFS, "/style.css", "text/css");
 });
  //Posting passwords is better than the GET method
  server.on("/get", HTTP_POST, [] (AsyncWebServerRequest *request) {
    if (request->hasParam(PARAM_INPUT_1,true)) {
      AP_SSID = request->getParam(PARAM_INPUT_1,true)->value();
      AP_PASS = request->getParam(PARAM_INPUT_2,true)->value();
      LM_URL = request->getParam(PARAM_INPUT_3,true)->value();
      AP_SSID.trim();   //Remove spaces
      AP_PASS.trim();
      LM_URL.trim();
      mDebugMsgcpp("Credentials received:");
      Serial.println(("|"+AP_SSID +"| , |"+AP_PASS+"|").c_str());
      request->send(SPIFFS, "/onConnection.html", "text/html");
      server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){//Serve the stylesheet
          request->send(SPIFFS, "/style.css", "text/css");
      });
      server.on("/ssid", HTTP_GET, [](AsyncWebServerRequest *request){
     mDebugMsgcpp("Sending AP_SSID to client");
    request->send(200, "text/plain", AP_SSID.c_str());
    });
      server.on("/ip", HTTP_GET, [](AsyncWebServerRequest *request){
     mDebugMsgcpp("Sending sMyStaticIP to client");
    request->send(200, "text/plain",IpAddress2String(MyStaticIP).c_str());
    startAPP=true;
    });
    server.on("/url", HTTP_GET, [](AsyncWebServerRequest *request){
                  request->send(200, "text/plain", LM_URL.c_str());
                  startAPP=true;
          });
      server.on("/startapp", HTTP_GET, [](AsyncWebServerRequest *request){
           //Send MeCFES bridgeapp
           request->send(SPIFFS, "/bridgeAPP.html", "text/html");
           startAPP=true;

    });
      delay(1000);
      InitSoftAPOk=true;   //Proceed in flowchart
    } else {
      mDebugMsgcpp("No message sent");
    }
    }
  );

  // Send a GET request to <ESP_IP>/get?input1=<inputMessage>
  // Obsolete HTTP_GET by HTTP_POST
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
    mDebugMsgcpp("HTTP_GET");
    // GET input1 value on <ESP_IP>/get?input1=<inputMessage>
    if (request->hasParam(PARAM_INPUT_1)) {
      AP_SSID = request->getParam(PARAM_INPUT_1)->value();
      AP_PASS = request->getParam(PARAM_INPUT_2)->value();
      LM_URL = request->getParam(PARAM_INPUT_3)->value();
       mDebugMsgcpp("Credentials:");
       Serial.println(("|"+AP_SSID +"| , |"+AP_PASS+"|").c_str());
       InitSoftAPOk=true;   //Proceed in flowchart
                  //                mSetCredentials(AP_SSID,AP_PASS,0);
    }
    else {
      mDebugMsgcpp("No message sent");
    }
    // Send web page with input fields to client (Here only for debugging purposes, moved to mUserFeedbackViaSoftAP)
    request->send(SPIFFS, "/onConnection.html", "text/html");
    server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
   request->send(SPIFFS, "/style.css", "text/css");
 });
   server.on("/ssid", HTTP_GET, [](AsyncWebServerRequest *request){
     mDebugMsgcpp("Sending AP_SSID to client");
    request->send(200, "text/plain", AP_SSID.c_str());
    });
   server.on("/ip", HTTP_GET, [](AsyncWebServerRequest *request){
     mDebugMsgcpp("Sending sMyStaticIP to client");
    request->send(200, "text/plain", IpAddress2String(MyStaticIP).c_str());
    startAPP=true;
    });
    server.on("/url", HTTP_GET, [](AsyncWebServerRequest *request){
                  request->send(200, "text/plain", LM_URL.c_str());
                  startAPP=true;
          });
  });
   server.on("/startapp", HTTP_GET, [](AsyncWebServerRequest *request){
           //Send MeCFES bridgeapp
           request->send(SPIFFS, "/bridgeAPP.html", "text/html");

           startAPP=true;
    });
  server.onNotFound(notFound);
  server.begin();
  mDebugMsgcpp("Waiting for user to insert credentials");
  //"Continue if good. else if Failed to get credentials, abort");
  return mWaitUntilTrueOrTimeout(InitSoftAPOk);
}





//  *** FLASH files
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
