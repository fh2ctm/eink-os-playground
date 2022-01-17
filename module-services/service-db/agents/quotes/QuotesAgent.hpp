﻿// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <service-db/QuotesMessages.hpp>
#include <Interface/Record.hpp>
#include "RandomizedQuoteModel.hpp"

namespace Quotes
{
    enum class QuotesRecordField
    {
    };

    class QuotesAgent : public RecordInterface<QuoteRecord, QuotesRecordField>
    {
      public:
        QuotesAgent(Database *quotesDB, std::unique_ptr<settings::Settings> settings);
        ~QuotesAgent() = default;

        auto runQuery(std::shared_ptr<db::Query> query) -> std::unique_ptr<db::QueryResult>;

      protected:
        auto handleCategoryList(std::shared_ptr<db::Query> msg) -> std::unique_ptr<db::QueryResult>;
        auto handleEnableCategoryById(std::shared_ptr<db::Query> query) -> std::unique_ptr<db::QueryResult>;
        auto handleQuotesList(std::shared_ptr<db::Query> query) -> std::unique_ptr<db::QueryResult>;
        auto handleQuotesListByCategoryId(std::shared_ptr<db::Query> query) -> std::unique_ptr<db::QueryResult>;
        auto handleQuotesListFromCustomCategory(std::shared_ptr<db::Query> query) -> std::unique_ptr<db::QueryResult>;
        auto handleEnabledQuotesList(std::shared_ptr<db::Query> query) -> std::unique_ptr<db::QueryResult>;
        auto handleEnableQuoteById(std::shared_ptr<db::Query> query) -> std::unique_ptr<db::QueryResult>;
        auto handleAddQuote(std::shared_ptr<db::Query> query) -> std::unique_ptr<db::QueryResult>;
        auto handleReadQuote(std::shared_ptr<db::Query> query) -> std::unique_ptr<db::QueryResult>;
        auto handleReadRandomizedQuote(std::shared_ptr<db::Query> query) -> std::unique_ptr<db::QueryResult>;
        auto handleWriteQuote(std::shared_ptr<db::Query> query) -> std::unique_ptr<db::QueryResult>;
        auto handleDeleteQuote(std::shared_ptr<db::Query> query) -> std::unique_ptr<db::QueryResult>;

      private:
        Database *database;
        RandomizedQuoteModel randomizedQuoteModel;
    };
} // namespace Quotes
