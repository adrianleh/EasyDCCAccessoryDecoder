#ifndef MULTIPLEX
#define MULTIPLEX

#include <assert.h>
#include <stdint.h>
#include <Arduino.h>

class MultiplexOutput {
    uint8_t ctlPins[4];
    uint8_t dataPin;
  public:
    MultiplexOutput(uint8_t ctlPin1, uint8_t ctlPin2, uint8_t ctlPin3, uint8_t ctlPin4, uint8_t dataPin) {
      this->ctlPins[0] = ctlPin1;
      this->ctlPins[1] = ctlPin2;
      this->ctlPins[2] = ctlPin3;
      this->ctlPins[4] = ctlPin4;
      this->dataPin = dataPin;
      pinMode(ctlPin1, OUTPUT);
      pinMode(ctlPin2, OUTPUT);
      pinMode(ctlPin3, OUTPUT);
      pinMode(ctlPin4, OUTPUT);
      pinMode(dataPin, OUTPUT);
    }
    void pulseOnAddr(uint8_t addr, uint8_t duration) {
      assert(addr < 16);
      for (uint8_t i = 0; i < 4; i++) {
        if (addr & (1 << i)) {
          digitalWrite(ctlPins[i], HIGH);
        } else {
          digitalWrite(ctlPins[i], LOW);
        }
      }
      digitalWrite(dataPin, HIGH);
      delay(duration);
      digitalWrite(dataPin, LOW);
    }
};

#endif // MULTIPLEX
