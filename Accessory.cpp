#ifndef ACCESSORY
#define ACCESSORY

#include <stdint.h>

typedef uint16_t addr_t;

class Accessory {
  private:
    addr_t addr;
  public:
    Accessory(addr_t addr) {
      this->addr = addr;
    }
    addr_t getAddress() { return this->addr; }
};

#endif // ACCESSORY
