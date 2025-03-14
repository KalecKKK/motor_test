/**
 * @file Motor_control.cpp
 * @brief 实现 Motor_control.hpp 中的函数
 * @version 0.1
 * @date 2025-02-28
 *
 */

#include "Motor_control.hpp"
#include <cstring>
#include <iostream>

namespace Motor {

DWORD Motor_control::UpdateInfo() {
  // 更新信息的具体实现
  uint8_t data[8];
  UINT source, len;
  auto result = can_transport.ReceiveLast(source, data, len);

  if (result == 0) {
    std::cerr << "Receive failed\n";
    return STATUS_ERR;
  }

  if (source != ((id_high << 8) | id_low)) {
    std::cerr << "Source ID error\n";
    return STATUS_ERR;
  }

  /*// DEBUG
  std::cout << "Receive data: ";
  for (int i = 0; i < len; i++) {
    std::cout << std::hex << static_cast<int>(data[i]) << " ";
  }
  std::cout << std::dec << std::endl;
  //*/

  switch (static_cast<Message_return_status>((data[0] >> 5) & 0b111)) {
  case Message_return_status::ACK_TYPE_1:
    // 处理ACK_TYPE_1
    motor_info.error_code = static_cast<ErrorCode>(data[0] & 0b11111);
    motor_info.position = static_cast<float>(data[1] << 8 | data[2]);
    motor_info.speed = static_cast<float>(data[3] << 4 | data[4] >> 4) - 2047;
    motor_info.current =
        static_cast<float>((data[4] & 0x0F) << 8 | data[5]) - 2047;
    motor_info.motor_temperature = (data[6] - 50) >> 1;
    motor_info.MOS_temperature = (data[7] - 50) >> 1;
    break;
  case Message_return_status::ACK_TYPE_2:
    // 处理ACK_TYPE_2
    break;
  case Message_return_status::ACK_TYPE_3:
    // 处理ACK_TYPE_3
    break;
  case Message_return_status::ACK_TYPE_4:
    // 处理ACK_TYPE_4
    break;
  case Message_return_status::ACK_TYPE_5:
    // 处理ACK_TYPE_5
    break;
  default:
    // 处理未知状态
    return STATUS_ERR;
  }

  return STATUS_OK;
}

Motor_control::Motor_control(const EcanVci::Can_transport &can_transport,
                             uint8_t id_high, uint8_t id_low)
    : motor_info({0}), can_transport(can_transport), id_high(id_high),
      id_low(id_low), max_retry_times(3) {}

Motor_control::Motor_control(const EcanVci::Can_transport &can_transport,
                             uint16_t id)
    : motor_info({0}), can_transport(can_transport), id_high(id >> 8),
      id_low(id & 0xFF), max_retry_times(3) {}

Motor_control::~Motor_control() {}

void Motor_control::SetMaxRetryTimes(uint16_t max_retry_times) {
  this->max_retry_times = max_retry_times;
  // TODO 下面设置的都要用到这个，都还没写
}

Motor_control::status_type Motor_control::SendCmd() {
  // 发送命令的具体实现
  return 0;
}

Motor_control::status_type Motor_control::SetZero() const {
  // 设为零位的具体实现
  // 示例：发送CAN命令
  uint8_t data[4] = {id_high, id_low, 0x00, 0x03};
  can_transport.Transmit(0x7FF, data, 4);
  return 0;
}

Motor_control::status_type Motor_control::ResetID() {
  // 重置ID的具体实现
  // 示例：发送CAN命令
  uint8_t data[6] = {0x7F, 0x7F, 0x00, 0x05, 0x7F, 0x7F};
  can_transport.Transmit(0x7FF, data, 6);
  return 0;
}

Motor_control::status_type Motor_control::ResetID(uint16_t new_id) {
  // 重置ID的具体实现
  // 示例：发送CAN命令
  uint8_t data[6] = {id_high,
                     id_low,
                     0x00,
                     0x04,
                     static_cast<uint8_t>(new_id >> 8),
                     static_cast<uint8_t>(new_id & 0xFF)};
  can_transport.Transmit(0x7FF, data, 6);
  return 0;
}

Motor_control::status_type Motor_control::ResetID(uint8_t new_id_high,
                                                  uint8_t new_id_low) {
  // 重置ID的具体实现
  // 示例：发送CAN命令
  uint16_t new_id = (new_id_high << 8) | new_id_low;
  return ResetID(new_id);
}

Motor::Communication_mode Motor_control::QueryCommunicationMode() {
  // 查询通信模式的具体实现
  // 示例：发送CAN命令
  uint8_t data[4] = {id_high, id_low, 0x00, 0x81};
  can_transport.Transmit(0x7FF, data, 4);
  return Motor::Communication_mode::UNKOWN;
}

uint16_t Motor_control::QueryID() const {
  // 查询ID的具体实现
  // 示例：发送CAN命令
  uint8_t data[4] = {0xFF, 0xFF, 0x00, 0x82};
  can_transport.Transmit(0x7FF, data, 4);
  // TODO complite receive
  // ...
  return 0;
}

uint16_t Motor_control::QueryID(const EcanVci::Can_transport &can_transport) {
  // 查询ID的具体实现
  // 示例：发送CAN命令
  uint8_t data[4] = {0xFF, 0xFF, 0x00, 0x82};
  can_transport.Transmit(0x7FF, data, 4);
  // TODO complite receive
  // ...
  return 0;
}

void Motor_control::HybridControl(Motor::PID_parameters pid, float position,
                                  float speed, float current) const {
  // 混合控制的具体实现
  // 示例：发送CAN命令
  uint8_t data[8];
  // 将pid, position, speed, current转换为CAN数据
  // ...
  can_transport.Transmit((id_high << 8) | id_low, data, 8);
}

void Motor_control::SetPosition(float position, uint16_t speed,
                                uint16_t current,
                                Message_return_status ack_status) const {
  // 设置位置的具体实现
  // 示例：发送CAN命令
  uint8_t data[8];
  // 将position, speed, current, ack_status转换为CAN数据
  // ...
  can_transport.Transmit((id_high << 8) | id_low, data, 8);
}

void Motor_control::SetSpeed(float speed, uint16_t current,
                             Message_return_status ack_status) const {
  // 设置速度的具体实现
  // 示例：发送CAN命令
  uint8_t data[7];
  // 将speed, current, ack_status转换为CAN数据
  uint32_t speed_bytes = *static_cast<uint32_t *>(static_cast<void *>(&speed));

  data[0] = 0x40 | static_cast<uint8_t>(ack_status);
  data[1] = 0xff & static_cast<uint8_t>(speed_bytes >> 24);
  data[2] = 0xff & static_cast<uint8_t>(speed_bytes >> 16);
  data[3] = 0xff & static_cast<uint8_t>(speed_bytes >> 8);
  data[4] = 0xff & static_cast<uint8_t>(speed_bytes);
  data[5] = 0xff & static_cast<uint8_t>(current >> 8);
  data[6] = 0xff & static_cast<uint8_t>(current);

  can_transport.Transmit((id_high << 8) | id_low, data, 7);
}

void Motor_control::SetCurrent(uint16_t current,
                               Message_return_status ack_status) const {
  // 设置电流的具体实现
  // 示例：发送CAN命令
  uint8_t data[3];
  // 将current, ack_status转换为CAN数据
  data[0] = 0x60 | static_cast<uint8_t>(ack_status);
  data[1] = 0xff & static_cast<uint8_t>(current >> 8);
  data[2] = 0xff & static_cast<uint8_t>(current);

  can_transport.Transmit((id_high << 8) | id_low, data, 3);
}

void Motor_control::ControlWithMode(Control_mode control_mode,
                                    float current_or_torque,
                                    Message_return_status ack_status) const {
  // 以指定模式控制的具体实现
  // 示例：发送CAN命令
  uint8_t data[3];
  // 将control_mode, current_or_torque, ack_status转换为CAN数据
  // ...
  can_transport.Transmit((id_high << 8) | id_low, data, 3);
}

} // namespace Motor
