#pragma once
#include "../include/CPU.h"
#include <cstdint>

uint8_t CPU::read(uint16_t addr) {
    return bus->read(addr);
}

void CPU::write(uint16_t addr, uint8_t data) {
    bus->write(addr, data);
}
