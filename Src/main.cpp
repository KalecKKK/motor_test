#include "definitions.hpp"
#include <chrono>
#include <iostream>

using namespace EcanVCI;

int main() {

  if (!OpenDevice(4, 0, 0)) {
    std::cerr << "OpenDevice failed\n";
    return 0;
  }
  std::cout << "OpenDevice success!\n";

  return 0;
}