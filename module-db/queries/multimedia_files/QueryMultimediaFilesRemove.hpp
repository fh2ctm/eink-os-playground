// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Common/Query.hpp>

#include <string>

namespace db::multimedia_files::query
{
    class Remove : public Query
    {
      public:
        explicit Remove(uint32_t id);
        [[nodiscard]] auto debugInfo() const -> std::string override;

        const uint32_t id;
    };

    class RemoveResult : public QueryResult
    {
        const bool ret = true;

      public:
        explicit RemoveResult(bool ret);
        [[nodiscard]] auto getResult() const -> bool;
        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

    class RemoveAll : public Query
    {
      public:
        explicit RemoveAll();
        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

    class RemoveAllResult : public QueryResult
    {
        const bool ret = true;

      public:
        explicit RemoveAllResult(bool ret);
        [[nodiscard]] auto getResult() const -> bool;
        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

} // namespace db::multimedia_files::query
