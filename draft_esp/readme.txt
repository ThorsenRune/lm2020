File structure

*.ino the executable arduino File

credentials.h    -    WIFI settings, depends on network
ignored in git by .gitignore settings
{
  contains:
  const char* AP_SSID = "networkname";  // your router's SSID here
  const char* AP_PASS = "password";  // your router's password here
  int MyStaticIP[4]={192, 168, 1, 51};
}
