#pragma once

#include "Bus.h"
#include "CPU.h"
#include <cstdint>

class NES {

  private:
      Bus bus;
      CPU cpu;

  public:
      NES();
      ~NES() = default;

      void reset();
      void clock();
};



