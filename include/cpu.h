#pragma once

#include <cwctype>
#include <stdint.h>

class CPU {
  public:
      CPU();
      ~CPU() = default;
  private:
      Bus *bus;
};
