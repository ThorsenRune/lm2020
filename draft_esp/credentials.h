// REPLACE WITH YOUR NETWORK CREDENTIALS
#ifndef ___CREDSH    // only once guard to avoid recursive inclusion
#define ___CREDSH

String AP_SSID = "networkname";  // your router's SSID here
String AP_PASS = "password";     // your router's password here
String LM_URL = "url";
const char* SoftAP_SSID = "Arduino_LM";  //Name of the SoftAP - Arduino gets nicely first in the network list
//Vedi ref201221 in google docs ...
<<<<<<< HEAD
IPAddress MyStaticIP;  //The static IP address when using internet wifi router
=======
//IPAddress MyStaticIP;  //The static IP address when using internet wifi router
>>>>>>> ccd45bcee06e67998c5b636bc35da41fcec307d6

#endif  //___CREDSH
