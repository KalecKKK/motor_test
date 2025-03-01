#include "Can_transport.hpp"
#include <chrono>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <thread>

EcanVci::Can_transport::Can_transport() : Can_transport(0x04, 0x00) {}

EcanVci::Can_transport::Can_transport(DWORD device_type, DWORD device_index) {
  this->device_type = device_type;
  this->device_index = device_index;

  int try_times = 0;
  while (!OpenDevice(device_type, device_index, 0) && try_times < 3) {
    std::cerr << "OpenDevice failed, try again\n";
    try_times++;
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
  if (try_times == 3) {
    std::cerr << "OpenDevice failed\n";
    throw std::runtime_error("OpenDevice failed");
  }

  std::cout << "OpenDevice succeeded\n";
}

EcanVci::Can_transport::~Can_transport() {
  CloseDevice(device_type, device_index);
  std::cout << "CloseDevice\n";
}

DWORD EcanVci::Can_transport::Transmit(CAN_ID can_index, BYTE data[],
                                       ULONG len) const {
  if (len > 8) {
    std::cerr << "Data length should be less than or equal to 8\n";
    throw std::runtime_error("Data length should be less than or equal to 8");
  }

  CAN_OBJ msg = {0x7FF, 0, 0, 0, 0, static_cast<BYTE>(len), {0}, {0}};
  memcpy(msg.Data, data, len);

  //*//debug
  std::cout << "device_type: " << device_type
            << ", device_index: " << device_index
            << ", can_index: " << can_index << ", len: " << len
            << ", data: " << std::endl;
  for (int i = 0; i < len; i++) {
    std::cout << std::hex << static_cast<int>(data[i]) << " ";
  }
  std::cout << std::dec << std::endl;
  //*/

  auto result = ::Transmit(device_type, device_index,
                           static_cast<DWORD>(can_index), &msg, 1);
  if (result != 1) {
    std::cerr << "Transmit failed\n";
    throw std ::runtime_error("Transmit failed");
  }
  return result;
}
