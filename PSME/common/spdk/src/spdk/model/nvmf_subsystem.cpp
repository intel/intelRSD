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
 * @file nvmf_subsystem.cpp
 */

#include "spdk/model/nvmf_subsystem.hpp"



using namespace spdk;

constexpr const char model::NvmfSubsystem::NQN[];
constexpr const char model::NvmfSubsystem::SUBTYPE[];
constexpr const char model::NvmfSubsystem::LISTEN_ADDRESSES[];
constexpr const char model::NvmfSubsystem::LISTEN_ADDRESS[];
constexpr const char model::NvmfSubsystem::HOST[];
constexpr const char model::NvmfSubsystem::HOSTS[];
constexpr const char model::NvmfSubsystem::ALLOW_ANY_HOST[];
constexpr const char model::NvmfSubsystem::SERIAL_NUMBER[];
constexpr const char model::NvmfSubsystem::NAMESPACE[];
constexpr const char model::NvmfSubsystem::NAMESPACES[];
constexpr const char model::NvmfSubsystem::MAX_NAMESPACES[];
constexpr const char model::NvmfSubsystem::DISCOVERY[];


void model::from_json(const json::Json& json, model::NvmfSubsystem& nvmf_subsystem) {

    nvmf_subsystem.set_nqn(json.at(model::NvmfSubsystem::NQN));
    nvmf_subsystem.set_subtype(json.at(model::NvmfSubsystem::SUBTYPE));
    nvmf_subsystem.set_serial_number(json.value(model::NvmfSubsystem::SERIAL_NUMBER, json::Json{}).get<JsonOptional<std::string>>());
    nvmf_subsystem.set_listen_addresses(json.at(model::NvmfSubsystem::LISTEN_ADDRESSES));
    nvmf_subsystem.set_hosts(json.at(model::NvmfSubsystem::HOSTS));

    /*TODO: remove after enabling optional fields*/
    if (json.count(model::NvmfSubsystem::NAMESPACES) != 0) {
        nvmf_subsystem.set_namespaces(json.at(model::NvmfSubsystem::NAMESPACES));
    }

    nvmf_subsystem.set_allow_any_host(json.at(model::NvmfSubsystem::ALLOW_ANY_HOST));
}


void model::to_json(json::Json& json, const model::NvmfSubsystem& nvmf_subsystem) {

    json = json::Json{
        {model::NvmfSubsystem::NQN,              nvmf_subsystem.get_nqn()},
        {model::NvmfSubsystem::SERIAL_NUMBER,    nvmf_subsystem.get_serial_number()},
        {model::NvmfSubsystem::SUBTYPE,          nvmf_subsystem.get_subtype()},
        {model::NvmfSubsystem::LISTEN_ADDRESSES, nvmf_subsystem.get_listen_addresses()},
        {model::NvmfSubsystem::HOSTS,            nvmf_subsystem.get_hosts()},
        {model::NvmfSubsystem::ALLOW_ANY_HOST,   nvmf_subsystem.is_allow_any_host()},
        {model::NvmfSubsystem::NAMESPACES,       nvmf_subsystem.get_namespaces()},
    };

}


std::string model::NvmfSubsystem::to_string() const {

    std::stringstream ss{};
    ss << "=======NVMf Subsystem=======" << std::endl;
    ss << "NQN: " << get_nqn() << std::endl;
    ss << "Subtype: " << get_subtype() << std::endl;

    ss << "Hosts: [" << std::endl;
    for (const auto& host : get_hosts()) {
        ss << host.to_string() << std::endl;
    }
    ss << "]" << std::endl;

    ss << "Listen addresses: [" << std::endl;
    for (const auto& address : get_listen_addresses()) {
        ss << address.to_string() << std::endl;
    }
    ss << "]" << std::endl;

    ss << "Namespaces: [" << std::endl;
    for (const auto& ns : get_namespaces()) {
        ss << ns.to_string() << std::endl;
    }
    ss << "]" << std::endl;

    ss << "Allow any host: " << std::boolalpha << is_allow_any_host() << std::endl;
    ss << "============================" << std::endl;

    return ss.str();
}

