#include "getWiFiCreds.h"
//#include "credentials.h"        //Default WIFI CREDENTIALS IN THIS FILE

  /*
    This module establish connection to the  wifi accesspoint (WAP or internet WiFi router)
    If no connection is made it will enter a mode for setting up WAP credentials using a temporary
    soft accesspoint (SoftAP or Direct WiFi)
 */

int nDbgLvel=5;   //Verbosity level for debuggin messages
//--------      Global VARIABLES
// Configure SoftAP (direct wifi ESP-client) characteristics


//  Parameters for the WiFiAccessPoint , will be get/set from SPIFFS

/*    MOVED TO creadentials.h:   */
//String AP_SSID="";  // your internet wifi  SSID
//String AP_PASS="";   // your internet wifi  password
String AP_SSID = "networkname";  // your router's SSID here
String AP_PASS = "password";     // your router's password here
String LM_URL = "url";
String AP_StaticIP="192.168.1.101";
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
IPAddress SoftAP_IP(192,168,4,1);
IPAddress gateway(192,168,4,9);
IPAddress subnet(255,255,255,0);
//AsyncWebServer gserver(80);

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
    ret=InitSoftAP(gserver);//Sets AP_SSID, AP_PASS by Setup a soft accesspoint 192.168.4.1 and ask the user for credentials
      //The InitSoftAP will return the parameters
      //connect to network and get the IP
      mDebugMsgcpp("Done InitSoftAP, Calling mGetMyStaticIP ");
    if (ret) ret=mGetMyStaticIP();//(AP_SSID, AP_PASS,MyStaticIP);
    if (ret){ //We got our credentials, save and restart
      if (nDbgLvel>2) Serial.printf("Reading file: %s\r\n", path); 
      mDebugMsgcpp("Calling: mUserFeedbackViaSoftAP");
        //Setup the SoftAP from before, refresh client with full credentials
        ret=mUserFeedbackViaSoftAP(); //Arguments AP_SSID, AP_PASS,MyStaticIP as globals
        if (ret) { // credentials ready save them
          mSetCredentials();//AP_SSID, AP_PASS,MyStaticIP);
        }
        return true ; //If credentials have been saved now the recursive call should end width
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
    //bool ret=InitSoftAP();//Sets AP_SSID, AP_PASS by Setup a soft accesspoint 192.168.4.1 and ask the user for credentials
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
bool mGetMyStaticIP(){//Get a free  IP address and make it static
  //Flowchart: connect to network and get the IP
  //TODO0 DEBUG
  // Set WiFi to station mode and disconnect from an AP if it was previously connected
  WiFi.disconnect();
  WiFi.mode(WIFI_STA);
  WiFi.begin(AP_SSID.c_str(), AP_PASS.c_str());
  mDebugMsgcpp("Connecting to internet WIFI to get IP");
  Serial.println(("|"+AP_SSID +"| , |"+AP_PASS+"|").c_str());
  for (int i=0;i<20;i++){ //Loop until timeout
    delay(500);
    Serial.print(".");
    if (WiFi.status() == WL_CONNECTED) { //Wifi connection good
      // get the IP
      AP_StaticIP=IpAddress2String(WiFi.localIP());
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

bool mUserFeedbackViaSoftAP(){//Global params:(String AP_SSID,String AP_PASS,IPAddress MyStaticIP) {
  /*
//Flowchart:   * reconnect to client via Soft AP
  //* send IP to client. Now user will know the IP, create a link to click
  //Start SoftAP mode again
   WiFi.softAP(SoftAP_SSID);
   delay(1000);
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
  gserver.on("/", HTTP_GET, [] (AsyncWebServerRequest *request) {
          // Send web page with SSID and IP fields to client
          request->send(SPIFFS, "/onConnection.html", "text/html");
          gserver.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
            request->send(SPIFFS, "/style.css", "text/css");
          });
          gserver.on("/ssid", HTTP_GET, [](AsyncWebServerRequest *request){
                  request->send(200, "text/plain", AP_SSID.c_str());
          });
          isMyStaticIPSet=true;
          gserver.on("/ip", HTTP_GET, [](AsyncWebServerRequest *request){
                  request->send(200, "text/plain", AP_StaticIP.c_str());
                  startAPP=true;
          });
          gserver.on("/url", HTTP_GET, [](AsyncWebServerRequest *request){
              mDebugMsgcpp("url  HTTP_GET");
                  request->send(200, "text/plain", LM_URL.c_str());
                  startAPP=true;
          });

        });
           gserver.on("/startapp", HTTP_GET, [](AsyncWebServerRequest *request){
                 //Send MeCFES bridgeapp
                 request->send(SPIFFS, "/bridgeAPP.html", "text/html");
                 startAPP=true;
                 stopsoftAP=true;

          });
              //Wait here until user has submitted the response in startapp (startAPP==true)
    mDebugMsgcpp("Waiting for user in mUserFeedbackViaSoftAP");
    return mWaitUntilTrueOrTimeout(startAPP);
    */
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
  if (nDbgLvel>4){ Serial.print("Saved ssid (AP_SSID): ");  Serial.println(AP_SSID);}
  AP_PASS=readFile(SPIFFS, "/Password.txt");
  if (nDbgLvel>4){ Serial.print("Password (AP_PASS): ");  Serial.println(AP_PASS);}
  LM_URL=readFile(SPIFFS, "/Url.txt");
  if (nDbgLvel>4){ Serial.print("server (LM_URL): ");  Serial.println(LM_URL);}
  AP_StaticIP=readFile(SPIFFS, "/IP.txt");
  if (nDbgLvel>4){ Serial.print("IP.txt (AP_StaticIP): ");  Serial.println(AP_StaticIP);}
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
  WiFi.softAP(SoftAP_SSID);
  delay(500);
  bool ret1=WiFi.softAPConfig(SoftAP_IP, gateway, subnet);
  Serial.print("Setting direct wifi (soft-AP) server:");
  Serial.println(ret1 ? "Ready" : "Failed!");
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
           request->send(SPIFFS, "/bridgeAPP.html", "text/html");
          // startAPP=true;
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
        //request->send(SPIFFS, "/onConnection.html", "text/html");
      }
    });
    /*
      delay(10000);
      InitSoftAPOk=true;   //Proceed in flowchart
    } else {
      mDebugMsgcpp("No message sent");
    }
    }
  );
  /*
  gserver.on("/url", HTTP_GET, [](AsyncWebServerRequest *request){
     mDebugMsgcpp("index.html Sending url to client");
                request->send(200, "text/plain", LM_URL.c_str());
                startAPP=true;
        });
  gserver.on("/ssid", HTTP_GET, [](AsyncWebServerRequest *request){
       mDebugMsgcpp("index.html Sending AP_SSID to client");
      request->send(200, "text/plain", AP_SSID.c_str());
  });
  gserver.on("/ip", HTTP_GET, [](AsyncWebServerRequest *request){
       mDebugMsgcpp("index.html Sending ip to client");
      request->send(200, "text/plain",AP_StaticIP.c_str());
  });

  // Send a GET request to <ESP_IP>/get?input1=<inputMessage>
  // Obsolete HTTP_GET by HTTP_POST
  gserver.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
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
    gserver.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
   request->send(SPIFFS, "/style.css", "text/css");
 });
   gserver.on("/ssid", HTTP_GET, [](AsyncWebServerRequest *request){
     mDebugMsgcpp("Sending AP_SSID Via GET to client");
    request->send(200, "text/plain", AP_SSID.c_str());
    InitSoftAPOk=true;
    });
   gserver.on("/ip", HTTP_GET, [](AsyncWebServerRequest *request){
     mDebugMsgcpp("Sending sMyStaticIP to client");
    request->send(200, "text/plain", AP_StaticIP.c_str());
    startAPP=true;
    });
    gserver.on("/url", HTTP_GET, [](AsyncWebServerRequest *request){
                  request->send(200, "text/plain", LM_URL.c_str());
                  startAPP=true;
          });
  });
  gserver.on("/startapp", HTTP_GET, [](AsyncWebServerRequest *request){
           //Send MeCFES bridgeapp
           request->send(SPIFFS, "/bridgeAPP.html", "text/html");

           startAPP=true;
    });
    */
  gserver.onNotFound(notFound);
  gserver.begin();
  Serial.println("HTTP server started");
  mDebugMsgcpp("Waiting for user to insert credentials");
  //"Continue if good. else if Failed to get credentials, abort");
  bool ret= mWaitUntilTrueOrTimeout(InitSoftAPOk);
  mDebugMsgcpp("InitSoftAP Done");
  return ret;
}





//  *** FLASH files
//Read credentials from files (SSID.txt,Password.txt and IP.txt)
String readFile(fs::FS &fs, const char * path){
  if (nDbgLvel==6) Serial.printf("Reading file: %s\r\n", path);
  File file = fs.open(path, "r");
    if(!file || file.isDirectory()){
            Serial.println("- empty file or failed to open file");
            return String();
  }
if (nDbgLvel==6)  Serial.println("- read from file:");
  String fileContent;
  while(file.available()){
    fileContent+=String((char)file.read());
  }
if (nDbgLvel==6)  Serial.println(fileContent);
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
