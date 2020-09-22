#pragma once

#include <string>

#include <Common/Query.hpp>
#include <Interface/ThreadRecord.hpp>

namespace db::query
{
    class ThreadRemove : public Query
    {
      public:
        explicit ThreadRemove(unsigned int _id);

        [[nodiscard]] std::string debugInfo() const override;

        unsigned int id;
    };

    class ThreadRemoveResult : public QueryResult
    {
      public:
        explicit ThreadRemoveResult(bool _result);

        [[nodiscard]] bool success() const noexcept;
        [[nodiscard]] std::string debugInfo() const override;

      private:
        bool result;
    };
} // namespace db::query
