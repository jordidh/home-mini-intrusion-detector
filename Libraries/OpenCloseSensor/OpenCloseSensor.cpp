/*
 * OpenCloseSensor.cpp - Library for use open close sensor
 * Created by Jordi Dalmau, 15/03/2017
 * MIT License
 */

 
#include "Arduino.h"
#include "OpenCloseSensor.h"

OpenCloseSensor::OpenCloseSensor(String name, String desc, int pin, int mode, int openValue) {
  _name = name;
  _description = desc;
  _pin = pin;
  _mode = mode;
  _openValue = openValue;
  _lastOpenValue = 9;
  _lastValueRead = 0;
  
  pinMode(pin, mode);
}

bool OpenCloseSensor::isOpen(bool* openValueHasChanged) {
  _lastValueRead = digitalRead(_pin);

  *openValueHasChanged = (_lastOpenValue != _lastValueRead);
  _lastOpenValue = _lastValueRead;

  return (_lastValueRead == _openValue);
}

String OpenCloseSensor::toJSON() {
  _lastValueRead = digitalRead(_pin);

  if (_lastValueRead == _openValue) {
    return "{ \"sensor\": \"" + _name + "\", \"value\": \"OPEN\" }";
  } else {
    return "{ \"sensor\": \"" + _name + "\", \"value\": \"CLOSE\" }";
  }
}

String OpenCloseSensor::toJSON(int lastValue) {
  if (lastValue == _openValue) {
    return "{ \"sensor\": \"" + _name + "\", \"value\": \"OPEN\" }";
  } else {
    return "{ \"sensor\": \"" + _name + "\", \"value\": \"CLOSE\" }";
  }
}

String OpenCloseSensor::toFirebaseDB(int lastValue) {
  //if (lastValue == _openValue) {
  //  return "sensor/sensor:" + _name + "/value:OPEN";
  //} else {
  //  return "sensor/sensor:" + _name + "/value:CLOSE";
  //}
  return "sensor/sensor:" + _name + "/value";
}

int OpenCloseSensor::lastValueRead() {
  return _lastValueRead;
}

String OpenCloseSensor::lastValueReadToString() {
  if (_lastValueRead == _openValue) {
    return "OPEN";
  } else {
    return "CLOSE";
  }
}
