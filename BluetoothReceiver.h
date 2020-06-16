#ifndef BLUETOOTH_RECEIVER_H
#define BLUETOOTH_RECEIVER_H

#include <BLEServer.h>

class BLEServer;
class BLECharacteristic;
class ReceivedPacket;
class Controller;

class BluetoothReceiver : public BLEServerCallbacks, public BLECharacteristicCallbacks {
  private:
    BLECharacteristic *pCharacteristic;
    bool deviceConnected = false;
    std::map<int, ReceivedPacket*> packets;
  public:
    BluetoothReceiver(Controller & controller);
    void init();

    // BLE lib needed functions.
    void onConnect(BLEServer* pServer);
    void onDisconnect(BLEServer* pServer);
    void onWrite(BLECharacteristic *pCharacteristic);

    void onTick(int tick);
};

#endif
