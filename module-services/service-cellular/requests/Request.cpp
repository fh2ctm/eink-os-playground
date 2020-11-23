// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <string>
#include <memory>

#include <at/Result.hpp>
#include <at/Commands.hpp>

#include "service-cellular/requests/Request.hpp"

namespace cellular
{
    void Request::setHandled(bool handled)
    {
        isRequestHandled = handled;
    }
    bool Request::isHandled() const noexcept
    {
        return isRequestHandled;
    }
    bool Request::checkModemResponse(const at::Result &result)
    {
        return result.code == at::Result::Code::OK;
    }
    bool Request::isValid() const noexcept
    {
        return true;
    }
}; // namespace cellular