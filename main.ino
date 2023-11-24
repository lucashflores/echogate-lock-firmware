/*
 * WebSocketClient.ino
 *
 *  Created on: 24.05.2015
 *
 */

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <Servo.h>
#include <string.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>

//#include <Hash.h>

ESP8266WiFiMulti WiFiMulti;
WebSocketsClient webSocket;
//uint8_t SERVO_Pin = D4;
Servo s;
int pos = 1;
//#define LED 16

#define USE_SERIAL Serial


void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {

	switch(type) {
		case WStype_DISCONNECTED:
			USE_SERIAL.printf("[WSc] Disconnected!\n");
			break;
		case WStype_CONNECTED: {
			USE_SERIAL.printf("[WSc] Connected to url: %s\n", payload);
      DynamicJsonDocument doc(1024);
      doc["event"] = "LockClient";
      if (pos < 180) {
        doc["data"] = "locked";
      }
      else {
        doc["data"] = "unlocked";
      }
      
      String output;
      serializeJson(doc, output);
      webSocket.sendTXT(output);
			// send message to server when Connected
			webSocket.sendTXT("Connected");
		}
			break;
		case WStype_TEXT:
			USE_SERIAL.printf("[WSc] get text: %s\n", payload);
      if (String((char*)payload) == "lock") {
        USE_SERIAL.printf("lock");
        pos = 0;
        s.write(pos);
        DynamicJsonDocument doc(1024);
        doc["event"] = "lockState";
        doc["data"] = "locked";
        String output;
        serializeJson(doc, output);
        webSocket.sendTXT(output);
			}
      else if (String((char*)payload) == "unlock") {
        USE_SERIAL.printf("unlock");
        pos = 180;
        s.write(pos);
        DynamicJsonDocument doc(1024);
        doc["event"] = "lockState";
        doc["data"] = "unlocked";
        String output;
        serializeJson(doc, output);
        webSocket.sendTXT(output);

			}
			break;
		case WStype_BIN:
			USE_SERIAL.printf("[WSc] get binary length: %u\n", length);
			hexdump(payload, length);

			// send data to server
			// webSocket.sendBIN(payload, length);
			break;
        case WStype_PING:
            // pong will be send automatically
            USE_SERIAL.printf("[WSc] get ping\n");
            break;
        case WStype_PONG:
            // answer to a ping we send
            USE_SERIAL.printf("[WSc] get pong\n");
            break;
		case WStype_ERROR:
			USE_SERIAL.printf("[WSc] Error!\n");
			break;
		default:
			USE_SERIAL.printf("caiu no default");
			break;
	}

}



void setup() {
	// USE_SERIAL.begin(921600);
	USE_SERIAL.begin(9600);
	// pinMode(LED, OUTPUT);
  s.attach(2);
	//Serial.setDebugOutput(true);
	USE_SERIAL.setDebugOutput(true);
  s.write(pos);

	USE_SERIAL.println();
	USE_SERIAL.println();
	USE_SERIAL.println();
  

	for(uint8_t t = 4; t > 0; t--) {
		USE_SERIAL.printf("[SETUP] BOOT WAIT %d...\n", t);
		USE_SERIAL.flush();
		delay(1000);
	}
	
	WiFiMulti.addAP("VIVOFIBRA-A420", "F92BDAECD3");

	//WiFi.disconnect();
	while(WiFiMulti.run() != WL_CONNECTED) {
		delay(100);
	}

	// server address, port and URLcc
	webSocket.begin("192.168.15.5", 3000, "/websocket");

	// event handler
	webSocket.onEvent(webSocketEvent);

	// try ever 5000 again if connection has failed
	webSocket.setReconnectInterval(5000);
}

void loop() {
  webSocket.loop();
}

