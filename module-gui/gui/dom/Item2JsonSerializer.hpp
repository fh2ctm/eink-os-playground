// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Item2JsonSerializingVisitor.hpp"
#include <module-utils/json/json11.hpp>
#include <list>

namespace gui
{
    class ItemNode;

    class Item2JsonSerializer
    {
        using prototype = std::map<int, std::list<json11::Json::object>>;
        json11::Json document;
        Item2JsonSerializingVisitor visitor;

        void handleSibling(gui::ItemNode &node, prototype &doc, int &level);
        void handleParent(gui::ItemNode &node, prototype &doc, int &level);
        void handleOther(gui::ItemNode &node, prototype &doc, int &level);

      public:
        void traverse(gui::Item &root);
        void dump(std::ostream &stream);
    };
} // namespace gui