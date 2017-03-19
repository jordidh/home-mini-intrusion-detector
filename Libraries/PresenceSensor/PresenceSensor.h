/*
 * OpenCloseSensor.h - Library for use sonar sensor HC-SR04
 * Created by Jordi Dalmau, 15/03/2017
 * MIT License
 */

 
#ifndef PresenceSensor_h
#define PresenceSensor_h
#include "Arduino.h"

class PresenceSensor {
  public:
    PresenceSensor(String name, String desc, int pinTrigger, int pinEcho, long presenceThreshold);
    bool presenceDetected(long* distance, bool* presenceValueHasChanged);
    String toJSON();
    String toJSON(long lastDistance);
    String toFirebaseDB(long lastDistance);
    String toFirebaseDBDistance(long lastDistance);
    long lastDistanceRead();
    String lastValueReadToString();
  private:
    String _name;
    String _description;
    int _pinTrigger;
    int _pinEcho;
    long _presenceThreshold; //distance in cm
    bool _lastPresenceValue; //last presence value (DETECTED or MOT_DETECTED) to check if a change is made
    long _lastDistanceRead; //last distance in cm read
};

#endif
