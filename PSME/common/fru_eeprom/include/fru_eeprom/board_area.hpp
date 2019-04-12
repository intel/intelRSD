/*!
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
 *
 * @file board_area.hpp
 *
 * @brief Board Area format
 * */

#pragma once

#include "fru_eeprom/utils.hpp"

#include <cstdint>
#include <string>

namespace fru_eeprom {
namespace parser {

struct BoardArea {
    uint8_t format{};
    uint8_t length{};
    uint8_t code{};
    uint8_t data_time[3]{};
    Field manufacturer{};
    Field product_name{};
    Field serial_number{};
    Field part_number{};
    Field fru_file_id{};
    uint8_t crc{};
};

}
}
