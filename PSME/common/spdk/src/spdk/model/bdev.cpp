/*!
 * @brief Bdev class implementation.
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions an
 * limitations under the License.
 *
 * @file bdev.cpp
 */

#include <string>
#include "spdk/model/bdev.hpp"



using namespace spdk;

constexpr const char model::Bdev::NAME[];
constexpr const char model::Bdev::UUID[];
constexpr const char model::Bdev::PRODUCT_NAME[];
constexpr const char model::Bdev::BDEV_BLOCK_SIZE[];
constexpr const char model::Bdev::ALIASES[];
constexpr const char model::Bdev::NUM_BLOCKS[];
constexpr const char model::Bdev::CLAIMED[];
constexpr const char model::Bdev::DRIVER_SPECIFIC[];
constexpr const char model::Bdev::SUPPORTED_IO_TYPES[];
constexpr const char model::Bdev::QOS_IOS_PER_SEC[];
constexpr const char model::Bdev::ASSIGNED_RATE_LIMITS[];

constexpr const char  model::BdevProductName::LOGICAL_VOLUME[];
constexpr const char  model::BdevProductName::MALLOC_DISK[];
constexpr const char  model::BdevProductName::NVMe_DISK[];

constexpr const char  model::ClearMethod::WRITE_ZEROES[];
constexpr const char  model::ClearMethod::NONE[];
constexpr const char  model::ClearMethod::UNMAP[];



void model::from_json(const json::Json& json, model::Bdev& bdev) {

    bdev.set_name(json.at(model::Bdev::NAME).get<std::string>());
    bdev.set_uuid(json.value(model::Bdev::UUID, json::Json{}).get<JsonOptional<std::string>>());
    bdev.set_product_name(json.at(model::Bdev::PRODUCT_NAME).get<std::string>());
    bdev.set_aliases(json.at(model::Bdev::ALIASES).get<std::vector<std::string>>());
    bdev.set_block_size(json.at(model::Bdev::BDEV_BLOCK_SIZE).get<std::uint32_t>());
    bdev.set_num_blocks(json.at(model::Bdev::NUM_BLOCKS).get<std::uint64_t>());
    bdev.set_claimed(json.at(model::Bdev::CLAIMED).get<bool>());
    bdev.set_driver_specific(json.at(model::Bdev::DRIVER_SPECIFIC));
    bdev.set_supported_io_types(json.at(model::Bdev::SUPPORTED_IO_TYPES).get<SupportedIoTypes>());
    bdev.set_assigned_rate_limits(json.value(model::Bdev::ASSIGNED_RATE_LIMITS, json::Json{}).get<JsonOptional<AssignedRateLimits>>());

    if (json.count(model::Bdev::QOS_IOS_PER_SEC)) {
        bdev.set_qos_ios_per_sec(json.at(model::Bdev::QOS_IOS_PER_SEC).get<std::uint64_t>());
    }
}


void model::to_json(json::Json& json, const model::Bdev& bdev) {

    json = json::Json{
        {model::Bdev::NAME,               bdev.get_name()},
        {model::Bdev::UUID,               bdev.get_uuid()},
        {model::Bdev::PRODUCT_NAME,       bdev.get_product_name()},
        {model::Bdev::ALIASES,            bdev.get_aliases()},
        {model::Bdev::BDEV_BLOCK_SIZE,    bdev.get_block_size()},
        {model::Bdev::NUM_BLOCKS,         bdev.get_num_blocks()},
        {model::Bdev::CLAIMED,            bdev.is_claimed()},
        {model::Bdev::DRIVER_SPECIFIC,    bdev.get_driver_specific()},
        {model::Bdev::SUPPORTED_IO_TYPES, bdev.get_supported_io_types()},
        {model::Bdev::ASSIGNED_RATE_LIMITS, bdev.get_assigned_rate_limits()}};

    if (bdev.get_qos_ios_per_sec()) {
        json[model::Bdev::QOS_IOS_PER_SEC] = bdev.get_qos_ios_per_sec();
    }
    else {
        json[model::Bdev::QOS_IOS_PER_SEC] = NULL;
    }

}


std::string model::Bdev::to_string() const {

    std::stringstream ss{};
    ss << "==========Bdev:============= " << std::endl;
    ss << "Name: " << get_name() << std::endl;
    if (get_uuid()) {
        ss << "UUID: " << get_uuid().value() << std::endl;
    }
    ss << "Product name: " << get_product_name() << std::endl;
    if (!get_aliases().empty()) {
        ss << "Aliases: " << std::endl;
        for (const auto& alias : get_aliases()) {
            ss << " " << alias << std::endl;
        }
    }
    ss << "Block size: " << get_block_size() << std::endl;
    ss << "Num blocks: " << get_num_blocks() << std::endl;
    ss << "Claimed: " << std::boolalpha << is_claimed() << std::endl;
    ss << "Driver specific: " << std::endl << get_driver_specific().dump(2) << std::endl;
    ss << "Supported IO types: " << std::endl << get_supported_io_types().to_string() << std::endl;
    if (m_qos_ios_per_sec) {
        ss << "QOS IOs per Seconds: " << get_qos_ios_per_sec().value() << std::endl;
    }
    if (m_assigned_rate_limits) {
        ss << "Assigned rate limits: " << std::endl << get_assigned_rate_limits().value().to_string() << std::endl;
    }
    ss << "============================ " << std::endl;
    return ss.str();
}



