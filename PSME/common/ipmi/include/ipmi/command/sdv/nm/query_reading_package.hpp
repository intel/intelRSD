/*!
 * @brief QueryReadingPackage IPMI command request and response.
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
 * @file ipmi/command/sdv/query_reading_package.hpp
 */

#pragma once



#include "ipmi/request.hpp"
#include "ipmi/response.hpp"
#include "ipmi/command/sdv/enums.hpp"
#include "reading.hpp"

namespace ipmi {
namespace command {
namespace sdv {

namespace request {

/*!
 * @brief Request message for QueryReadingPackage command.
 */
class QueryReadingPackage : public Request {
public:
    /*!
     * @brief Constructor.
     */
    QueryReadingPackage();


    QueryReadingPackage(const QueryReadingPackage&) = default;


    QueryReadingPackage(QueryReadingPackage&&) = default;


    virtual ~QueryReadingPackage();


    const char* get_command_name() const override {
        return "QueryReadingPackage";
    }

    /*
     * @brief Reading package selector
     * @param package_guid ReadingPackage identifier
     */
    void set_package_guid(std::uint32_t package_guid) {
        m_package_guid = package_guid;
    }

    /*
     * @brief Start reading index setter.
     * @param start_reading_index Index of first reading to be retrieved (zero based)
     */
    void set_start_reading_index(std::uint8_t start_reading_index) {
        m_start_reading_index = start_reading_index;
    }

private:

    std::uint32_t m_package_guid{0};
    std::uint8_t m_start_reading_index{0};

    void pack(IpmiInterface::ByteBuffer& data) const override;
};

}

namespace response {

/*!
 * @brief Response message for QueryReadingPackage command.
 */
class QueryReadingPackage : public Response {
public:

    /*!
     * @brief Constructor.
     */
    QueryReadingPackage();


    QueryReadingPackage(const QueryReadingPackage&) = default;


    QueryReadingPackage(QueryReadingPackage&&) = default;


    virtual ~QueryReadingPackage();


    const char* get_command_name() const override {
        return "QueryReadingPackage";
    }

    /*
     * @brief Returns Next reading index.
     * The index of the next not returned reading or 0 if all readings returned.
     *
     * @return Next reading index
     */
    std::uint8_t get_next_reading_index() const {
        return m_next_reading_index;
    }

    /*!
     * @brief Returns reporting interval value.
     * @return Reporting interval for pushing data, 0 = disabled.
     */
    deciseconds get_reporting_interval() const {
        return m_reporting_inverval;
    }

    /*!
     * @brief Returns MetricId collection of given package.
     * @return MetricId collection.
     */
    const MetricIdVec& get_metric_ids() const {
        return m_metric_ids;
    }

private:
    static constexpr const std::size_t MIN_RESPONSE_SIZE = 7;
    std::uint8_t m_next_reading_index{};
    deciseconds m_reporting_inverval{};
    MetricIdVec m_metric_ids{};

    void unpack(const IpmiInterface::ByteBuffer& data) override;

    void throw_error_on_completion_code(CompletionCode completion_code) const override;
};

}

}
}
}
