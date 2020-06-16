#include "BluetoothReceiver.h"

#include "Packet.h"

// BLE
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// Send debug messages
#include <HardwareSerial.h>


// NOTE: These NEED to be uppercase.
#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART service UUID
#define CHARACTERISTIC_UUID_RX "6EED7E34-9F2A-4F0F-B1D6-70CD04E8E581"
#define CHARACTERISTIC_UUID_TX "1CF8D309-11A3-46FB-9378-9AFFF7DCE3B4"

BluetoothReceiver::BluetoothReceiver(Controller & controller) {
  packets[1] = new GetPWMPacket(controller);
  packets[2] = new GetLEDStripsPacket(controller);
  packets[3] = new AddPWMDriverPacket(controller);
  packets[5] = new AddLEDStripPacket(controller);
  packets[8] = new AddColorSequencePacket(controller);
  packets[9] = new SetLEDStripColorSequencePacket(controller);
}

void BluetoothReceiver::init() {
  // Create the BLE Device
  BLEDevice::init("LEDs"); // Give it a name

  // Create the BLE Server
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(this);

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);
  // Make it possible to find this device based on the UUID.
  pServer->getAdvertising()->addServiceUUID(SERVICE_UUID);

  // Create a BLE Characteristic
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID_TX,
                      BLECharacteristic::PROPERTY_NOTIFY
                    );
                      
  pCharacteristic->addDescriptor(new BLE2902());

  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID_RX,
                                         BLECharacteristic::PROPERTY_WRITE
                                       );

  pCharacteristic->setCallbacks(this);

  // Start the service
  pService->start();

  // Start advertising
  pServer->getAdvertising()->start();
  Serial.println("Waiting a client connection to notify...");
}

void BluetoothReceiver::onConnect(BLEServer* pServer) {
  deviceConnected = true;
};

void BluetoothReceiver::onDisconnect(BLEServer* pServer) {
  deviceConnected = false;
}

void BluetoothReceiver::onWrite(BLECharacteristic *pCharacteristic) {
  std::string rxValue = pCharacteristic->getValue();

  if (rxValue.length() > 0) {
    int packetID = rxValue[0];

    auto packetItr = packets.find(packetID);
    if (packetItr == packets.end()) {
      Serial.println("Unknown packet! ");
      Serial.println(packetID);
      // TODO: Send error packet back to phone.
    } else {
      std::string trimmed = rxValue.substr(1);
      packetItr->second->parse(trimmed);
    }
    
    /*Serial.println("*********");
    Serial.print("Received Value: ");

    for (int i = 0; i < rxValue.length(); i++) {
      Serial.print(rxValue[i]);
    }

    Serial.println();*/

    /*// Do stuff based on the command received from the app
    if(rxValue.length() > 1) {
      int channel = atoi(rxValue.substr(0,1).c_str());
      pwm.setPWM(channel, 0, atoi(rxValue.substr(1).c_str()));
    } else {
      
    }*/

    Serial.println();
    Serial.println("*********");
  }
}

void BluetoothReceiver::onTick(int tick) {
  // Heartbeat every 5 seconds
  if (tick % 5 * 60 == 0) {
    pCharacteristic->setValue("Heartbeat");
    pCharacteristic->notify();
  }
}
