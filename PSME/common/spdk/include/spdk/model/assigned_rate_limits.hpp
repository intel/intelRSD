/*!
 * @copyright Copyright (c) 2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file assigned_rate_limits.hpp
 */

#include <cstdint>
#include <string>
#include "json-wrapper/json-wrapper.hpp"
#include "spdk/json_optional.hpp"



#pragma once

/*!
 * @brief Implementation of AssignedRateLimits model based on SPDK
 */
namespace spdk {
namespace model {
class AssignedRateLimits {

public:
    static constexpr const char RW_MBYTES_PER_SEC[] = "rw_mbytes_per_sec";
    static constexpr const char W_MBYTES_PER_SEC[] = "w_mbytes_per_sec";
    static constexpr const char R_MBYTES_PER_SEC[] = "r_mbytes_per_sec";
    static constexpr const char RW_IOS_PER_SEC[] = "rw_ios_per_sec";


    /*!
     * @brief Lists current info about AssignedRateLimits
     * @return info about AssignedRateLimits as a string
     */
    std::string to_string() const;


    /*!
     * @brief Get number of R/W megabytes per second to allow.
     * @return number of R/W megabytes
     */
    const JsonOptional<uint64_t>& get_rw_mbytes_per_sec() const {
        return m_rw_mbytes_per_sec;
    }


    /*!
     * @brief Set number of R/W megabytes per second to allow.
     * @param number of R/W megabytes per second
     */
    void set_rw_mbytes_per_sec(const JsonOptional<uint64_t>& rw_mbytes_per_sec) {
        m_rw_mbytes_per_sec = rw_mbytes_per_sec;
    }


    /*!
     * @brief Get number of Write megabytes per second to allow.
     * @return number of Write megabytes
     */
    const JsonOptional<uint64_t>& get_w_mbytes_per_sec() const {
        return m_w_mbytes_per_sec;
    }


    /*!
     * @brief Set number of Write megabytes per second to allow.
     * @param number of Write megabytes
     */
    void set_w_mbytes_per_sec(const JsonOptional<uint64_t>& w_mbytes_per_sec) {
        m_w_mbytes_per_sec = w_mbytes_per_sec;
    }


    /*!
     * @brief Get number of Read megabytes per second to allow.
     * @return number of Read megabytes
     */
    const JsonOptional<uint64_t>& get_r_mbytes_per_sec() const {
        return m_r_mbytes_per_sec;
    }


    /*!
     * @brief Set number of Read megabytes per second to allow.
     * @param number of Read megabytes
     */
    void set_r_mbytes_per_sec(const JsonOptional<uint64_t>& r_mbytes_per_sec) {
        m_r_mbytes_per_sec = r_mbytes_per_sec;
    }


    /*!
     * @brief Get number of R/W I/Os per second to allow.
     * @return number of R/W I/Os
     */
    const JsonOptional<uint64_t>& get_rw_ios_per_sec() const {
        return m_rw_ios_per_sec;
    }


    /*!
     * @brief Set number of R/W I/Os per second to allow.
     * @param number of R/W I/Os
     */
    void set_rw_ios_per_sec(const JsonOptional<uint64_t>& rw_ios_per_sec) {
        m_rw_ios_per_sec = rw_ios_per_sec;
    }


private:
    JsonOptional<uint64_t> m_rw_mbytes_per_sec{};
    JsonOptional<uint64_t> m_w_mbytes_per_sec{};
    JsonOptional<uint64_t> m_r_mbytes_per_sec{};
    JsonOptional<uint64_t> m_rw_ios_per_sec{};

};


/*!
 * @brief converts from JSON to AssignedRateLimits
 * @param json json object
 * @param assigned_rate_limits AssignedRateLimits object
 */
void from_json(const json::Json& json, AssignedRateLimits& assigned_rate_limits);


/*!
 * @brief converts from AssignedRateLimits to Json
 * @param json Json object
 * @param assigned_rate_limits AssignedRateLimits object
 */
void to_json(json::Json& json, const AssignedRateLimits& assigned_rate_limits);
}
}


