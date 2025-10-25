#pragma once
#include "../include/Bus.h"
#include <cstdint>

Bus::Bus() {
    // Connect the bus
    cpu.connectBus(this);
}

uint8_t Bus::read(uint16_t addr, bool flag) {
   return ram[addr]; 
}

void Bus::write(uint16_t addr, uint8_t data) {
    // We mirror the address by ANDing it with 0x07FF since the cpu has 64kb addressable range while the ram is
    // only 2kb in size so the address should wrap around once it goes out of bounds
    ram[addr & 0x07FF] = data; 
}


