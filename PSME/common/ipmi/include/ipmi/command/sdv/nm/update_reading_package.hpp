/*!
 * @brief UpdateReadingPackage IPMI command request and response.
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
 * @file ipmi/command/sdv/update_reading_package.hpp
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
 * @brief Request message for UpdateReadingPackage command.
 */
class UpdateReadingPackage : public Request {
public:

    /*!
     * @brief Constructor.
     */
    UpdateReadingPackage();


    UpdateReadingPackage(const UpdateReadingPackage&) = default;


    UpdateReadingPackage(UpdateReadingPackage&&) = default;


    virtual ~UpdateReadingPackage();


    const char* get_command_name() const override {
        return "UpdateReadingPackage";
    }

    /*
     * @brief Reading package selector
     * @param package_guid ReadingPackage identifier
     */
    void set_package_guid(std::uint32_t package_guid);

    /*
     * @brief Enables append mode (new metrics are added to reading package).
     */
    void enable_append_mode();

    /*
     * @brief UpdateReadingPackage will remove selected package.
     */
    void delete_reading_package();

    /*!
     * @brief Package definition is volatile.
     */
    void set_package_volatile();

    /*!
     * @brief Package definition is stored on flash.
     */
    void set_package_persistent();

    /*!
     * @brief Reporting interval value setter.
     * @param interval reporting interval, default = 0 - disabled
     */
    void set_reporting_interval(const deciseconds interval);

    /*
     * @brief Sets metrics to be added to package.
     * @param metric_ids MetricId collection
     */
    void set_metric_ids(const MetricIdVec& metric_ids);

private:

    std::uint32_t m_package_guid{0};
    std::uint8_t m_reading_package_command{0};
    deciseconds m_reporting_interval{0};
    MetricIdVec m_readings{};

    void pack(IpmiInterface::ByteBuffer& data) const override;
};

}

namespace response {

/*!
 * @brief Response message for UpdateReadingPackage command.
 */
class UpdateReadingPackage : public Response {
public:

    /*!
     * @brief Constructor.
     */
    UpdateReadingPackage();


    UpdateReadingPackage(const UpdateReadingPackage&) = default;


    UpdateReadingPackage(UpdateReadingPackage&&) = default;


    virtual ~UpdateReadingPackage();


    const char* get_command_name() const override {
        return "UpdateReadingPackage";
    }

    /*
     * @brief Returns reading package identifier.
     * @return ReadingPackage identifier.
     */
    std::uint32_t get_reading_package_guid() const {
        return m_reading_package_guid;
    }

private:
    static constexpr const std::size_t RESPONSE_SIZE = 8;
    std::uint32_t m_reading_package_guid{};

    void unpack(const IpmiInterface::ByteBuffer& data) override;

    void throw_error_on_completion_code(CompletionCode completion_code) const override;
};

}

}
}
}
