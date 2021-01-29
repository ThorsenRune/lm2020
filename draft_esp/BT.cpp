/*
		Handling wireless transmissions
			bluetooth

*/
//#define DEBUG_ON  //Skip compiling codeblocks
#ifndef    DEBUG_ON
#include "BT.h"
#include "BLE2902.h"

//Define Service, Characteristic and Descriptor
#define LMService BLEUUID("783b26f8-740d-4187-9603-82281d6d7e4f")
BLECharacteristic LMCharacteristic(BLEUUID("1bfd9f18-ae1f-4bba-9fe9-0df611340195"), BLECharacteristic::PROPERTY_READ  | BLECharacteristic::PROPERTY_WRITE  | BLECharacteristic::PROPERTY_NOTIFY);
BLEDescriptor LMDescriptor(BLEUUID("2f562183-0ca1-46be-abd6-48d0be28f83d"));

//Identify BT connection
bool isBTConnected = false;


//Control if the BT is connected
class MyServerCallbacks : public BLEServerCallbacks {
    void onBTConnect(BLEServer* pServer) {
     isBTConnected = true;
    };

    void onBTDisconnect(BLEServer* pServer) {
      isBTConnected = false;
    }
};


/* ###############################################################  CALL back to receive data from Phone */

class MyCallbacks: public BLECharacteristicCallbacks {

    void onBTWrite(BLECharacteristic *pCharacteristic) {
      std::string rxValue = pCharacteristic->getValue();
      //Serial.println(rxValue[0]);

      if (rxValue.length() > 0) {
       // Serial.println("*********");
       // Serial.print("Received Value: ");

        for (int i = 0; i < rxValue.length(); i++) {
         //Serial.print(rxValue[i]);
        }
        //Serial.println();
        //Serial.println("*********");
      }

    }
};
/* ############################################################### */

void InitBLE() {
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
}
		#endif
