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
 * @file listen_addess.cpp
 */


#include "spdk/json_optional.hpp"
#include "spdk/model/listen_address.hpp"



using namespace spdk;

constexpr const char model::ListenAddress::TRTYPE[];
constexpr const char model::ListenAddress::ADRFAM[];
constexpr const char model::ListenAddress::TRADDR[];
constexpr const char model::ListenAddress::TRSVCID[];

constexpr const char model::AddressFamily::IPv4[];
constexpr const char model::AddressFamily::IPv6[];
constexpr const char model::AddressFamily::IB[];
constexpr const char model::AddressFamily::FC[];

constexpr const char model::TransportType::RDMA[];


void model::from_json(const json::Json& json, model::ListenAddress& listen_address) {

    listen_address.set_trtype(json.at(model::ListenAddress::TRTYPE).get<std::string>());
    listen_address.set_adrfam(json.value(model::ListenAddress::ADRFAM, json::Json{}).get<JsonOptional<std::string>>());
    listen_address.set_traddr(json.at(model::ListenAddress::TRADDR).get<std::string>());
    listen_address.set_trsvcid(json.value(model::ListenAddress::TRSVCID, json::Json{}).get<JsonOptional<std::string>>());
}


void model::to_json(json::Json& json, const model::ListenAddress& listen_address) {

    json = json::Json{
        {model::ListenAddress::TRTYPE,  listen_address.get_trtype()},
        {model::ListenAddress::ADRFAM,  listen_address.get_adrfam()},
        {model::ListenAddress::TRADDR,  listen_address.get_traddr()},
        {model::ListenAddress::TRSVCID, listen_address.get_trsvcid()},
    };
}


std::string model::ListenAddress::to_string() const {

    std::stringstream ss{};
    ss << "   Trtype: " << get_trtype() << std::endl;
    if (get_adrfam()) {
        ss << "   Adrfram: " << get_adrfam().value() << std::endl;
    }
    ss << "   Traddr: " << get_traddr() << std::endl;
    if (get_trsvcid()) {
        ss << "   Trsvcid: " << get_trsvcid().value();
    }
    return ss.str();
}



