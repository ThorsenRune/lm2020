#include "getWiFiCreds.h"

bool mWIFIConnect1(){//Main entry point - a blocking call
  mDebugMsg("Executing: mWIFIConnect1");
  //Get credentials from SPIFFS (Flowchart 0)
  bool ret=mGetCredentials1();
  //If  credentals  try to connect (Flowchart 1)
  if (ret){
    mDebugMsg("Setting up the websocket, connect to MyStaticIP");
    bool ret=mStartWebSocket();//(char*(AP_SSID), char*(AP_PASS),MyStaticIP); //Setup the static IP obtained
    if (ret) return true; //Tell caller to proceed
  } else {  //Fail in websocket connection, get credentials via SoftAP
            //(Flowchart 2)
    mDebugMsg("Calling InitSoftAP ");
    bool ret=InitSoftAP();//Sets AP_SSID, AP_PASS by Setup a soft accesspoint 192.168.4.1 and ask the user for credentials
      //The InitSoftAP will return the parameters
      //connect to network and get the IP
      mDebugMsg("Done InitSoftAP, Calling mGetMyStaticIP ");
    if (ret) ret=mGetMyStaticIP();//(AP_SSID, AP_PASS,MyStaticIP);
    if (ret){ //We got our credentials, save and restart
      mDebugMsg("Calling: mUserFeedbackViaSoftAP");
        //Setup the SoftAP from before, refresh client with full credentials
        ret=mUserFeedbackViaSoftAP(); //Arguments AP_SSID, AP_PASS,MyStaticIP as globals
        if (ret) { // credentials ready save them
          mSetCredentials();//AP_SSID, AP_PASS,MyStaticIP);
        }
        return mWIFIConnect(); //If credentials have been saved now the recursive call should end width
          //  --->mStartWebSocket
    } else {  //Fail in getting credentials
        mDebugMsg("Fail in getting credentials, retry");
        return false;
    }

  }
  return false; //Tell caller that we are waiting for a client to connect
}
//=========================		END OF CONNECTION  CALL  ====================

//--------      Global VARIABLES
String AP_SSID  ;  // your internet wifi  SSID
String AP_PASS ;   // your internet wifi  password

//--------			SUB functions	----------------



bool mGetCredentials1(){//Blocking. Will return true if credentials are in FLASH
  //RT210112: Moved code into method
  AP_SSID=readFile(SPIFFS, "/SSID.txt");
  Serial.print("Your ssid: ");
  Serial.println(AP_SSID);
  AP_PASS=readFile(SPIFFS, "/Password.txt");
  Serial.print("Your password: ");
  Serial.println(AP_PASS);
  sMyStaticIP = readFile(SPIFFS, "/IP.txt");
  MyStaticIP=String2IpAddress(sMyStaticIP);
  Serial.print("Your IP: ");
  Serial.println(MyStaticIP);
  if ((bEmptyString==AP_SSID)|| (bEmptyString==sMyStaticIP)||(bEmptyString==AP_PASS)) {
    mDebugMsg("Missing credentals files");
    return false;   //Invalid file
  } else{           //We have credentials
    isMyStaticIPSet=true;
    return true;
  }
}
