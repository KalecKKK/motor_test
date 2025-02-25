/**
 * @file definitions.hpp
 * @author saxiy
 * @brief 补充官方驱动给的宏定义
 * @version 0.1
 * @date 2025-02-25
 *
 * @copyright Copyright (c) 2025
 *
 */

#pragma once

#include "ECanVci.h"

namespace EcanVCI {

enum Tim0Kbps {
  TIM0_KBPS_5 = 0xBF,
  TIM0_KBPS_10 = 0x31,
  TIM0_KBPS_20 = 0x18,
  TIM0_KBPS_40 = 0x87,
  TIM0_KBPS_50 = 0x09,
  TIM0_KBPS_80 = 0x83,
  TIM0_KBPS_100 = 0x04,
  TIM0_KBPS_125 = 0x03,
  TIM0_KBPS_200 = 0x81,
  TIM0_KBPS_250 = 0x01,
  TIM0_KBPS_400 = 0x80,
  TIM0_KBPS_500 = 0x00,
  TIM0_KBPS_666 = 0x80,
  TIM0_KBPS_800 = 0x00,
  TIM0_KBPS_1000 = 0x00
};

enum Tim1Kbps {
  TIM1_KBPS_5 = 0xFF,
  TIM1_KBPS_10 = 0x1C,
  TIM1_KBPS_20 = 0x1C,
  TIM1_KBPS_40 = 0xFF,
  TIM1_KBPS_50 = 0x1C,
  TIM1_KBPS_80 = 0xFF,
  TIM1_KBPS_100 = 0x1C,
  TIM1_KBPS_125 = 0x1C,
  TIM1_KBPS_200 = 0xFA,
  TIM1_KBPS_250 = 0x1C,
  TIM1_KBPS_400 = 0xFA,
  TIM1_KBPS_500 = 0x1C,
  TIM1_KBPS_666 = 0xB6,
  TIM1_KBPS_800 = 0x16,
  TIM1_KBPS_1000 = 0x14
};

} // namespace EcanVCI
