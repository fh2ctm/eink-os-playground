// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "bsp/usb/usb.hpp"
#include <termios.h>
#include <fcntl.h>

namespace bsp
{
    int fd;
    xQueueHandle USBReceiveQueue;

    void usbDeviceTask(void *ptr)
    {
        usbCDCReceive(ptr);
    }

    int usbCDCReceive(void *)
    {
        LOG_INFO("[ServiceDesktop:BSP_Driver] Start reading on fd:%d", fd);
        uint8_t inputData[SERIAL_BUFFER_LEN];
        static std::string receiveMsg;

        while (1) {
            if (uxQueueSpacesAvailable(USBReceiveQueue) != 0) {
                ssize_t length = read(fd, &inputData[0], SERIAL_BUFFER_LEN);
                if (length > 0) {
                    receiveMsg = std::string(inputData, inputData + length);
                    LOG_DEBUG("[ServiceDesktop:BSP_Driver] Received: %d signs", static_cast<int>(length));
                    xQueueSend(USBReceiveQueue, &receiveMsg, portMAX_DELAY);
                }
                else {
                    // yielding task because nothing in a buffer
                    vTaskDelay(10);
                }
            }
            else {
                LOG_DEBUG("[ServiceDesktop:BSP_Driver] USB receive Queue is full, yielding task");
                vTaskDelay(1000);
            }
        }
    }

    int usbCDCSend(std::string *sendMsg)
    {
        ssize_t t = write(fd, (*sendMsg).c_str(), (*sendMsg).length());
        delete sendMsg;

        if (t >= 0) {
            LOG_DEBUG("[ServiceDesktop:BSP_Driver] Send: %d signs", static_cast<int>(t));
            return 0;
        }
        else {
            LOG_ERROR("[ServiceDesktop:BSP_Driver] Writing to PTY failed with code: %d", errno);
            return -1;
        }
    }

    int usbInit(xQueueHandle receiveQueue, USBDeviceListener *)
    {

        fd = 0;
        fd = open("/dev/ptmx", O_RDWR | O_NOCTTY);
        if (fd == -1) {
            LOG_ERROR("bsp::usbInit Failed to open /dev/ptmx, can't allocate new pseudo terminal");
            return -1;
        }

        grantpt(fd);
        unlockpt(fd);

        char *pts_name = ptsname(fd);
        if (pts_name == nullptr) {
            LOG_ERROR("bsp::usbInit ptsname returned NULL, no pseudo terminal allocated");
            return -1;
        }
        LOG_INFO("bsp::usbInit linux ptsname: %s", pts_name);
        struct termios newtio;
        memset(&newtio, 0, sizeof(newtio));
        struct termios oldtio;
        tcgetattr(fd, &oldtio);

        newtio             = oldtio;
        newtio.c_cflag     = SERIAL_BAUDRATE | CS8 | CLOCAL | CREAD;
        newtio.c_iflag     = 0;
        newtio.c_oflag     = 0;
        newtio.c_lflag     = 0;
        newtio.c_cc[VMIN]  = 1;
        newtio.c_cc[VTIME] = 0;
        tcflush(fd, TCIFLUSH);

        cfsetispeed(&newtio, SERIAL_BAUDRATE);
        cfsetospeed(&newtio, SERIAL_BAUDRATE);
        tcsetattr(fd, TCSANOW, &newtio);

        xTaskHandle taskHandleReceive;
        USBReceiveQueue = receiveQueue;

        BaseType_t task_error = xTaskCreate(&bsp::usbDeviceTask,
                                            "USBLinuxReceive",
                                            SERIAL_BUFFER_LEN * 8,
                                            nullptr,
                                            tskIDLE_PRIORITY,
                                            &taskHandleReceive);

        if (task_error != pdPASS) {
            LOG_ERROR("bsp::usbInit Failed to start freertos USB_Linux_Receive");
            return -1;
        }

        return 0;
    }
} // namespace bsp
