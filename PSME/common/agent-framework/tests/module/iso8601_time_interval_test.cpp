/*!
 * @brief Iso8601TimeInterval tests.
 *
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
 * @file iso8601_time_interval_test.cpp
 */

#include "agent-framework/module/utils/iso8601_time_interval.hpp"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace agent_framework::utils;
using namespace std::chrono;

TEST(Iso8601TimeInterval, CastingIntervals) {
    EXPECT_EQ(10.0, Iso8601TimeInterval::parse("PT10S").as<duration<double>>().count());
    EXPECT_EQ(0.5, Iso8601TimeInterval::parse("PT0.5S").as<duration<double>>().count());
    EXPECT_EQ(milliseconds(1500), Iso8601TimeInterval::parse("PT1.5S").as<milliseconds>());
    EXPECT_EQ(seconds(1), Iso8601TimeInterval::parse("PT1.5S").as<seconds>());
}

TEST(Iso8601TimeInterval, compareOperators) {
    EXPECT_EQ(Iso8601TimeInterval(), Iso8601TimeInterval::zero());
    EXPECT_EQ(Iso8601TimeInterval::min(), Iso8601TimeInterval::zero());
    EXPECT_EQ(Iso8601TimeInterval::max(),
              Iso8601TimeInterval(hours(std::numeric_limits<int>::max())
                                  + minutes(std::numeric_limits<int>::max())
                                  + seconds(std::numeric_limits<int>::max()) + milliseconds(999)));
    EXPECT_EQ(Iso8601TimeInterval(milliseconds(10505)), Iso8601TimeInterval::parse("PT10.505S"));
    EXPECT_EQ(Iso8601TimeInterval(hours(24) + seconds(10) + milliseconds(505)), Iso8601TimeInterval::parse("PT24H10.505S"));
    EXPECT_EQ(Iso8601TimeInterval(duration_cast<milliseconds>(duration<double>(0.501))), Iso8601TimeInterval(milliseconds(501)));

    EXPECT_GT(Iso8601TimeInterval(milliseconds(10)), Iso8601TimeInterval(milliseconds(5)));
    EXPECT_GE(Iso8601TimeInterval(milliseconds(10)), Iso8601TimeInterval(milliseconds(5)));
    EXPECT_GE(Iso8601TimeInterval(milliseconds(10)), Iso8601TimeInterval(milliseconds(10)));

    EXPECT_LT(Iso8601TimeInterval(milliseconds(5)), Iso8601TimeInterval(milliseconds(10)));
    EXPECT_LE(Iso8601TimeInterval(milliseconds(5)), Iso8601TimeInterval(milliseconds(10)));
    EXPECT_LE(Iso8601TimeInterval(milliseconds(10)), Iso8601TimeInterval(milliseconds(10)));
}

TEST(Iso8601TimeInterval, toString) {
    EXPECT_EQ("PT0S", Iso8601TimeInterval::zero().to_string());
    EXPECT_EQ("PT2147483647H2147483647M2147483647.999S", Iso8601TimeInterval::max().to_string());
    EXPECT_EQ("PT0S", Iso8601TimeInterval::min().to_string());
    EXPECT_EQ("PT1H", Iso8601TimeInterval(hours(1)).to_string());
    EXPECT_EQ("PT1M", Iso8601TimeInterval(minutes(1)).to_string());
    EXPECT_EQ("PT1H1M", Iso8601TimeInterval(hours(1) + minutes(1)).to_string());
    EXPECT_EQ("PT1H1M1S", Iso8601TimeInterval(hours(1) + minutes(1) + seconds(1)).to_string());
    EXPECT_EQ("PT1H1M1.5S", Iso8601TimeInterval(hours(1) + minutes(1) + seconds(1) + milliseconds(500)).to_string());
    EXPECT_EQ("PT1H1M0.5S", Iso8601TimeInterval(hours(1) + minutes(1) + milliseconds(500)).to_string());
    EXPECT_EQ("PT10.505S", Iso8601TimeInterval::parse("PT10.505S").to_string());
    EXPECT_EQ("PT24H10.505S", Iso8601TimeInterval::parse("PT24H10.505S").to_string());
    EXPECT_EQ("PT12H", Iso8601TimeInterval::parse("PT11H59M60S").to_string());
    EXPECT_EQ("PT5H5M5.5S", Iso8601TimeInterval::parse("PT5H5M5.5S").to_string());
    EXPECT_EQ("PT2147483647H2147483647M2147483647.999S",
              Iso8601TimeInterval::parse("PT2147483647H2147483647M2147483647.999S").to_string());
    EXPECT_EQ("PT2147483647H2147483647M2147483647.999S", Iso8601TimeInterval(hours(std::numeric_limits<int>::max())
                                  + minutes(std::numeric_limits<int>::max())
                                  + seconds(std::numeric_limits<int>::max()) + milliseconds(999)).to_string());
}

TEST(Iso8601TimeInterval, IncorrectIntervals) {
    // incorrect formats
    EXPECT_THROW(Iso8601TimeInterval::parse("PT"), Iso8601TimeInterval::Exception);
    EXPECT_THROW(Iso8601TimeInterval::parse("10s"), Iso8601TimeInterval::Exception);
    EXPECT_THROW(Iso8601TimeInterval::parse("PT-1S"), Iso8601TimeInterval::Exception);
    // overflow
    EXPECT_THROW(Iso8601TimeInterval::parse("PT2147483648H"), Iso8601TimeInterval::Exception);
    EXPECT_THROW((Iso8601TimeInterval(milliseconds::max())), Iso8601TimeInterval::Exception);
}
