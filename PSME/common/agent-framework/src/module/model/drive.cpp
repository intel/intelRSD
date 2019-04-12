/*!
 * @copyright
 * Copyright (c) 2015-2019 Intel Corporation
 *
 * @copyright
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * @copyright
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * @copyright
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * */

#include "agent-framework/module/model/drive.hpp"
#include "agent-framework/module/constants/compute.hpp"



using namespace agent_framework::model;
using namespace agent_framework::model::utils;

const enums::Component Drive::component = enums::Component::Drive;
const enums::CollectionName Drive::collection_name = enums::CollectionName::Drives;


Drive::Drive(const std::string& parent_uuid, enums::Component parent_type) :
    Resource{parent_uuid, parent_type} {}


Drive::~Drive() {}


json::Json Drive::to_json() const {
    json::Json result = json::Json();

    result[literals::Drive::NAME] = get_name();
    result[literals::Drive::DESCRIPTION] = get_description();
    result[literals::Drive::STATUS] = get_status().to_json();
    result[literals::Drive::INTERFACE] = get_interface();
    result[literals::Drive::TYPE] = get_type();
    result[literals::Drive::RPM] = get_rpm();
    result[literals::Drive::FIRMWARE_VERSION] = get_firmware_version();
    result[literals::Drive::LATENCY_TRACKING_ENABLED] = get_latency_tracking_enabled();
    result[literals::Drive::CAPACITY] = get_capacity_gb();
    result[literals::Drive::FRU_INFO] = get_fru_info().to_json();
    result[literals::Drive::INDICATOR_LED] = get_indicator_led();
    result[literals::Drive::ASSET_TAG] = get_asset_tag();
    result[literals::Drive::CAPABLE_SPEED_GBS] = get_capable_speed_gbs();
    result[literals::Drive::NEGOTIATED_SPEED_GBS] = get_negotiated_speed_gbs();
    result[literals::Drive::LOCATION] = get_locations().to_json();
    result[literals::Drive::STATUS_INDICATOR] = get_status_indicator();
    result[literals::Drive::REVISION] = get_revision();
    result[literals::Drive::FAILURE_PREDICTED] = get_failure_predicted();
    result[literals::Drive::SKU] = get_sku();
    result[literals::Drive::IDENTIFIERS] = get_identifiers().to_json();
    result[literals::Drive::HOTSPARE_TYPE] = get_hotspare_type();
    result[literals::Drive::ENCRYPTION_ABILITY] = get_encryption_ability();
    result[literals::Drive::ENCRYPTION_STATUS] = get_encryption_status();
    result[literals::Drive::BLOCK_SIZE_BYTES] = get_block_size_bytes();
    result[literals::Drive::PREDICTED_MEDIA_LIFE_LEFT] = get_predicted_media_life_left();
    result[literals::Drive::ERASED] = get_erased();
    result[literals::Drive::COLLECTIONS] = get_collections().to_json();
    result[literals::Drive::OEM] = get_oem().to_json();

    return result;
}


Drive Drive::from_json(const json::Json& json) {
    Drive drive{};

    drive.set_name(json[literals::Drive::NAME]);
    drive.set_description(json[literals::Drive::DESCRIPTION]);
    drive.set_status(attribute::Status::from_json(json[literals::Drive::STATUS]));
    drive.set_interface(json[literals::Drive::INTERFACE]);
    drive.set_type(json[literals::Drive::TYPE]);
    drive.set_firmware_version(json[literals::Drive::FIRMWARE_VERSION]);
    drive.set_latency_tracking_enabled(json[literals::Drive::LATENCY_TRACKING_ENABLED]);
    drive.set_rpm(json[literals::Drive::RPM]);
    drive.set_capacity_gb(json[literals::Drive::CAPACITY]);
    drive.set_fru_info(attribute::FruInfo::from_json(json[literals::Drive::FRU_INFO]));
    drive.set_indicator_led(json[literals::Drive::INDICATOR_LED]);
    drive.set_asset_tag(json[literals::Drive::ASSET_TAG]);
    drive.set_capable_speed_gbs(json[literals::Drive::CAPABLE_SPEED_GBS]);
    drive.set_negotiated_speed_gbs(json[literals::Drive::NEGOTIATED_SPEED_GBS]);
    drive.set_locations(Locations::from_json(json[literals::Drive::LOCATION]));
    drive.set_status_indicator(json[literals::Drive::STATUS_INDICATOR]);
    drive.set_revision(json[literals::Drive::REVISION]);
    drive.set_failure_predicted(json[literals::Drive::FAILURE_PREDICTED]);
    drive.set_sku(json[literals::Drive::SKU]);
    drive.set_identifiers(Identifiers::from_json(json[literals::Drive::IDENTIFIERS]));
    drive.set_hotspare_type(json[literals::Drive::HOTSPARE_TYPE]);
    drive.set_encryption_ability(json[literals::Drive::ENCRYPTION_ABILITY]);
    drive.set_encryption_status(json[literals::Drive::ENCRYPTION_STATUS]);
    drive.set_block_size_bytes(json[literals::Drive::BLOCK_SIZE_BYTES]);
    drive.set_predicted_media_life_left(json[literals::Drive::PREDICTED_MEDIA_LIFE_LEFT]);
    drive.set_erased(json[literals::Drive::ERASED]);
    drive.set_collections(Collections::from_json(json[literals::Drive::COLLECTIONS]));
    drive.set_oem(attribute::Oem::from_json(json[literals::Drive::OEM]));

    return drive;
}
