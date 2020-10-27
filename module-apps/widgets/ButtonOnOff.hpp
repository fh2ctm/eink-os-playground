// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Label.hpp"

namespace gui
{
    enum class ButtonType
    {
        On,
        Off
    };

    class ButtonOnOff : public Label
    {
      public:
        ButtonOnOff(Item *parent, ButtonType buttonType);
    };

} /* namespace gui */
