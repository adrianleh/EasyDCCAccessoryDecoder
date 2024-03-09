#ifndef TRACK
#define TRACK

#include <Arduino.h>

class Track {
  
  byte relayPin;
  int goSignalCount;
  Track* childTrack;

protected:

  void setRelay() {
    if(this->goSignalCount > 0) {
      digitalWrite(relayPin, HIGH);
    } else {
      digitalWrite(relayPin, LOW);
    }
  }

public:
  Track(byte relayPin, Track* childTrack) {
    this->relayPin = relayPin;
    pinMode(relayPin, OUTPUT);
    this->goSignalCount = 0;
    this->setRelay();
  }
  
  Track(byte relayPin) : Track(relayPin, NULL) {}

  void addGoSignal() {
    this->goSignalCount++;
    this->setRelay();
    if(this->childTrack) {
      this->childTrack->addGoSignal();
    }
  }
  
  void removeGoSignal() {
    if(goSignalCount > 0) {
      this->goSignalCount--;
    }
    this->setRelay();
    if(this->childTrack) {
      this->childTrack->removeGoSignal();
    }
  }
};

#endif
