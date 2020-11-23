// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "SupplementaryServicesRequest.hpp"

namespace cellular
{
    class ClipRequest : public SupplementaryServicesRequest
    {
      public:
        ClipRequest(const std::string &data, GroupMatch matchGroups) : SupplementaryServicesRequest(data, matchGroups)
        {}

        static std::unique_ptr<SupplementaryServicesRequest> create(const std::string &serviceCode,
                                                                    const std::string &data,
                                                                    GroupMatch matchGroups);

        auto command() -> std::string final;
        void handle(RequestHandler &h, at::Result &result) final;
        auto isValid() const noexcept -> bool final;

      private:
        static constexpr auto clipServiceCode = "30";
    };
}; // namespace cellular