 /*
    This module will get the WiFi credentials stored in the FLASH memory
    If the credentals are not valid, it will setup a direct wifi at (todo8: always same address?)
    The client can connect to the local wifi network and insert the credentials of the internetwifi

 */
 /*
 Flowchart of mWIFIConnect()
 mWIFIConnect will return true if connection is established and the program can
 proceed to main

 (0) credentials&IP=Read flash (SPIFF)
       |
       v
 (1) Connect to network
         |-fail—> Setup SoftAP (2) {InitSoftAP}
         |-success-->Wait for client
         V
     on connect
         |--> LM Program
         |--(timeout) —> Jump to (2)
         =

 (2) Setup SoftAP {InitSoftAP}
       |--> Connect to client via Soft AP
               | Get credentials from client. User writes SSID & Password
               | connect to network and get the IP
               | reconnect to client via Soft AP
               | send IP to client. Now user will know the IP, create a link to click
               | save credentials&IP to FLASH (SPIFF)
       *------*
       |
   Jump to (1)
  */
