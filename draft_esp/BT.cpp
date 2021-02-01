/*
		Handling wireless transmissions
			bluetooth

*/
#define DEBUG_ON  //Skip compiling codeblocks
#ifndef    DEBUG_ON
#endif
#include "BT.h"
<<<<<<< HEAD
#include "transmit.h"

=======
#include <Arduino.h>
#include <stdint.h>           //Define standard types uint32_t etc
#include <stdbool.h>				//Boolan types rt210107
#include "debug.h"   //Enables debugging with   DUMP(someValue);  TRACE();
>>>>>>> 7d8a0b1b356bee62971cde04f97cb33f38967f2d

//Define Service, Characteristic and Descriptor
#define LMService BLEUUID("783b26f8-740d-4187-9603-82281d6d7e4f")
BLECharacteristic LMCharacteristic(BLEUUID("1bfd9f18-ae1f-4bba-9fe9-0df611340195"), BLECharacteristic::PROPERTY_READ  | BLECharacteristic::PROPERTY_WRITE  | BLECharacteristic::PROPERTY_NOTIFY);
BLEDescriptor LMDescriptor(BLEUUID("2f562183-0ca1-46be-abd6-48d0be28f83d"));

//Identify BT connection
bool isBTConnected = false;


//Control if the BT is connected
class MyServerCallbacks : public BLEServerCallbacks {
    //Todo:@FC when is this metod invoked?
    void onBTConnect(BLEServer* pServer) {
      DEBUG(1,"BT Connected");
     isBTConnected = true;
    };

    void onBTDisconnect(BLEServer* pServer) {
      isBTConnected = false;
      DEBUG(1,"BT Dis-Connected");
    }
};


/* ###############################################################  CALL back to receive data from Phone */

class MyCallbacks: public BLECharacteristicCallbacks {
    //Reveive data from client
    //todo:fc see _WsEvent
    void onBTWrite(BLECharacteristic *pCharacteristic) {
      std::string myString = pCharacteristic->getValue();
      size_t len=myString.length();
      std::vector<uint8_t> myVector(myString.begin(), myString.end());
      uint8_t *rxValue = &myVector[0];
      //Serial.println(rxValue[0]);

      if (myString.length() > 0) {
       // Serial.println("*********");
       // Serial.print("Received Value: ");
<<<<<<< HEAD
         mReceive2(rxValue,myString.length());
=======
       //todo0: fix dataformat because :
       //mReceive2 expects uint8_t *data, size_t len
      //   mReceive2(rxValue,rxValue.length());
>>>>>>> 7d8a0b1b356bee62971cde04f97cb33f38967f2d
         //todo: rxValue must be an int array
         //rxValue.length() must be an int
        //for (int i = 0; i < myString.length(); i++) {
         //Serial.print(rxValue[i]);
        //}
        //Serial.println();
        //Serial.println("*********");
      }
    }
};
/* ############################################################### */

bool InitBLE() {
//Set BT name
  BLEDevice::init("MeCFES");
// Create the BLE Server
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

// Create the BLE Service
  BLEService *pLM = pServer->createService(LMService);

//Create and add the characteristic to pBattery service
  pLM->addCharacteristic(&LMCharacteristic);
//Attach descriptor
  LMDescriptor.setValue("Data sent with LM Protocol");
  LMCharacteristic.addDescriptor(&LMDescriptor);
  LMCharacteristic.addDescriptor(new BLE2902());

  /* ###############################################################  define callback */
//Define the command for Phone-to-Arduino communication handling
  LMCharacteristic.setCallbacks(new MyCallbacks());
  /* ############################################################### */

//Add UUID to the custom service
  pServer->getAdvertising()->addServiceUUID(LMService);
//Start service
  pLM->start();

// Start advertising
  pServer->getAdvertising()->start();
  return true;
}
