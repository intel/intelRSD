/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2016-2019 Intel Corporation
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
 *
 * @file chassis_builder_mf3.cpp
 *
 * @brief ChassisBuilder implementation for MF3
 * */

#include "discovery/builders/chassis_builder_mf3.hpp"
#include "discovery/discovery_utils.hpp"
#include "fru_eeprom/fru_eeprom_parser.hpp"

using namespace agent::pnc;
using namespace agent::pnc::discovery::builders;
using namespace agent_framework::model;
using namespace agent::pnc::discovery::utils;
using namespace fru_eeprom::parser;

void ChassisBuilderMf3::build_default() {
}

ChassisBuilderMf3::ReturnType ChassisBuilderMf3::update_fru(const FruEeprom& fru_eeprom) {
    FruEepromParser parser(fru_eeprom.fields.data, sizeof(fru_eeprom.fields.data));
    attribute::FruInfo fru_info{};
    try {
        parser.parse();

        auto chassis_info_area = parser.get_chassis_info();
        m_obj.set_type(fru_chassis_type_to_model_chassis_type(chassis_info_area.type));

        auto board_area = parser.get_board();
        fru_info.set_manufacturer(get_null_terminated_optional_string(
                reinterpret_cast<uint8_t*>(board_area.manufacturer.data), board_area.manufacturer.data_len));
        fru_info.set_serial_number(get_null_terminated_optional_string(
                reinterpret_cast<uint8_t*>(board_area.serial_number.data), board_area.serial_number.data_len));
        fru_info.set_part_number(get_null_terminated_optional_string(
                reinterpret_cast<uint8_t*>(board_area.part_number.data), board_area.part_number.data_len));
        fru_info.set_model_number(get_null_terminated_optional_string(
                reinterpret_cast<uint8_t*>(board_area.product_name.data), board_area.product_name.data_len));
        m_obj.set_fru_info(fru_info);

        auto product_info_area = parser.get_product_info();
        m_obj.set_asset_tag(get_null_terminated_optional_string(
                reinterpret_cast<uint8_t*>(product_info_area.asset_tag.data), product_info_area.asset_tag.data_len));

    }
    catch (const std::exception& e) {
        log_error("pnc-discovery", "FRU EEPROM parsing error: " << e.what());
    }

    return *this;
}

