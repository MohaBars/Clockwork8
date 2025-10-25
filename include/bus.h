#pragma once
#include <cstdint>
#include "CPU.h"

class Bus {

  public:
      Bus();
      ~Bus() = default;

      CPU cpu;

      uint8_t read(uint16_t addr, bool flag = false);
      void write(uint16_t addr, uint8_t data);

  private:
      static constexpr uint16_t RAM_SIZE = 0x0800;  // 2 KB
      uint8_t ram[RAM_SIZE];
};
