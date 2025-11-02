#pragma once
#include "../include/CPU.h"
#include <cstdint>
#include "../include/Bus.h"


uint8_t CPU::read(uint16_t addr) {
    return bus->read(addr);
}

void CPU::write(uint16_t addr, uint8_t data) {
    bus->write(addr, data);
}

void CPU::clock() {
    if(cycles == 0) {
        // Read next instruction byte
        opcode = read(PC);
        
        // Set the unused flag always to true
        setFlag(U, true);

        PC++;
        
        // Get the number of cycles for this instruction
        cycles = lookup[opcode].cycles;
        
        uint8_t addCycle1 = (this->*lookup[opcode].addressingMode)();
        uint8_t addCycle2 = (this->*lookup[opcode].operation)();
        
        // If both the addressing mode and the operation require additional cycle, add it to the cycles
        cycles += (addCycle1 & addCycle2);

        // Set the unused flag back to true in case instruction has altered it 
        setFlag(U, true);

        // Increment clock count
        clock_accum++;

        // Decrement num cycles remaining for this instruction
        cycles--;
    } 
}

// Reset function to reset the system to a known state
void CPU::reset()
{
	  // Get address to set program counter to
	  addr_abs = 0xFFFC;
	  uint16_t lo = read(addr_abs + 0);
	  uint16_t hi = read(addr_abs + 1);

	  // Set it
	  PC = (hi << 8) | lo;

	  // Reset internal registers
	  a = 0;
	  x = 0;
	  y = 0;
	  stkptr = 0xFD;
	  status = 0x00 | U;

	  // Clear internal helper variables
	  addr_rel = 0x0000;
	  addr_abs = 0x0000;
	  fetched = 0x00;

	  // Reset takes time
  	cycles = 8;
}

uint8_t CPU::getFlag(FLAGS flag){
    uint8_t result = 0;
    if((status & flag) > 0){
      result = 1;
    } 
    return result;
}

void CPU::setFlag(FLAGS flag, bool value){
    if(value){
      status |= flag;
    } else {
      status &= ~flag;
    }
}

void CPU::IRQ()
{
	  // If interrupts are allowed
	  if (getFlag(I) == 0)
	  {
		  // Push the program counter to the stack. It's 16-bits dont
		  // forget so that takes two pushes
		  write(0x0100 + stkptr, (PC >> 8) & 0x00FF);
		  stkptr--;
		  write(0x0100 + stkptr, PC & 0x00FF);
		  stkptr--;

		  // Then Push the status register to the stack
		  setFlag(B, 0);
		  setFlag(U, 1);
		  setFlag(I, 1);
		  write(0x0100 + stkptr, status);
		  stkptr--;

		  // Read new program counter location from fixed address
		  addr_abs = 0xFFFE;
		  uint16_t lo = read(addr_abs + 0);
		  uint16_t hi = read(addr_abs + 1);
		  PC = (hi << 8) | lo;

		  // IRQs take time
		  cycles = 7;
	  }
}


// A Non-Maskable Interrupt cannot be ignored. 
void CPU::NMI()
{
	  write(0x0100 + stkptr, (PC >> 8) & 0x00FF);
	  stkptr--;
	  write(0x0100 + stkptr, PC & 0x00FF);
	  stkptr--;

	  setFlag(B, 0);
	  setFlag(U, 1);
	  setFlag(I, 1);
	  write(0x0100 + stkptr, status);
	  stkptr--;

	  addr_abs = 0xFFFA;
	  uint16_t lo = read(addr_abs + 0);
	  uint16_t hi = read(addr_abs + 1);
  	PC = (hi << 8) | lo;

  	cycles = 8;
}

// Implied addressing mode: No additional data required for this instruction
uint8_t CPU::IMP(){
    fetched = accumulator;
    return 0;
}

// Immediated addressing mode: We expect the next byte to be used as a value
uint8_t CPU::IMM(){
    addr_abs = PC++;
    return 0;
}

// Indirect addressing mode:
// The supplied 16-bit address is read to get the actual 16-bit address. Apparently this
// instruction is unusual in that it has a bug in the hardware! To emulate its
// function accurately, we also need to emulate this bug. If the low byte of the
// supplied address is 0xFF, then to read the high byte of the actual address
// we need to cross a page boundary. This doesnt actually work on the chip as 
// designed, instead it wraps back around in the same page, yielding an 
// invalid actual address
uint8_t CPU::IND()
{
	  uint16_t ptr_lo = read(PC);
	  PC++;
	  uint16_t ptr_hi = read(PC);
	  PC++;

	  uint16_t ptr = (ptr_hi << 8) | ptr_lo;

	  if (ptr_lo == 0x00FF) // Simulate page boundary hardware bug
	  {
		  addr_abs = (read(ptr & 0xFF00) << 8) | read(ptr + 0);
	  }
	  else // Behave normally
	  {
		  addr_abs = (read(ptr + 1) << 8) | read(ptr + 0);
	  }
	
	  return 0;
}

// Indirect X addressing mode:
// The supplied 8-bit address is offset by X Register to index
// a location in page 0x00. The actual 16-bit address is read 
// from this location
uint8_t CPU::IZX()
{
	  uint16_t addr = read(PC);
	  PC++;

	  uint16_t lo = read((uint16_t)(addr + (uint16_t)x) & 0x00FF);
	  uint16_t hi = read((uint16_t)(addr + (uint16_t)x + 1) & 0x00FF);

	  addr_abs = (hi << 8) | lo;
	
	  return 0;
}

// Indirect Y addressing mode:
// The supplied 8-bit address indexes a location in page 0x00. From 
// here the actual 16-bit address is read, and the contents of
// Y Register is added to it to offset it. If the offset causes a
// change in page then an additional clock cycle is required.
uint8_t CPU::IZY()
{
  	uint16_t addr = read(PC);
	  PC++;

	  uint16_t lo = read(addr & 0x00FF);
	  uint16_t hi = read((addr + 1) & 0x00FF);

	  addr_abs = (hi << 8) | lo;
	  addr_abs += y;
	
	  if ((addr_abs & 0xFF00) != (hi << 8))
		  return 1;
	  else
		  return 0;
}

// Absolute addressing mode: 
// A full 16-bit address is loaded and used
uint8_t CPU::ABS()
{
	  uint16_t lo = read(PC);
	  PC++;
	  uint16_t hi = read(PC);
	  PC++;

	  addr_abs = (hi << 8) | lo;

	  return 0;
}


// Absolute with X Offset adressing mode:
// Fundamentally the same as absolute addressing, but the contents of the X Register
// is added to the supplied two byte address. If the resulting address changes
// the page, an additional clock cycle is required
uint8_t CPU::ABX()
{
	  uint16_t lo = read(PC);
	  PC++;
	  uint16_t hi = read(PC);
	  PC++;

	  addr_abs = (hi << 8) | lo;
	  addr_abs += x;

	  if ((addr_abs & 0xFF00) != (hi << 8))
		  return 1;
	  else
		  return 0;	
}


// Absolute with Y Offset addressing mode:
// Fundamentally the same as absolute addressing, but the contents of the Y Register
// is added to the supplied two byte address. If the resulting address changes
// the page, an additional clock cycle is required
uint8_t CPU::ABY()
{
	  uint16_t lo = read(PC);
	  PC++;
	  uint16_t hi = read(PC);
	  PC++;

	  addr_abs = (hi << 8) | lo;
	  addr_abs += y;

	  if ((addr_abs & 0xFF00) != (hi << 8))
		  return 1;
	  else
		  return 0;
}

// Zero Page addressing mode:
// To save program bytes, zero page addressing allows you to absolutely address
// a location in first 0xFF bytes of address range. Clearly this only requires
// one byte instead of the usual two.
uint8_t CPU::ZP0()
{
	  addr_abs = read(PC);	
	  PC++;
	  addr_abs &= 0x00FF;
	  return 0;
}



// Zero Page with X Offset addressing mode:
// Fundamentally the same as Zero Page addressing, but the contents of the X Register
// is added to the supplied single byte address. This is useful for iterating through
// ranges within the first page.
uint8_t CPU::ZPX()
{
	  addr_abs = (read(PC) + x);
	  PC++;
	  addr_abs &= 0x00FF;
	  return 0;
}


// Zero Page with Y Offset addressing mode
// Same as above but uses Y Register for offset
uint8_t CPU::ZPY()
{
	  addr_abs = (read(PC) + y);
	  PC++;
	  addr_abs &= 0x00FF;
	  return 0;
}


// Relative addressing mode:
// This address mode is exclusive to branch instructions. The address
// must reside within -128 to +127 of the branch instruction, i.e.
// you cant directly branch to any address in the addressable range.
uint8_t CPU::REL()
{
	  addr_rel = read(PC);
	  PC++;
	  if (addr_rel & 0x80)
		  addr_rel |= 0xFF00;
	  return 0;
}

// TODO: implement instructions and then move on to PPU
