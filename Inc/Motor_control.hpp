/**
 * @file Motor_control.hpp
 * @author KalecKKK
 * @brief 定义电机类型及其基本行为
 * @version 0.1
 * @date 2025-02-28
 *
 * @copyright Copyright (c) 2025
 *
 */

#pragma once

#include "Can_transport.hpp"
#include <stdint.h>

namespace Motor {
enum Communication_mode {
  UNKOWN = 0x00,
  AUTO_MSG = 0x01,
  QA_MODE = 0x02,
};

enum ErrorCode {
  NO_ERROR = 0x00,
  OVERHEAT = 0x01,
  OVERCURRENT = 0x02,
  LOW_VOLTAGE = 0x03,
  ENCODER_ERROR = 0x04,
  INDICATES_ERROR = 0x05,
  BREKE_HIGH_VOLTAGE = 0x06,
  DRV_DRIVER_ERROR = 0x07,
};

struct PID_parameters {
  float kp, ki, kd;
};

enum Control_mode {
  CURRENT_MODE = 0x00,
  TORQUE_MODE = 0x01,
  VARIABEL_DAMPING_BRAKING = 0x02,
  ENERGY_CONSUMPTION_BRAKING = 0x03,
  REGENERATIVE_BRAKING = 0x04,
};

enum Message_return_status {
  NO_ACK = 0x00,
  ACK_TYPE_1 = 0x01,
  ACK_TYPE_2 = 0x02,
  ACK_TYPE_3 = 0x03,
  ACK_TYPE_4 = 0x04,
  ACK_TYPE_5 = 0x05,
};

class Motor_control {
protected:
  typedef uint8_t status_type;

  const EcanVci::Can_transport &can_transport;

  uint8_t id_high, id_low;

  uint16_t max_retry_times;

  /**
   * @brief 发送命令
   * @return status_type 返回状态类型
   */
  status_type SendCmd();

  Motor_control() = delete;
  Motor_control(Motor_control &) = delete;
  Motor_control &operator=(Motor_control &) = delete;

public:
  struct Motor_info {
    uint16_t motor_id;
    ErrorCode error_code;
    float position;
    float speed;
    float current;
    uint8_t motor_temperature;
    uint8_t MOS_temperature;
  } motor_info;

  DWORD UpdateInfo();

public:
  /**
   * @brief 构造函数
   * @param id_high 高位ID
   * @param id_low 低位ID
   */
  Motor_control(const EcanVci::Can_transport &can_transport, uint8_t id_high,
                uint8_t id_low);

  /**
   * @brief 构造函数
   * @param id 电机ID
   */
  Motor_control(const EcanVci::Can_transport &can_transport, uint16_t id);

  /**
   * @brief 析构函数
   */
  ~Motor_control();

  /**
   * @brief 设置最大重试次数
   * @param max_retry_times 最大重试次数
   */
  void SetMaxRetryTimes(uint16_t max_retry_times);

  /**
   * @brief 设为零位
   * @return status_type 返回状态类型
   */
  status_type SetZero() const;

  /**
   * @brief 重置ID
   * @return status_type 返回状态类型
   */
  status_type ResetID();

  /**
   * @brief 重置ID
   * @param new_id 新ID
   * @return status_type 返回状态类型
   */
  status_type ResetID(uint16_t new_id);

  /**
   * @brief 重置ID
   * @param new_id_high 新高位ID
   * @param new_id_low 新低位ID
   * @return status_type 返回状态类型
   */
  status_type ResetID(uint8_t new_id_high, uint8_t new_id_low);

  /**
   * @brief 查询通信模式
   * @return Motor::Communication_mode 返回通信模式
   */
  Motor::Communication_mode QueryCommunicationMode();

  /**
   * @brief 查询ID
   * @return uint16_t 返回ID
   */
  uint16_t QueryID() const;

  /**
   * @brief 查询ID
   * @return uint16_t 返回ID
   */
  static uint16_t QueryID(const EcanVci::Can_transport &can_transport);

  /**
   * @brief 混合控制
   * @param pid PID参数
   * @param position 期望位置
   * @param speed 期望速度
   * @param current 期望电流
   */
  void HybridControl(Motor::PID_parameters pid, float position, float speed,
                     float current) const;

  /**
   * @brief 设置位置
   * @param position 期望位置
   * @param speed 期望速度
   * @param current 电流阈值
   * @param ack_status 报文返回状态
   */
  void SetPosition(float position, uint16_t speed, uint16_t current,
                   Message_return_status ack_status) const;

  /**
   * @brief 设置速度
   * @param speed 速度
   * @param current 电流
   * @param ack_status 报文返回状态
   */
  void SetSpeed(float speed, uint16_t current,
                Message_return_status ack_status) const;

  /**
   * @brief 设置电流
   * @param current 电流
   * @param ack_status 报文返回状态
   */
  void SetCurrent(uint16_t current, Message_return_status ack_status) const;

  /**
   * @brief 以指定模式控制
   * @param control_mode 控制模式
   * @param current_or_torque 电流或扭矩
   * @param ack_status 报文返回状态
   */
  void ControlWithMode(Control_mode control_mode, float current_or_torque,
                       Message_return_status ack_status) const;
};

} // namespace Motor
