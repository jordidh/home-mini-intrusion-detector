/*
 * Desc: Home mini intrusion detection
 *       Inputs: 
 *        * D2 input with pullup resistance to read the value of the sensor MC-38 wired door window sensor magnetic switch
 *        * D3 input with pullup resistance to read the value of the sensor MC-38 wired door window sensor magnetic switch
 *        * D5 input echo value for sensor HC-SR04
 *       Outputs:
 *        * D4 output trigger for sensor HC-SR04
 * Author: JDH
 * Date: 13/03/2017
 * Version: 1.0
 * IDE configuration: https://www.wemos.cc/tutorial/get-started-arduino.html
 */
#include <ESP8266WiFi.h>
#include <OpenCloseSensor.h>
#include <PresenceSensor.h>

#include <FirebaseCloudMessaging.h>
#include <FirebaseHttpClient.h>
#include <Firebase.h>
#include <FirebaseObject.h>
#include <FirebaseError.h>
#include <FirebaseArduino.h>

/* 
 *  This file must have the following constants:
 * #define WIFI_SSID "********"
 * #define WIFI_PASSWORD "********"
 * #define FIREBASE_HOST "********"
 * #define FIREBASE_AUTH "********"
 * 
 * const String HOME_NAME = "********";
 * const String DEVICE_NAME = "********";
 * #define OC_COUNT ?
 * OpenCloseSensor ocSensors[OC_COUNT] = {
 *   OpenCloseSensor(...),
 *   ...
 * };
 * #define P_COUNT ?
 * PresenceSensor pSensors [P_COUNT] = {
 *   PresenceSensor(...),
 *   ...
 * };
 */
#include "config.h"


WiFiServer server(80);

int ledPin = BUILTIN_LED; //BUILTIN_LED


String toFirebase()
{
  return "home/home:" + HOME_NAME + "/device/device:" + DEVICE_NAME + "/";
}

String toJSON(String sensors)
{
  return "{ \"home\":\"" + HOME_NAME + "\", \"device\": { \"device\":\"" + DEVICE_NAME + "\", \"sensors\": [" + sensors + "] } }";
}


String prepareHtmlPage()
{
  String htmlSensors = "";

  for (int i=0; i<OC_COUNT; i++) {
    htmlSensors += "<p>Sensor OC " + String(i) + " = " + ocSensors[i].lastValueReadToString() + "</p>";
  }

  for (int i=0; i<P_COUNT; i++) {
    htmlSensors += "<p>Sensor P " + String(i) + " = " + ocSensors[i].lastValueReadToString() + "</p>";
  }
  
  String htmlPage = 
     String("HTTP/1.1 200 OK\r\n") +
            "Content-Type: text/html\r\n" +
            "Connection: close\r\n" +  // the connection will be closed after completion of the response
            "Refresh: 10\r\n" +  // refresh the page automatically every 5 sec
            "\r\n" +
            "<!DOCTYPE HTML>" +
            "<html>" +
            "<H1>" + HOME_NAME + " - " + DEVICE_NAME + "<H1>" +
            htmlSensors +
            "</html>" +
            "\r\n";
  return htmlPage;
}

String prepareJsonPage()
{
  String jsonSensors = "";

  for (int i=0; i<OC_COUNT; i++) {
    jsonSensors += ocSensors[0].toJSON() + ", ";
  }

  for (int i=0; i<P_COUNT; i++) {
    jsonSensors += pSensors[0].toJSON() + ", ";
  }

  if (jsonSensors.length() > 2) {
    jsonSensors = jsonSensors.substring(0, jsonSensors.length() - 2);
  }
  
  String jsonPage = 
     String("HTTP/1.1 200 OK\r\n") +
            "Content-Type: application/json\r\n" +
            "Connection: close\r\n" +  // the connection will be closed after completion of the response
            "Refresh: 10\r\n" +  // refresh the page automatically every 5 sec
            "\r\n" +
            toJSON(jsonSensors) +
            "\r\n";
  return jsonPage;
}


void setup() {
  Serial.begin(115200);
  
  // Setup IO
  pinMode(ledPin, OUTPUT);  // set onboard LED as output
  digitalWrite(ledPin,LOW);   //LOW = 0, HIGH = 1

  // WiFi connection
  Serial.printf("Connecting to %s ", WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" connected");

  // Web Server
  server.begin();
  Serial.printf("Web server started, open %s in a web browser\n", WiFi.localIP().toString().c_str());

  //Firebase
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Serial.printf("Firebase started");
}

void loop() {
  long distanceDetected;
  String firebaseMessage = "";
  String firebaseValue = "";
  bool hasChanged = false;
  bool isOpen = false;
  bool hasDetected = false;

  for (int i=0; i<OC_COUNT; i++) {
    isOpen = ocSensors[i].isOpen(&hasChanged);
    if (hasChanged) {
      firebaseValue = ocSensors[i].lastValueReadToString();
  
      //Write results to firebase
      firebaseMessage = toFirebase() + ocSensors[i].toFirebaseDB(0);
      Firebase.setString(firebaseMessage, firebaseValue);
      // handle error
      if (Firebase.failed()) {
        Serial.print("setting ocSensor1 failed:");
        Serial.println(Firebase.error());  
      }
    }
  }

  for (int i=0; i<P_COUNT; i++) {
    hasDetected = pSensors[i].presenceDetected(&distanceDetected, &hasChanged);
    if (hasChanged) {
      firebaseValue = pSensors[i].lastValueReadToString();
      //Write results to firebase
      firebaseMessage = toFirebase() + pSensors[i].toFirebaseDB(0);
      Firebase.setString(firebaseMessage, firebaseValue);
      // handle error
      if (Firebase.failed()) {
          Serial.print("setting presenceDetected failed:");
          Serial.println(Firebase.error());  
      }
  
      firebaseMessage = toFirebase() + pSensors[i].toFirebaseDBDistance(0);
      Firebase.setFloat(firebaseMessage, pSensors[i].lastDistanceRead());
      // handle error
      if (Firebase.failed()) {
          Serial.print("setting presenceDetected failed:");
          Serial.println(Firebase.error());  
      }
    }
  }

  delay(1000);

  WiFiClient client = server.available(); 
  // wait for a client (web browser) to connect
  if (client)
  {
    Serial.println("\n[Client connected]");
    while (client.connected())
    {
      // read line by line what the client (web browser) is requesting
      if (client.available())
      {
        String line = client.readStringUntil('\r');
        Serial.print(line);
        // wait for end of client's request, that is marked with an empty line
        if (line.length() == 1 && line[0] == '\n')
        {
          client.println(prepareJsonPage());
          break;
        }
      }
    }
    delay(1); // give the web browser time to receive the data

    // close the connection:
    client.stop();
    Serial.println("[Client disonnected]");
  }

  delay(1000);
}
