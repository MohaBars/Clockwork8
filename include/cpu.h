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

      // Registers
      uint8_t accumulator = 0x00;
      uint8_t x = 0x00;
      uint8_t y = 0x00;
      uint8_t stkptr = 0x00;     // Stack pointer
      uint8_t status = 0x00;
      uint16_t PC = 0x0000;        // Program counter

  private:
      Bus *bus = nullptr;

      uint8_t read(uint16_t addr);
      void write(uint16_t addr, uint8_t data);

      void setFlag(FLAGS flag, bool value);
      uint8_t getFlag(FLAGS flag);
};
