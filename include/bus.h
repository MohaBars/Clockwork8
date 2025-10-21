#pragma once
#include <cstdint>
#include <stdint.h>

class Bus {

  public:
      Bus();
      ~Bus() = default;

      uint16_t read(uint16_t addr);
      uint8_t write(uint16_t addr, uint16_t data);
  private:
      static constexpr uint16_t RAM_SIZE = 0x0800;  // 2 KB
      uint8_t ram[RAM_SIZE];
};
