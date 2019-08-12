/*
 *  @file BoardDefinitions.hpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 09.08.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#ifndef PUREPHONE_BOARDDEFINITIONS_HPP
#define PUREPHONE_BOARDDEFINITIONS_HPP

#include "drivers/dmamux/DriverDMAMux.hpp"
#include "drivers/dma/DriverDMA.hpp"
#include "drivers/i2c/DriverI2C.hpp"
#include "drivers/pll/DriverPLL.hpp"

enum class BoardDefinitions{
    AUDIOCODEC_I2C_BAUDRATE = 100000,
    AUDIOCODEC_I2C = static_cast<int >(drivers::I2CInstances ::I2C2),
    AUDIOCODEC_DMAMUX = static_cast<int >(drivers::DMAMuxInstances ::DMAMUX0),
    AUDIOCODEC_DMA = static_cast<int >(drivers::DMAInstances ::DMA_0),

    KEYBOARD_I2C_BAUDRATE = AUDIOCODEC_I2C_BAUDRATE,
    KEYBOARD_I2C = AUDIOCODEC_I2C,

    BATTERY_CHARGER_I2C_BAUDRATE = AUDIOCODEC_I2C_BAUDRATE,
    BATTERY_CHARGER_I2C = AUDIOCODEC_I2C,

    AUDIO_PLL = static_cast<int >(drivers::PLLInstances::Audio),


};

#endif //PUREPHONE_BOARDDEFINITIONS_HPP
