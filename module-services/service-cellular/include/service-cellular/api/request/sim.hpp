// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "request.hpp"

namespace cellular::msg::request::sim
{
    /** Set active SIM card
     */
    struct SetActiveSim : public msg::Request
    {
        explicit SetActiveSim(api::SimSlot sim) : sim(sim)
        {}
        const api::SimSlot sim;

        using Response = msg::Response;
    };

    /** Get current state of PIN lock
     */
    struct GetLockState : public msg::Request
    {
        struct Response : public msg::Response
        {
            explicit Response(bool locked) : locked(locked)
            {}
            const bool locked;
        };
    };

    /** Unlock SIM using PIN
     */
    struct PinUnlock : msg::Request
    {
        explicit PinUnlock(const api::SimCode &pin) : pin(pin)
        {}
        const api::SimCode pin;

        using Response = msg::Response;
    };

    /** Change current PIN
     */
    struct ChangePin : msg::Request
    {
        explicit ChangePin(const api::SimCode &oldPin, const api::SimCode &pin) : oldPin(oldPin), pin(pin)
        {}
        const api::SimCode oldPin;
        const api::SimCode pin;

        using Response = msg::Response;
    };

    /** Reset PIN using PUK
     */
    struct UnblockWithPuk : msg::Request
    {
        explicit UnblockWithPuk(const api::SimCode &puk, const api::SimCode &pin) : puk(puk), pin(pin)
        {}
        const api::SimCode puk;
        const api::SimCode pin;

        using Response = msg::Response;
    };

    /** Enable or disable PIN lock on SIM
     */
    struct SetPinLock : msg::Request
    {
        explicit SetPinLock(api::SimLockState lock, const api::SimCode &pin) : lock(lock), pin(pin)
        {}
        const api::SimLockState lock;
        const api::SimCode pin;

        struct Response : msg::Response
        {
            explicit Response(bool retCode, api::SimLockState lock) : msg::Response(retCode), lock(lock)
            {}
            const api::SimLockState lock;
        };
    };
} // namespace cellular::msg::request::sim