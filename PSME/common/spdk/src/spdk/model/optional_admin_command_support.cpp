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
 * @file optional_admin_command_support.cpp
 */

#include "spdk/model/optional_admin_command_support.hpp"



using namespace spdk;

constexpr const char model::OptionalAdminCommandSupport::NS_MANAGE[];
constexpr const char model::OptionalAdminCommandSupport::SECURITY[];
constexpr const char model::OptionalAdminCommandSupport::FIRMAWARE[];
constexpr const char model::OptionalAdminCommandSupport::FORMAT[];


void model::from_json(const json::Json& json, model::OptionalAdminCommandSupport& csts) {
    csts.set_ns_manage(json.at(model::OptionalAdminCommandSupport::NS_MANAGE).get<std::uint64_t>());
    csts.set_security(json.at(model::OptionalAdminCommandSupport::SECURITY).get<std::uint64_t>());
    csts.set_firmware(json.at(model::OptionalAdminCommandSupport::FIRMAWARE).get<std::uint64_t>());
    csts.set_format(json.at(model::OptionalAdminCommandSupport::FORMAT).get<std::uint64_t>());
}


void model::to_json(json::Json& json, const model::OptionalAdminCommandSupport& csts) {
    json = json::Json{
        {model::OptionalAdminCommandSupport::NS_MANAGE, csts.get_ns_manage()},
        {model::OptionalAdminCommandSupport::SECURITY,  csts.get_security()},
        {model::OptionalAdminCommandSupport::FIRMAWARE, csts.get_firmware()},
        {model::OptionalAdminCommandSupport::FORMAT,    csts.get_format()}
    };
}


std::string model::OptionalAdminCommandSupport::to_string() const {

    std::stringstream ss{};
    ss << "   NS Manage: " << get_ns_manage() << std::endl;
    ss << "   Security: " << get_security() << std::endl;
    ss << "   Firmware: " << get_firmware() << std::endl;
    ss << "   Format: " << get_format() << std::endl;

    return ss.str();
}