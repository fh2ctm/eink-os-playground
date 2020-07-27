#include "bsp/torch/torch.hpp"
#include "AL3644TT.hpp"

#include "bsp/BoardDefinitions.hpp"
#include "drivers/i2c/DriverI2C.hpp"

#include "fsl_common.h"

using namespace drivers;

static std::shared_ptr<drivers::DriverI2C> i2c;
static drivers::I2CAddress addr = {.deviceAddress = 0x63, .subAddressSize = 1};

static xQueueHandle qHandleIrq = NULL;

namespace bsp
{

    namespace torch
    {
        std::shared_ptr<DriverGPIO> gpio;
        const unsigned short max_current_mA = 150;

        int32_t init(xQueueHandle qHandle)
        {
            i2c = DriverI2C::Create(
                static_cast<I2CInstances>(BoardDefinitions::TORCH_DRIVER_I2C),
                DriverI2CParams{.baudrate = static_cast<uint32_t>(BoardDefinitions::TORCH_DRIVER_I2C_BAUDRATE)});

            qHandleIrq = qHandle;

            gpio =
                DriverGPIO::Create(static_cast<GPIOInstances>(BoardDefinitions::TORCH_DRIVER_GPIO), DriverGPIOParams{});

            // OUTPUT
            gpio->ConfPin(DriverGPIOPinParams{.dir      = DriverGPIOPinParams::Direction::Output,
                                              .irqMode  = DriverGPIOPinParams::InterruptMode::NoIntmode,
                                              .defLogic = 0,
                                              .pin      = static_cast<uint32_t>(BoardDefinitions::TORCH_DRIVER_EN)});
            gpio->WritePin(static_cast<uint32_t>(BoardDefinitions::TORCH_DRIVER_EN), 0);
            vTaskDelay(pdMS_TO_TICKS(5));
            gpio->WritePin(static_cast<uint32_t>(BoardDefinitions::TORCH_DRIVER_EN), 1);
            vTaskDelay(pdMS_TO_TICKS(5));

            if (isPresent()) {
                turn(false);
                setCurrent(max_current_mA);
                return kStatus_Success;
            }
            return kStatus_Fail;
        }

        void deinit()
        {
            qHandleIrq = NULL;
            turn(false);
        }

        bool isPresent(void)
        {
            al3644tt_device_id id;
            addr.subAddress = AL3644TT_DEVICE_ID_REG;
            auto read       = i2c->Read(addr, (uint8_t *)(&id), 1);

            if (read != 1) {
                return false;
            }

            if (id.device_id == AL3644TT_ID && (id.silicon_rev == AL3644TT_REV_1 || id.silicon_rev == AL3644TT_REV_2)) {
                return true;
            }
            LOG_WARN("Something is present at the torch LED driver address (0x%lx), but not the AL3644TT",
                     addr.deviceAddress);
            return false;
        }

        bool setCurrent(const unsigned short mA)
        {
            // set the same current for both channels
            addr.subAddress = AL3644TT_LED1_TORCH_BRIGHTNESS_REG;
            al3644tt_led1_torch_brightness led1_brightness{
                .brightness_code          = al3644tt_current_convert(mA > max_current_mA ? max_current_mA : mA),
                .led2_brightness_override = AL3644TT_LED1_TORCH_BRIGHTNESS_OVERRIDE,
            };
            auto wrote = i2c->Write(addr, (uint8_t *)(&led1_brightness), 1);
            if (wrote != 1) {
                return false;
            }
            return true;
        };
        bool turn(bool state)
        {
            addr.subAddress = AL3644TT_ENABLE_REG;
            al3644tt_enable_reg en_reg{
                .led1_en       = static_cast<uint8_t>(state == true ? AL3644TT_LED_ENABLED : AL3644TT_LED_DISABLED),
                .led2_en       = AL3644TT_LED_DISABLED,
                .mode          = 0b10,
                .torch_temp_en = 0,
                .strobe_en     = 0,
                .strobe_type   = 0,
                .tx_pin_en     = 0,
            };
            auto wrote = i2c->Write(addr, (uint8_t *)(&en_reg), 1);
            if (wrote != 1) {
                return false;
            }
            return true;
        }
        bool isOn()
        {
            addr.subAddress = AL3644TT_ENABLE_REG;
            al3644tt_enable_reg en_reg;
            auto read = i2c->Read(addr, (uint8_t *)(&en_reg), 1);
            if (read != 1) {
                return false;
            }
            return en_reg.led1_en == AL3644TT_LED_ENABLED;
        }

    } // namespace torch
} // namespace bsp
