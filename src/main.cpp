#include <Arduino.h>
#include "time.h"

/*
		Video: https://www.youtube.com/watch?v=oCMOYS71NIU
		Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleNotify.cpp
		Ported to Arduino ESP32 by Evandro Copercini

	 Create a BLE server that, once we receive a connection, will send periodic notifications.
	 The service advertises itself as: 6E400001-B5A3-F393-E0A9-E50E24DCCA9E
	 Has a characteristic of: 6E400002-B5A3-F393-E0A9-E50E24DCCA9E - used for receiving data with "WRITE" 
	 Has a characteristic of: 6E400003-B5A3-F393-E0A9-E50E24DCCA9E - used to send data with	"NOTIFY"

	 The design of creating the BLE server is:
	 1. Create a BLE Server
	 2. Create a BLE Service
	 3. Create a BLE Characteristic on the Service
	 4. Create a BLE Descriptor on the characteristic
	 5. Start the service.
	 6. Start advertising.

	 In this example rxValue is the data received (only accessible inside that function).
	 And txValue is the data to be sent, in this example just a byte incremented every second. 
*/
#include <cstdlib>


// PWM
#include <Wire.h>

#include "packets/received_packets/ReceivedPackets.h"
#include "Controller.h"

// Controller
Controller controller;

// TIMER
volatile int interruptCounter;
int totalInterruptCounter;

hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
 
void IRAM_ATTR onTimer() {
	portENTER_CRITICAL_ISR(&timerMux);
	interruptCounter++;
	portEXIT_CRITICAL_ISR(&timerMux);
}

void setup() {
	Serial.begin(115200);

	timer = timerBegin(0, 80, true);
	timerAttachInterrupt(timer, &onTimer, true);
	timerAlarmWrite(timer, 16666 /*100000*/, true);
	timerAlarmEnable(timer);
	
	// if you want to really speed stuff up, you can go into 'fast 400khz I2C' mode
	// some i2c devices dont like this so much so if you're sharing the bus, watch
	// out for this!
	Wire.setClock(400000);

	controller.init();
}

void loop() {
	if (interruptCounter > 0) {
		portENTER_CRITICAL(&timerMux);
		interruptCounter--;
		portEXIT_CRITICAL(&timerMux);
 
		totalInterruptCounter++;
		controller.onTick(totalInterruptCounter);
		
		/*Serial.println("Loop");
		if (deviceConnected) {
	
			// NOTE: Max packet size is 20 bytes!
			std::string strToSend = "It worked! abcdefghijklmnopqrstuvwxyz0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
			pCharacteristic->setValue(strToSend);
			
			pCharacteristic->notify(); // Send the value to the app!
			Serial.println("*** Sent Value ***");
	
			// You can add the rxValue checks down here instead
			// if you set "rxValue" as a global var at the top!
			// Note you will have to delete "std::string" declaration
			// of "rxValue" in the callback function.
	//		if (rxValue.find("A") != -1) { 
	//			Serial.println("Turning ON!");
	//			digitalWrite(LED, HIGH);
	//		}
	//		else if (rxValue.find("B") != -1) {
	//			Serial.println("Turning OFF!");
	//			digitalWrite(LED, LOW);
	//		}
		}*/
	}
}