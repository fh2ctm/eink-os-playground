/*
 *  @file DriverGPIO.hpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 14.08.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#ifndef PUREPHONE_DRIVERGPIO_HPP
#define PUREPHONE_DRIVERGPIO_HPP

#include "../DriverInterface.hpp"
#include <functional>

namespace drivers {


    enum class GPIOInstances {
        GPIO_1,
        GPIO_2,
        GPIO_3,
        GPIO_4,
        GPIO_5
    };

    struct DriverGPIOParams {

    };

    struct DriverGPIOPinParams{
        enum class Direction{
            Input=0, /*!< Set current pin as digital input.*/
            Output /*!< Set current pin as digital output.*/
        };

        enum class InterruptMode{
            NoIntmode = 0U,              /*!< Set current pin general IO functionality.*/
            IntLowLevel = 1U,            /*!< Set current pin interrupt is low-level sensitive.*/
            IntHighLevel = 2U,           /*!< Set current pin interrupt is high-level sensitive.*/
            IntRisingEdge = 3U,          /*!< Set current pin interrupt is rising-edge sensitive.*/
            IntFallingEdge = 4U,         /*!< Set current pin interrupt is falling-edge sensitive.*/
            IntRisingOrFallingEdge = 5U, /*!< Enable the edge select bit to override the ICR register's configuration.*/
        };

        Direction dir;
        InterruptMode irqMode;
        uint8_t defLogic;
        uint32_t pin;
    };

    class DriverGPIO : public DriverInterface<DriverGPIO> {
    public:
        static std::shared_ptr<DriverGPIO> Create(const GPIOInstances inst, const DriverGPIOParams &params);

        DriverGPIO(const DriverGPIOParams &params) : parameters(params) {}

        virtual ~DriverGPIO() {}

        virtual int32_t ConfPin(const DriverGPIOPinParams& params) = 0;

        // Sets the output level of the multiple GPIO pins to the logic 1.
        virtual void SetPort(const uint32_t mask) = 0;

        // Sets the output level of the multiple GPIO pins to the logic 0.
        virtual void ClearPort(const uint32_t mask) = 0;

        // Reverses the current output logic of the multiple GPIO pins.
        virtual void TogglePort(const uint32_t mask) = 0;

        // Sets the output level of the individual GPIO pin to logic 1 or 0.
        virtual void WritePin(const uint32_t pin, const uint8_t output) = 0;

        // Reads the current input value of the GPIO port.
        virtual uint8_t ReadPin(const uint32_t pin) = 0;

        virtual void EnableInterrupt(const uint32_t mask) = 0;

        virtual void DisableInterrupt(const uint32_t mask) = 0;


    protected:
        const DriverGPIOParams parameters;

    };

}


#endif //PUREPHONE_DRIVERGPIO_HPP
