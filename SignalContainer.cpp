#ifndef SIGNAL_CONTAINER
#define SIGNAL_CONTAINER

#include "Signals.cpp"
#define SIGNAL_SIZE 2
#define SIGNALS_PER_MEM (8 / 2)
#define BITMASK (SIGNALS_PER_MEM - 1)

#ifndef CONTAINER_DEBUG
#define MS_UNTIL_WRITE (10 * 1000) // 10 seconds
#else
#define MS_UNTIL_WRITE 1000
#endif 

#define MAGIC_VALUE 17

class SignalContainer {
    int baseAddr;
    Signal** signals;
    bool* writeFlags;
    uint8_t length;
    bool needsAnyWrite;
    uint8_t nChunks;
    long int lastWriteReq;
#ifdef CONTAINER_DEBUG 
    byte *fakeEEPROM;
#endif
  public:
    SignalContainer(uint8_t noSignals) {
      this->signals = static_cast<Signal**>(malloc(sizeof(Signal*) * noSignals));
      this->writeFlags = static_cast<bool*>(malloc(sizeof(bool) * noSignals));
      this->lastWriteReq = millis();
      this->length = noSignals;
      this->needsAnyWrite = false;
      this->nChunks = (this->length + (SIGNALS_PER_MEM - 1)) / SIGNALS_PER_MEM;
#ifdef CONTAINER_DEBUG
      this->fakeEEPROM = static_cast<byte*>(malloc(sizeof(byte) * this->nChunks));
      this->baseAddr = 512;
#endif
    }
    void addSignal(uint8_t pos, Signal* signal) {
      assert(pos < this->length);
      signals[pos] = signal;
      signal->transition(signal->getState()); // Ensure signal is in correct state upon loading.
    }

    Signal* getForRead(uint8_t pos) {
      return signals[pos];
    }

    Signal* getForWrite(uint8_t pos) {
      writeFlags[pos] = true;
      needsAnyWrite = true;
      lastWriteReq = millis();
      return getForRead(pos);
    }

    void transitionWithAddress(addr_t address, SignalState state) {
      for (uint8_t i = 0; i < this->length; i++) {
        if(this->getForRead(i)->getAddress() != address) {
          continue;
        }
        this->getForWrite(i)->transition(state);
      }
    }

    void transitionWithAddressDoubleAddress(addr_t address, uint8_t value) {
      value &= 1; // only allow one bit to be there
      for (uint8_t i = 0; i < this->length; i++) {
        auto curr_signal = this->getForRead(i);
        auto curr_addr = curr_signal->getAddress();
        uint8_t raw_0_idx_state = static_cast<uint8_t>(curr_signal->getState()) - 1;
        if(curr_addr == address) {
          raw_0_idx_state &= 0x2; // 0 1nd bit
          raw_0_idx_state |= value;
        } else if(curr_addr + 1 == address) {
          raw_0_idx_state &= 0x1; // 0 2nd bit
          raw_0_idx_state |= (value << 1);
        } else {
          continue; // Not the signal in question
        }
        auto state = static_cast<SignalState>(raw_0_idx_state + 1);
        this->getForWrite(i)->transition(state);
      }
    }

    inline boolean everWritten() {
      return EEPROM.read(baseAddr) != MAGIC_VALUE;
    }

    inline void markWritten() {
      EEPROM.write(baseAddr, MAGIC_VALUE);
    }

    void saveStates() { // Can be called repeatedly from loop without damaging EEPROM
      if (!needsAnyWrite) {
        return;
      }
      long int lastWriteTimeDelta = millis() - lastWriteReq;
      if (lastWriteTimeDelta <= MS_UNTIL_WRITE) {
        return;
      }
      if(!everWritten()) {
        markWritten();
      }
      auto addr = baseAddr + 1;
      byte curr = 0;
      bool chunkNeedsWrite = false;
      for (uint8_t i = 0; i < this->length; i++) {
        chunkNeedsWrite |= writeFlags[i]; // if anything in chunk needs write, then write
        auto currState = getForRead(i)->getState() - 1;
        byte index = (2 * (i % SIGNALS_PER_MEM)); // bit index in current byte
        curr |= (currState << index);
        bool endOfChunk = (i % SIGNALS_PER_MEM == (SIGNALS_PER_MEM - 1)) ;
        bool endOfArray = i == this->length - 1;
        if ((endOfChunk || endOfArray) && chunkNeedsWrite) {
          Serial.print("Write ");
          Serial.print(curr, BIN);
          Serial.print(" to addr #");
          Serial.println(addr);
#ifdef CONTAINER_DEBUG
          byte oldVal = fakeEEPROM[addr];
#else
          byte oldVal = EEPROM.read(addr);
#endif
          if (oldVal == curr) {
            Serial.println("Skip writing same value");
            continue; 
          }
      
#ifdef CONTAINER_DEBUG
          this->fakeEEPROM[addr] = curr;
#else
          EEPROM.write(addr, curr);
#endif
          addr += 1; // Write to higher address next time
          curr = 0; // Reset current value
          chunkNeedsWrite = false;
        }
      }
      needsAnyWrite = false;
    }

    void loadStates() {
      if(!everWritten()) { return; } // Only read if written
      auto addr = baseAddr + 1;
      for (uint8_t memIdx = 0; memIdx < this->nChunks; memIdx++) {
#ifdef CONTAINER_DEBUG
        byte memVal = this->fakeEEPROM[addr];
#else
        byte memVal = EEPROM.read(addr);
#endif
        for (uint8_t i = 0; i < SIGNALS_PER_MEM ; i++) {
          uint8_t signalIdx = memIdx * SIGNALS_PER_MEM + i;
          if (signalIdx >= this->length) { // We might not always have n % 4 = 0 signals
            return;
          }
          byte rawState = (memVal >> (2 * i)) & BITMASK;
          SignalState newState = static_cast<SignalState>(rawState + 1);
          getForRead(signalIdx)->transition(newState); // We get for read (even though we "change" the state), since this doesn't change the state, it merely restores it
        }
        addr++;
      }
    }
};

#endif // SIGNAL_CONTAINER
