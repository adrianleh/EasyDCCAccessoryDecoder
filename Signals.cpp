#ifndef SIGNALS
#define SIGNALS

#include <EEPROM.h>
#include <assert.h>
#include <Arduino.h>
#include "Accessory.cpp"
#include "Track.cpp"

enum SignalState {
  HP0 = 1,
  HP1 = 2,
  HP2 = 3,
  SH1 = 4
};

static inline boolean stateIsGo(SignalState state) {
  return state != HP0;
} 

#define CHECK_VIRTUAL if(Signal::isVirtual()) { return; }

class Signal : public Accessory {
    SignalState state;
    Signal* attachedDistantSignal;
    Track* track;
  protected:
    boolean virt;
    virtual boolean localTransition(SignalState state) {};
    void notifyTrack(SignalState prevState, SignalState newState) {
      if(track == NULL) { return; }
      if(!stateIsGo(prevState) && stateIsGo(newState)) {
        track->addGoSignal();
      } else if (stateIsGo(prevState) && !stateIsGo(newState)) {
        track->removeGoSignal();
      }
    }
  public:
    Signal(addr_t addr, boolean isVirtual, Track* track) : Accessory (addr) {
      state = HP0;
      this->virt = isVirtual;
      this->attachedDistantSignal = NULL;
      this->track = track;
    }

    SignalState getState() {
      return this->state;
    }

    boolean isVirtual() {
      this->virt;
    }

    void setAttached(Signal* attachedDistantSignal) {
      this->attachedDistantSignal = attachedDistantSignal;
    }
    
    boolean transition(SignalState state) {
      if(!this->localTransition(state)) {
        return false;
      }
      this->notifyTrack(this->state, state);
      this->state = state;
      if (this->attachedDistantSignal) {
        auto attachedState = this->attachedDistantSignal->getState();
        this->attachedDistantSignal->transition(attachedState);
      }
    }
};

class TrackBlockSignal : public Signal {
    uint16_t pinRed;
    uint16_t pinWhite;
  protected:
    void write(uint8_t red, uint8_t white) {
      CHECK_VIRTUAL;
      digitalWrite(pinRed, red);
      digitalWrite(pinWhite, white);
    }
  public:
    TrackBlockSignal(addr_t addr, boolean isVirtual, Track* track, uint16_t pinRed, uint16_t pinWhite) : Signal(addr, isVirtual, track) {
      this->pinRed = pinRed;
      this->pinWhite = pinWhite;
      if(!isVirtual) {
        pinMode(pinRed, OUTPUT);
        pinMode(pinWhite, OUTPUT);
      }
      this->transition(this->getState());
    }
    
    boolean localTransition(SignalState state) {
      switch (state) {
        case SH1:
          this->write(LOW, HIGH);
          return true;
        case HP0:
          this->write(HIGH, LOW);
          return true;
      }
      return false;
    }
};


class BlockSignal : public Signal {
    uint16_t pinRed;
    uint16_t pinGreen;
  protected:
    void write(uint8_t red, uint8_t green) {
      CHECK_VIRTUAL;
      digitalWrite(pinRed, red);
      digitalWrite(pinGreen, green);
    }
  public:
    BlockSignal(addr_t addr, boolean isVirtual, Track* track, uint16_t pinRed, uint16_t pinGreen) : Signal(addr, isVirtual, track) {
      this->pinRed = pinRed;
      this->pinGreen = pinGreen;
      if(!isVirtual) {
        pinMode(pinRed, OUTPUT);
        pinMode(pinGreen, OUTPUT);
      }
      this->transition(this->getState());
    }
    
    boolean localTransition(SignalState state) {
      switch (state) {
        case HP1:
          this->write(LOW, HIGH);
          return true;
        case HP0:
          this->write(HIGH, LOW);
          return true;
      }
      return false;
    }
};


class EntrySignal : public BlockSignal {
    uint16_t pinYellow;
  protected:
    void write(uint8_t red, uint8_t green, uint8_t yellow) {
      CHECK_VIRTUAL;
      BlockSignal::write(red, green);
      digitalWrite(pinYellow, yellow);
    }
  public:
    EntrySignal(addr_t addr, boolean isVirtual, Track* track, uint16_t pinRed, uint16_t pinGreen, uint16_t pinYellow) : BlockSignal(addr, isVirtual, track, pinRed, pinGreen) {
      this->pinYellow = pinYellow;
      if(!isVirtual) {
        pinMode(pinYellow, OUTPUT);
      }
      this->transition(this->getState());
    }
    
    boolean localTransition(SignalState state) {
      switch (state) {
        case HP1:
          this->write(LOW, HIGH, LOW);
          return true;
        case HP2:
          this->write(LOW, HIGH, HIGH);
          return true;
        case HP0:
          this->write(HIGH, LOW, LOW);
          return true;
      }
      return false;
    }
};


class ExitSignal : public EntrySignal {
    uint16_t pinWhite;
    uint16_t pinRed2;
  protected:
    void write(uint8_t red, uint8_t red2, uint8_t green, uint8_t yellow, uint8_t white) {
      CHECK_VIRTUAL;
      EntrySignal::write(red, green, yellow);
      digitalWrite(pinWhite, white);
      digitalWrite(pinRed2, red2);
    }
  public:
    ExitSignal(addr_t addr, boolean isVirtual, Track* track, uint16_t pinRed, uint16_t pinRed2, uint16_t pinGreen, uint16_t pinYellow, uint16_t pinWhite) : EntrySignal(addr, isVirtual, track, pinRed, pinGreen, pinYellow) {
      this->pinWhite = pinWhite;
      if(!isVirtual) {
        pinMode(pinWhite, OUTPUT);
      }
      this->transition(this->getState());
    }
    
    boolean localTransition(SignalState state) {
      switch (state) {
        case HP1:
          this->write(LOW, LOW, HIGH, LOW, LOW);
          return true;
        case HP2:
          this->write(LOW, LOW, HIGH, HIGH, LOW);
          return true;
        case HP0:
          this->write(HIGH, HIGH, LOW, LOW, LOW);
          return true;
        case SH1:
          this->write(HIGH, LOW, LOW, LOW, HIGH);
          return true;
      }
      return false;
    }
};


class DistantSignal : public Signal {
    uint16_t pinYellowT;
    uint16_t pinYellowB;
    uint16_t pinGreenT;
    uint16_t pinGreenB;
    Signal *attachedMainSignal;

    void write(uint8_t yT, uint8_t yB, uint8_t gT, uint8_t gB) {
      CHECK_VIRTUAL;
      digitalWrite(pinYellowT, yT);
      digitalWrite(pinYellowB, yB);
      digitalWrite(pinGreenT, gT);
      digitalWrite(pinGreenB, gB);
    }
  public:
    DistantSignal(addr_t addr, boolean isVirtual, Track* track, uint16_t pinYellowT, uint16_t pinYellowB, uint16_t pinGreenT, uint16_t pinGreenB, Signal *attachedMainSignal) : Signal(addr, isVirtual, track) {
      this->pinYellowT = pinYellowT;
      this->pinYellowB = pinYellowB;
      this->pinGreenT = pinGreenT;
      this->pinGreenB = pinGreenB;
      this->attachedMainSignal = attachedMainSignal;
      if(!isVirtual) {
        pinMode(pinYellowT, OUTPUT);
        pinMode(pinYellowB, OUTPUT);
        pinMode(pinGreenT, OUTPUT);
        pinMode(pinGreenB, OUTPUT);
      }
      this->transition(this->getState());
    }

    DistantSignal(addr_t addr, boolean isVirtual, Track* track, uint16_t pinYellowT, uint16_t pinYellowB, uint16_t pinGreenT, uint16_t pinGreenB) : DistantSignal(addr, isVirtual, track, pinYellowT, pinYellowB, pinGreenT, pinGreenB, NULL) {
    }

    boolean localTransition(SignalState state) {
      if (attachedMainSignal &&
          (attachedMainSignal->getState() == HP0 || attachedMainSignal->getState() == SH1)) { // If distant signal is mounted on a main signal that is red (HP0/SH1), we turn of all LEDs
        this->write(LOW, LOW, LOW, LOW);
        return true;
      }
      switch (state) {
        case HP1:
          this->write(LOW, LOW, HIGH, HIGH);
          return true;
        case HP2:
          this->write(LOW, HIGH, HIGH, LOW);
          return true;
        case SH1:
        case HP0:
          this->write(HIGH, HIGH, LOW, LOW);
          return true;
      }
      return false;
    };


};

#endif // SIGNALS
