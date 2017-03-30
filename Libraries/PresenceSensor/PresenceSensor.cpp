/*
 * PresenceSensor.cpp - Library for use sonar sensor HC-SR04
 * Created by Jordi Dalmau, 15/03/2017
 * MIT License
 */

 
#include "Arduino.h"
#include "PresenceSensor.h"

PresenceSensor::PresenceSensor(String userId, String homeId, String deviceId, String id, String name, String desc, int pinTrigger, int pinEcho, long presenceThreshold) {
  _userId = userId;
  _homeId = homeId;
  _deviceId = deviceId;

  _id = id;
  _name = name;
  _description = desc;
  _pinTrigger = pinTrigger;
  _pinEcho = pinEcho;
  _presenceThreshold = presenceThreshold;
  _lastPresenceValue = false;
  _lastDistanceRead = 0;
  
  pinMode(pinTrigger, OUTPUT);
  pinMode(pinEcho, INPUT);
}

/* Method that detects if threre is an object near the sensor
 * Parameters:
 *   sensor: pointer to the sensor variable
 *   distance: returns the distance in cm
 * 
 * (1) Using IO trigger for at least 10us high level signal,
 * (2) The Module automatically sends eight 40 kHz and detect whether there is a pulse signal back.
 * (3) IF the signal back, through high level , time of high output IO duration is the time from sending ultrasonic to returning.
 * Test distance = (high level time×velocity of sound (340M/S) / 2,
 */
bool PresenceSensor::presenceDetected(long* distance, bool* presenceValueHasChanged) {
  long duration;
  bool presenceValue = false;

  digitalWrite(_pinTrigger, LOW);
  delayMicroseconds(2);
  digitalWrite(_pinTrigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(_pinTrigger, LOW);
  duration = pulseIn(_pinEcho, HIGH);
  *distance = duration / 58.2;
  _lastDistanceRead = *distance;

  presenceValue = (*distance <= _presenceThreshold);

  if (_lastPresenceValue != presenceValue) {
    _lastPresenceValue = presenceValue;
    *presenceValueHasChanged = true;
  } else {
    *presenceValueHasChanged = false;
  }

  return presenceValue;
}

String PresenceSensor::toJSON() {
  long duration;

  digitalWrite(_pinTrigger, LOW);
  delayMicroseconds(2);
  digitalWrite(_pinTrigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(_pinTrigger, LOW);
  duration = pulseIn(_pinEcho, HIGH);
  _lastDistanceRead = duration / 58.2;

  if (_lastDistanceRead <= _presenceThreshold) {
    return "{ \"sensor\": \"" + _name + "\", \"value\": \"DETECTED\", \"distance\": \"" + _lastDistanceRead + "\" }";
  }
  else {
    return "{ \"sensor\": \"" + _name + "\", \"value\": \"NOT DETECTED\", \"distance\": \"" + _lastDistanceRead + "\" }";
  }
}

String PresenceSensor::toJSON(long lastDistance) {
  if (lastDistance <= _presenceThreshold) {
    return "{ \"sensor\": \"" + _name + "\", \"value\": \"DETECTED\", \"distance\": \"" + lastDistance + "\" }";
  }
  else {
    return "{ \"sensor\": \"" + _name + "\", \"value\": \"NOT DETECTED\", \"distance\": \"" + lastDistance + "\" }";
  }
}

String PresenceSensor::toFirebaseDB() {
  return "secured-homes/" + _userId + "/" + _homeId + "/devices/" + _deviceId + "/sensors/" + _id;
}

long PresenceSensor::lastDistanceRead() {
  return _lastDistanceRead;
}

String PresenceSensor::lastValueReadToString() {
  if (_lastDistanceRead  <= _presenceThreshold) {
    return "DETECTED";
  } else {
    return "NOT_DETECTED";
  }
}

String PresenceSensor::getId() {
  return _id;
}

String PresenceSensor::getName() {
  return _name;
}

String PresenceSensor::getDescription() {
  return _description;
}

String PresenceSensor::getState() {
  if (_lastDistanceRead  <= _presenceThreshold) {
    return "DETECTED";
  } else {
    return "NOT_DETECTED";
  }
}
