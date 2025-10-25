#pragma once
#include <cstdint>

class Bus;


class CPU {

  public:
      CPU();
      ~CPU();

      void connectBus(Bus *ptr) { bus = ptr; }

      // enum to store flags to set the status reg later
      enum FLAGS {
        C = (1 << 0),    // Carry
        Z = (1 << 1),    // Zero
        I = (1 << 2),    // Disable interrupts
        D = (1 << 3),    // Decimal mode
        B = (1 << 4),    // Break
        U = (1 << 5),    // Unused
        V = (1 << 6),    // Overflow
        N = (1 << 7),    // Negative
      };


  private:
      Bus *bus = nullptr;

      uint8_t read(uint16_t addr);
      void write(uint16_t addr, uint8_t data);
};
