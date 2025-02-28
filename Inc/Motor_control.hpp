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

#include <stdint.h>
#include "ECanVci_well.hpp"

namespace Motor_info
{
    enum Communication_mode {
        UNKOWN   = 0x00,
        AUTO_MSG = 0x01,
        QA_MODE  = 0x02,
    };

    enum ErrorCode {
        NO_ERROR            = 0x00,
        OVERHEAT            = 0x01,
        OVERCURRENT         = 0x02,
        LOW_VOLTAGE         = 0x03,
        ENCODER_ERROR       = 0x04,
        INDICATES_ERROR     = 0x05,
        BREKE_HIGH_VOLTAGE  = 0x06,
        DRV_DRIVER_ERROR    = 0x07,
    };
} // namespace Motor_info


class Motor_control
{
protected:
    typedef uint8_t status_type;

    uint8_t id_high, id_low;

    uint16_t max_retry_times;

    status_type SendCmd();

    Motor_control(Motor_control&) = delete;
    Motor_control& operator= (Motor_control&) = delete;
public:
    Motor_control(uint8_t id_high, uint8_t id_low);
    ~Motor_control();

    void SetMaxRetryTimes(uint16_t max_retry_times);

    status_type SetZero();

    status_type ResetID();
    status_type ResetID(uint16_t new_id);
    status_type ResetID(uint8_t new_id_high, uint8_t new_id_low);

    Motor_info::Communication_mode QueryCommunicationMode();

    static uint16_t QueryID();

    void HybridControl();
    void SetPosition();
    void SetSpeed();
    void SetAcceleration();
    void SetCurrent();
};


// void send_motor_ctrl_cmd(uint16_t motor_id,float kp,float kd,float pos,float spd,float cur);
// void set_motor_position(uint16_t motor_id,float pos,uint16_t spd,uint16_t cur,uint8_t ack_status);
// void set_motor_speed(uint16_t motor_id,float spd,uint16_t cur,uint8_t ack_status);
// void set_motor_acceleration(uint16_t motor_id,uint16_t acc,uint8_t ack_status);	
// void set_motors_current(OD_Motor_Msg rv_MotorMsg[8],uint8_t motor_quantity);