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
/*#include <ESP8266WiFi.h>*/

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
//#include "config.h"


#define ledPin BUILTIN_LED  // BUILTIN_LED = PIN D4
#define USOUND_TRIG  D6     // PIN D6
#define USOUND_ECHO  D5     // PIN D5
#define RELAY  D1

long DISTANCE_THRESHOLD = 50; // In centimeters

bool presenceDetected = false;
long distanceRead;


//WiFiServer server(80);


void setup() {
  //String firebaseMessage = "";

  Serial.begin(115200);

  //Configure the Wemos PINs
  pinMode(USOUND_TRIG,OUTPUT);
  pinMode(USOUND_ECHO,INPUT);
  pinMode(RELAY,OUTPUT);
  pinMode(ledPin, OUTPUT);  // set onboard LED as output (D4)

  //Initialize the output value PINs
  digitalWrite(USOUND_TRIG,HIGH);
  digitalWrite(RELAY,LOW);
  digitalWrite(ledPin,LOW);

/*
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
  */
}

void loop() {

  digitalWrite(ledPin,HIGH);   //LOW = 0, HIGH = 1

  delay(1000);

  digitalWrite(ledPin,LOW);   //LOW = 0, HIGH = 1


  //Get the water level and start or stop the water pump
  presenceDetected = PresenceDetected(USOUND_TRIG, USOUND_ECHO, &distanceRead, DISTANCE_THRESHOLD);
  Serial.println(distanceRead);
  if (presenceDetected) {
    digitalWrite(RELAY, HIGH);
    Serial.println("Relé HIGH");
  }
  else {
    digitalWrite(RELAY, LOW);
    Serial.println("Relé LOW");
  }


/*
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
          client.println("");
          break;
        }
      }
    }
    delay(1); // give the web browser time to receive the data

    // close the connection:
    client.stop();
    Serial.println("[Client disonnected]");
  }
  */

  delay(1000);
}



/* Method that detects if there is an object near the sensor
 * Parameters:
 *
 * (1) Using IO trigger for at least 10us high level signal,
 * (2) The Module automatically sends eight 40 kHz and detect whether there is a pulse signal back.
 * (3) IF the signal back, through high level , time of high output IO duration is the time from sending ultrasonic to returning.
 * Test distance = (high level time×velocity of sound (340M/S) / 2,
 */
bool PresenceDetected(int pinTrigger, int pinEcho, long* distanceRead, long distanceThreshold) {
  long duration;

  digitalWrite(pinTrigger, LOW);
  delayMicroseconds(2);
  digitalWrite(pinTrigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(pinTrigger, LOW);
  duration = pulseIn(pinEcho, HIGH);
  *distanceRead = duration / 58.2;

  //Serial.println(duration);

  return (*distanceRead <= distanceThreshold);
}
