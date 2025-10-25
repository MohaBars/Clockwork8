#pragma once
#include <cstdint>

class Bus;


class CPU {

  public:
      CPU();
      ~CPU();

      void connectBus(Bus *ptr) { bus = ptr; }
  private:
      Bus *bus = nullptr;
};
