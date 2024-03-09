#ifndef TURNOUTCONTAINER
#define TURNOUTCONTAINER

#include <Arduino.h>
#include "Accessory.cpp"
#include "Multiplex.cpp"
#include "Turnout.cpp"

class TurnoutContainer {
  private:
  Turnout** turnouts;
  uint8_t size;
  uint8_t idx;
  public:
  TurnoutContainer(uint8_t noTurnouts) {
    this->size = noTurnouts;
    this->idx = 0;
    turnouts = static_cast<Turnout**>((malloc(sizeof(Turnout*) * noTurnouts)));
  }
  boolean add(Turnout *turnout) {
    if (idx >= size) {
      return false;
    }
    turnouts[idx++] = turnout;
  }
  void transition(addr_t addr, TurnoutState state) {
    for(uint8_t i = 0; i < this->idx; i++) {
      Turnout* curr = turnouts[i];
      if(curr->getAddress() == addr) {
        curr->transition(state);
      }
    }
  }
};

#endif
