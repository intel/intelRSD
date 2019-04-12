/*!
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file reserve_sdr_repository_test.cpp
 */

#include "ipmi/command/generic/reserve_sdr_repository.hpp"
#include <gtest/gtest.h>

using namespace std;
using namespace ipmi::command::generic;

namespace {

constexpr const std::uint16_t RESERVATION_ID = 0x1234;

}

TEST(ReserveSdrRepositoryRequest, PackRequestData) {
    request::ReserveSdrRepository request{};
    const auto req_data = request.do_pack();
    ASSERT_TRUE(req_data.empty());
}

TEST(ReserveSdrRepositoryResponse, GetReservationId) {
    ipmi::IpmiInterface::ByteBuffer data = {
        ipmi::COMPLETION_CODE_NORMAL,
        std::uint8_t(RESERVATION_ID),  // LS byte first
        std::uint8_t(RESERVATION_ID >> 8)
    };
    response::ReserveSdrRepository response;
    ASSERT_EQ(0, response.get_reservation_id());
    response.do_unpack(data);
    ASSERT_EQ(RESERVATION_ID, response.get_reservation_id());
}
