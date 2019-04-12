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
 * @file lvol_driver.cpp
 */


#include "json-wrapper/json-wrapper.hpp"
#include "spdk/model/drivers/lvol_driver.hpp"



using namespace spdk;

const char model::drivers::LvolDriver::DRIVER_NAME[] = "lvol";

namespace {

constexpr const char BASE_BDEV[] = "base_bdev";
constexpr const char CLONE[] = "clone";
constexpr const char SNAPSHOT[] = "snapshot";
constexpr const char LVOL_STORE_UUID[] = "lvol_store_uuid";
constexpr const char THIN_PROVISION[] = "thin_provision";
constexpr const char CLONES[] = "clones";
constexpr const char BASE_SNAPSHOT[] = "base_snapshot";

}


void model::drivers::to_json(json::Json& json, const LvolDriver& lvol_driver) {
    json = json::Json{
        {::BASE_BDEV,       lvol_driver.get_base_bdev()},
        {::CLONE,           lvol_driver.is_clone()},
        {::SNAPSHOT,        lvol_driver.is_snapshot()},
        {::LVOL_STORE_UUID, lvol_driver.get_lvol_store_uuid()},
        {::THIN_PROVISION,  lvol_driver.is_thin_provision()},
        {::BASE_SNAPSHOT,   lvol_driver.get_base_snapshot()},
        {::CLONES,          lvol_driver.get_clones()}
    };
}


void model::drivers::from_json(const json::Json& json, LvolDriver& lvol_driver) {
    lvol_driver.set_base_bdev(json.at(::BASE_BDEV).get<std::string>());
    lvol_driver.set_clone(json.at(::CLONE).get<bool>());
    lvol_driver.set_snapshot(json.at(::SNAPSHOT).get<bool>());
    lvol_driver.set_lvol_store_uuid(json.at(::LVOL_STORE_UUID).get<std::string>());
    lvol_driver.set_thin_provision(json.at(::THIN_PROVISION).get<bool>());
    lvol_driver.set_base_snapshot(json.value(::BASE_SNAPSHOT, json::Json{}).get<JsonOptional<std::string>>());
    lvol_driver.set_clones(json.value(::CLONES, json::Json{}).get<JsonOptional<std::vector<std::string>>>());
}


std::string model::drivers::LvolDriver::to_string() const {
    std::stringstream ss{};

    ss << std::endl << " Base Bdev: " << m_base_bdev << std::endl;
    ss << std::endl << " Clone: " << std::boolalpha << m_clone << std::endl;
    ss << std::endl << " Snapshot: " << std::boolalpha << m_snapshot << std::endl;
    ss << std::endl << " Lvol Store UUID: " << m_lvol_store_uuid << std::endl;
    ss << std::endl << " Thin provision: " << std::boolalpha << m_thin_provision << std::endl;

    return ss.str();
}





