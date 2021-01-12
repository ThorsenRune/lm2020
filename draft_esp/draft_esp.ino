/*
 *
(0) credentials&IP=Read flash (SPIFF)
*
*
(1) Connect to network  (!!!!! Let the user know which network the device is connected)

|-fail—> Setup SoftAP (2)

|-success-->Wait for client

    on connect

    |--> LM Program

    |--(timeout) —> Jump to (2)
*
*
*
*
(2) Setup SoftAP

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


AsyncWebServer server(80);

// Configure SoftAP characteristics
const char* ssid_softap = "MeCFES_Config";

const char* password = "12345678";
int hidden=0;    //optional parameter, if set to true will hide SSID.
int channel=1;        //optional parameter to set Wi-Fi channel, from 1 to 13. Default channel = 1.
int max_connection=4; //optional parameter to set max simultaneous connected stations, from 0 to 8. Defaults to 4.
                      //Once the max number has been reached, any other station that wants to connect will be forced to wait until an already connected station disconnects.
String IPvalue="12.0.0.1";
String ssidvalue;
String passwordvalue;

const char* PARAM_INPUT_1 = "SSID";
const char* PARAM_INPUT_2 = "Password";


//IP Address settings
IPAddress local_IP(192,168,4,1);
IPAddress gateway(192,168,4,9);
IPAddress subnet(255,255,255,0);


//HTML page to show IP and successful connection message to user
const char Onconnection_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<style>
html, body {
  height: 100%;
  background: linear-gradient(to bottom right, #ccffff 0%, #ffccff 100%);
}

h2  {

  font-family: cabin;
  padding: 10px;
  text-align: center;
  }

h1  {

  font-family: cabin;
  padding: 10px;
  text-align: center;
  }

h3  {

  font-family: cabin;
  padding: 10px;
  text-align: center;
  }
p    {
font-family: big caslon;
padding: 10px;
text-align: center;
}
form
{
  text-align: center;
  font-family: big caslon;
}
input[type=submit] {
  text-align: center;
  border: none;
  color: black;
  padding: 16px 32px;
  text-decoration: none;
  margin: 4px 2px;
  font-family: big caslon;
  border-radius: 4px;
  cursor: pointer;
}
input[type=text], input[type=password] {
  width: 100%;
  padding: 12px 20px;
  margin: 2px 2px;
  box-sizing: border-box;
  border-radius: 4px;
}
</style>
<head>
  <title>MeCFES: Connessione completata!</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  </head><body>
  <h2> MeCFES: Connessione completata!</h2>
  <p>La connessione alla rete wifi e' stata completata con successo. Ecco i dati relativi a SSID ed indirizzo IP della rete. </p>
  <br>
  <p><u>Non dimenticarti di annotarli, saranno necessari per la lettura dei dati dal dispositivo MeCFES.</u></p>
 <br>
 <br>
 <h3>SSID WiFi:</h3>
 <br>
 <h1><span id="SSID">%SSID%</span></h1>
 <br>
 <h3>IP:</h3>
 <br>
 <h1><span id="IP">%IP%</span></h1>
</body>
</html>)rawliteral";



// Function to replace placeholders with SSID and IP values
String processor(const String& var){
  if(var == "SSID"){
    return String(ssidvalue);
  }
  else if (var == "IP")
  {
    return String(IPvalue);
  }
  return String();
}


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





void InitSoftAP() {
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
      ssidvalue = request->getParam(PARAM_INPUT_1)->value();
      passwordvalue = request->getParam(PARAM_INPUT_2)->value();
      writeFile(SPIFFS, "/SSID.txt", ssidvalue.c_str());
      writeFile(SPIFFS, "/Password.txt", passwordvalue.c_str());
    }
    else {
      ssidvalue = "No message sent";
      passwordvalue = "No message sent";
    }
    // Send web page with input fields to client
    request->send(SPIFFS, "/onConnection.html", "text/html");

  });
  server.onNotFound(notFound);
  server.begin();

}




void setup() {
 Serial.begin(115200);
 if(!SPIFFS.begin(true)){
  Serial.println("An Error has occurred while mounting SPIFFS");
  return;
  }
  //Todo1: change InitSoftAP to return credentials
 InitSoftAP(&AP_SSID, &AP_PASS);  //Setup a soft accesspoint 192.168.4.1 and ask the user for credentials
 mGetStaticIP(&AP_SSID, &AP_PASS, int &MyStaticIP[4]);// Get static IP for the network
 mStartWebSocket(staticIP); //Setup the static IP obtained
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

void loop() {
  String Credential1 = readFile(SPIFFS, "/SSID.txt");
  Serial.print("Your ssid: ");
  Serial.println(Credential1);
   String Credential2 = readFile(SPIFFS, "/Password.txt");
  Serial.print("Your password: ");
  Serial.println(Credential2);
   String Credential3 = readFile(SPIFFS, "/IP.txt");
  Serial.print("Your IP: ");
  Serial.println(Credential3);

}
