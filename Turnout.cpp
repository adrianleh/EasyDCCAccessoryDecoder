#ifndef TURNOUT
#define TURNOUT

#include <Arduino.h>
#include "Accessory.cpp"
#include "Multiplex.cpp"

enum TurnoutState {
  CLOSED = 0,
  OPEN = 1
};

#define TURNOUT_PLUSE_MS 200

class Turnout : public Accessory {
    MultiplexOutput *mltplx;
    uint8_t openAddr;
    uint8_t closeAddr;
  public:
    Turnout(addr_t addr, MultiplexOutput *mltplx, uint8_t openAddr, uint8_t closeAddr) : Accessory(addr) {
      this->mltplx = mltplx;
      this->openAddr = openAddr;
      this->closeAddr = closeAddr;
    }

    Turnout(addr_t addr, uint8_t openAddr, uint8_t closeAddr) : Turnout(addr, NULL, openAddr, closeAddr) {}
    
    void transition(TurnoutState state) {
      uint8_t addr = state == OPEN ? openAddr : closeAddr;
      if (mltplx) {
        mltplx->pulseOnAddr(addr, TURNOUT_PLUSE_MS);
      } else {
        this->pulse(addr);
      }
    }
    
    void pulse(uint8_t addr) {
      digitalWrite(addr, HIGH);
      delay(TURNOUT_PLUSE_MS);
      digitalWrite(addr, LOW);
    }
};

#endif // TURNOUT
