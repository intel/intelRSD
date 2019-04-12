/*!
 * @brief Iso8601TimeInterval declaration.
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
 * @file agent-framework/module/utils/iso8601_time_interval.hpp
 */

#pragma once

#include <chrono>
#include <stdexcept>
#include <string>

namespace agent_framework {
namespace utils {


/*!
 * Class for storing ISO8601 time interval with millisecond precision.
 */
class Iso8601TimeInterval final {
public:
    /*!
     * @brief Exception type that will be thrown when an error condition is
     * discovered while parsing the Iso8601TimeInterval
     */
    class Exception: public std::runtime_error {
    public:
        /*!
         * @brief constructs exception object
         * @param what_arg Describes what happened
         */
        explicit Exception(const std::string& what_arg) :
            std::runtime_error("Iso8601TimeInterval - " + what_arg) {
        }

        Exception(const Exception&) = default;
        Exception& operator=(const Exception&) = default;
        Exception(Exception&&) = default;
        Exception& operator=(Exception&&) = default;

        virtual ~Exception();
    };

    /*!
     * Parse ISO8601 time interval.
     *
     * Parses only time part of ISO8601 interval (a part after "PT").
     *
     * @param iso8601_interval_str String representing ISO8601 time interval
     * @return Iso8601TimeInterval representing given interval.
     */
    static Iso8601TimeInterval parse(const std::string& iso8601_interval_str);

    /*!
     * Constructor
     * @param millis Number of milliseconds
     */
    explicit Iso8601TimeInterval(std::chrono::milliseconds millis = {});

    /*!
     * Maximal Iso8601TimeInterval
     */
    static Iso8601TimeInterval max();

    /*!
     * Minimal Iso8601TimeInterval
     */
    static Iso8601TimeInterval min();

    /*!
     * Zero Iso8601TimeInterval
     */
    static Iso8601TimeInterval zero();

    /*!
     * return ISO8601 string representation of time interval.
     */
    std::string to_string() const;

    /*!
     * Converts interval to std::chrono::duration
     */
    template<typename ToDur>
    ToDur as() const {
        return std::chrono::duration_cast<ToDur>(m_milliseconds);
    }

    /*!
     * return number of milliseconds in interval
     */
    const std::chrono::milliseconds& get_milliseconds() const {
        return m_milliseconds;
    }

    bool operator==(const Iso8601TimeInterval& other) const {
        return m_milliseconds == other.m_milliseconds;
    }

    bool operator!=(const Iso8601TimeInterval& other) const {
        return !this->operator==(other);
    }

    bool operator<(const Iso8601TimeInterval& other) const {
        return m_milliseconds < other.m_milliseconds;
    }

    bool operator<=(const Iso8601TimeInterval& other) const {
        return m_milliseconds <= other.m_milliseconds;
    }

    bool operator>(const Iso8601TimeInterval& other) const {
        return m_milliseconds > other.m_milliseconds;
    }

    bool operator>=(const Iso8601TimeInterval& other) const {
        return m_milliseconds >= other.m_milliseconds;
    }

private:

    static constexpr std::chrono::milliseconds MAX_MILLIS = std::chrono::hours(std::numeric_limits<int>::max())
                + std::chrono::minutes(std::numeric_limits<int>::max())
                + std::chrono::seconds(std::numeric_limits<int>::max())
                + std::chrono::milliseconds(999);

    std::chrono::milliseconds m_milliseconds{};
};

}
}
