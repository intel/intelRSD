/*!
 * @copyright Copyright (c) 2018-2019 Intel Corporation
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
 * @file optional_admin_command_support.hpp
 */

#pragma once



#include <string>
#include "json-wrapper/json-wrapper.hpp"



namespace spdk {
namespace model {


class OptionalAdminCommandSupport {
public:

    static constexpr const char NS_MANAGE[] = "ns_manage";
    static constexpr const char SECURITY[] = "security";
    static constexpr const char FIRMAWARE[] = "firmware";
    static constexpr const char FORMAT[] = "format";


    /*!
     * @brief Gets the NS Manage
     * @return NS Manage
     */
    std::uint64_t get_ns_manage() const {
        return m_ns_manage;
    }


    /*!
     * @brief Sets the NS Manage
     * @param ns_manage new NS Manage
     */
    void set_ns_manage(std::uint64_t ns_manage) {
        m_ns_manage = ns_manage;
    }


    /*!
     * @brief Gets the security
     * @return security
     */
    std::uint64_t get_security() const {
        return m_security;
    }


    /*!
     * @brief Sets the security
     * @param security new security
     */
    void set_security(std::uint64_t security) {
        m_security = security;
    }


    /*!
     * @brief Gets firmware
     * @return firmware
     */
    std::uint64_t get_firmware() const {
        return m_firmware;
    }


    /*!
     * @brief Sets firmware
     * @param firmware new firmware
     */
    void set_firmware(std::uint64_t firmware) {
        m_firmware = firmware;
    }


    /*!
     * @brief Gets format
     * @return format
     */
    std::uint64_t get_format() const {
        return m_format;
    }


    /*!
     * @brief Sets format
     * @param format new format
     */
    void set_format(std::uint64_t format) {
        m_format = format;
    }


    /*!
     * @brief Lists current info about Oacs
     * @return info about Oacs as a string
     */
    std::string to_string() const;


private:
    std::uint64_t m_ns_manage{};
    std::uint64_t m_security{};
    std::uint64_t m_firmware{};
    std::uint64_t m_format{};

};


/*!
 * @brief converts from JSON to OptionalAdminCommandSupport
 * @param json json object
 * @param oacs OptionalAdminCommandSupport object
 */
void from_json(const json::Json& json, OptionalAdminCommandSupport& oacs);


/*!
 * @brief converts from OptionalAdminCommandSupport to Json
 * @param json Json object
 * @param oacs OptionalAdminCommandSupport object
 */
void to_json(json::Json& json, const OptionalAdminCommandSupport& oacs);

}
}


