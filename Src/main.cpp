/**
 * @file main.cpp
 * @author KalecKKK
 * @brief 测试
 * @version 0.1
 * @date 2025-02-28
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "Motor_control.hpp"
#include <chrono>
#include <iostream>
#include <thread>

using namespace Motor;

int main() {
  try {
    EcanVci::Can_transport can_transport;

    Motor_control motor(can_transport, 0x00, 0x01);

    int times = 10;
    while (times--) {
      motor.ResetID();
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    uint32_t frames_count = 0;
    while (true) {
      // motor.SetSpeed(100, 0x0FFF, Message_return_status::ACK_TYPE_1);
      motor.SetCurrent(100, Message_return_status::ACK_TYPE_1);
      // motor.SetSpeed(100, 100, Message_return_status::ACK_TYPE_1);
      // motor.SetPosition(100, 100, 100, Message_return_status::ACK_TYPE_2);
      // motor.ControlWithMode(Control_mode::CURRENT_MODE, 100,
      // Message_return_status::ACK_TYPE_1);

      if (motor.UpdateInfo()) {
        std::cout << "Position: " << motor.motor_info.position
                  << " Speed: " << motor.motor_info.speed
                  << " Current: " << motor.motor_info.current << std::endl;
      }

      if (++frames_count % 100 == 0) {
        std::cout << "Send frames count: " << frames_count << std::endl;
      }

      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
  }
  return 0;
}