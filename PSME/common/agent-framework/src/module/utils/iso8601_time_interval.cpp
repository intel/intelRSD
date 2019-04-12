/*!
 * @brief Iso8601TimeInterval implementation.
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
 * @file utils/iso8601_time_interval.cpp
 */

#include "agent-framework/module/utils/iso8601_time_interval.hpp"

#include <limits>
#include <sstream>
#include <regex>

using namespace std::chrono;

namespace {

constexpr char ISO8601_INTERVAL_PATTERN[] = "P(T(?:([0-9]+)H)?(?:([0-9]+)M)?(?:([0-9]+)(?:[.,]([0-9]{0,3}))?S)?)?";

int digits2int(const std::string& digits) {
    if (!digits.empty()) {
        return std::stoi(digits);
    }
    return 0;
}

}


namespace agent_framework {
namespace utils {


Iso8601TimeInterval::Exception::~Exception() {}


constexpr std::chrono::milliseconds Iso8601TimeInterval::MAX_MILLIS;


Iso8601TimeInterval::Iso8601TimeInterval(std::chrono::milliseconds millis) : m_milliseconds{millis} {
    if (m_milliseconds > MAX_MILLIS) {
        throw Exception("out of range");
    }
}


Iso8601TimeInterval Iso8601TimeInterval::max() {
    return Iso8601TimeInterval(MAX_MILLIS);
}


Iso8601TimeInterval Iso8601TimeInterval::min() {
    return Iso8601TimeInterval();
}


Iso8601TimeInterval Iso8601TimeInterval::zero() {
    return Iso8601TimeInterval();
}


Iso8601TimeInterval Iso8601TimeInterval::parse(const std::string& iso8601_interval_str) {
    static const std::regex iso8601_interval_regex(ISO8601_INTERVAL_PATTERN, std::regex_constants::icase);
    std::smatch part_match{};
    if (std::regex_match(iso8601_interval_str, part_match, iso8601_interval_regex)) {
        if (0 != part_match[1].str().compare("T")
            && 0 != part_match[1].str().compare("t")) {
            const auto hour_match = part_match[2].str();
            const auto minute_match = part_match[3].str();
            const auto second_match = part_match[4].str();
            auto fraction_match = (part_match[5].str() + "000").substr(0,3);
            if (!hour_match.empty() || !minute_match.empty() || !second_match.empty() || !fraction_match.empty()) {
                try {
                    auto millis =  duration_cast<milliseconds>(hours(digits2int(hour_match)))
                                 + duration_cast<milliseconds>(minutes(digits2int(minute_match)))
                                 + duration_cast<milliseconds>(seconds(digits2int(second_match)))
                                 + milliseconds(digits2int(fraction_match));
                    return Iso8601TimeInterval(millis);
                }
                catch(...) {
                    throw Exception("cannot parse string: '" + iso8601_interval_str + "' to a duration.");
                }
            }
        }
    }
    throw Exception("cannot parse string: '" + iso8601_interval_str + "' to a duration.");
}


std::string Iso8601TimeInterval::to_string() const {
    using namespace std::chrono;
    if (milliseconds::zero() == m_milliseconds) {
        return "PT0S";
    }
    std::ostringstream result{};
    result << "PT";
    auto millis = m_milliseconds;
    auto hr = duration_cast<hours>(millis);
    if (hr > hours(std::numeric_limits<int>::max())) {
        hr = hours(std::numeric_limits<int>::max());
    }
    millis -= hr;
    auto mins = duration_cast<minutes>(millis);
    if (mins > minutes(std::numeric_limits<int>::max())) {
        mins = minutes(std::numeric_limits<int>::max());
    }
    millis -= mins;
    auto secs = duration_cast<seconds>(millis);
    millis -= secs;
    if (hr != hours::zero()) {
        result << hr.count() << 'H';
    }
    if (mins != minutes::zero()) {
        result << mins.count() << 'M';
    }
    if (secs == seconds::zero() && millis == milliseconds::zero()) {
        return result.str();
    }
    result << secs.count();
    if (millis != milliseconds::zero()) {
        result << '.';
        const auto millis_str = std::to_string(millis.count());
        // omit all ending zeros
        auto rit = millis_str.crbegin();
        while('0' == *rit) {
            ++rit;
        }
        auto end_it = rit.base();
        for (auto it = millis_str.cbegin(); it != end_it; ) {
            result << *it++;
        }
    }
    result << 'S';
    return result.str();
}

}
}
