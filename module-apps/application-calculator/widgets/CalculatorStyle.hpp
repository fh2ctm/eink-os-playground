// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <module-gui/gui/widgets/Style.hpp>

namespace style::calculator
{
    inline constexpr auto grid_cells        = 9;
    inline constexpr auto equals            = "app_calculator_equals";
    inline constexpr auto decimal_separator = "app_calculator_decimal_separator";

    namespace window
    {
        inline constexpr auto math_box_height      = 240;
        inline constexpr auto math_box_offset_top  = style::header::height + 200;
        inline constexpr auto math_box_cell_height = 80;
        inline constexpr auto math_box_cell_width  = style::window::default_body_width / 3;
        inline constexpr auto input_offset_top     = style::header::height + 20;
        inline constexpr auto input_height         = 100;
        inline constexpr auto input_width          = 380;
        inline constexpr auto input_margin         = 50;
    } // namespace window
} // namespace style::calculator
