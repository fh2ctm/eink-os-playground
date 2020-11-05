// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <string>
#include <vector>

#include <FreeRTOS.h>
#include <at/ErrorCode.hpp>
#include <at/Commands.hpp>
#include <at/Result.hpp>
#include <mutex.hpp>
#include <task.h>

namespace at
{
    class Chanel
    {
      protected:
        constexpr static char cmdSeparator  = '\r';
        const std::array<char, 3> validTerm = {cmdSeparator, ',', '='};
        std::string formatCommand(const std::string &cmd) const;

        cpp_freertos::MutexStandard mutex;
        TaskHandle_t blockedTaskHandle = nullptr;

      public:
        static const std::string OK;
        static const std::string ERROR;
        static const std::string NO_CARRIER;
        static const std::string BUSY;
        static const std::string NO_ANSWER;
        static const std::string CME_ERROR;
        static const std::string CMS_ERROR;
        // /// other codes unused right now: Please see quectel QuectelEC2526EC21ATCommandsManualV1.3 page 21
        // const std::string Chanel::CONNECT = "CONNECT";
        // const std::string Chanel::RING = "RING";
        // const std::string Chanel::NO_DIALTONE = "NO DIALTONE";

        /// waits till ok or timeout
        virtual auto cmd(const std::string cmd, uint32_t timeout = at::default_timeout, size_t rxCount = 0)
            -> Result final;
        virtual auto cmd(const at::AT at) -> Result final;
        virtual auto cmd(at::Cmd &at) -> Result final;
        /// check for OK, ERROR in string in last token
        virtual Result::Code at_check(const std::vector<std::string> &arr);
        /// check for +CME ERROR: errors if exists in last token
        virtual bool at_check_cmx_error(const std::string &CMX, const std::vector<std::string> &arr, uint32_t &errcode);
        /// log result
        virtual void cmd_log(std::string cmd, const Result &result, uint32_t timeout) final;

        virtual void cmd_init()                        = 0;
        virtual void cmd_send(std::string cmd)         = 0;
        virtual std::vector<std::string> cmd_receive() = 0;
        virtual void cmd_post()                        = 0;
    };
}; // namespace at
