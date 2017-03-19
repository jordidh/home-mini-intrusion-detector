/*
 * OpenCloseSensor.h - Library for use open close sensor
 * Created by Jordi Dalmau, 15/03/2017
 * MIT License
 */

 
#ifndef OpenCloseSensor_h
#define OpenCloseSensor_h
#include "Arduino.h"

class OpenCloseSensor {
  public:
    OpenCloseSensor(String name, String desc, int pin, int mode, int openValue);
    bool isOpen(bool* openValueHasChanged);
    String toJSON();
    String toJSON(int lastValue);
    String toFirebaseDB(int lastValue);
    int lastValueRead();
    String lastValueReadToString();
  private:
    String _name;
    String _description;
    int _pin;
    int _mode; //INPUT, INPUT_PULLUP
    int _openValue;  //LOW, HIGH
    int _lastOpenValue; //last open/close value (LOW/HIGH) to check if a change is made
    int _lastValueRead;  //LOW, HIGH
};

#endif