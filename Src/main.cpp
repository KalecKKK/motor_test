#include "definitions.hpp"
#include <chrono>
#include <iostream>

using namespace EcanVCI;

int main() {

  if (!OpenDevice(16, 0, 0)) {
    std::cerr << "OpenDevice failed\n";
    return 0;
  }

  return 0;
}