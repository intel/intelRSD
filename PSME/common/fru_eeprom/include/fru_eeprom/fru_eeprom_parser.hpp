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
 * @file fru_eeprom_parser.hpp
 *
 * @brief FRU EEPROM Parser
 * */

#pragma once

#include "fru_eeprom/common_header.hpp"
#include "fru_eeprom/chassis_info_area.hpp"
#include "fru_eeprom/board_area.hpp"
#include "fru_eeprom/product_info_area.hpp"

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace fru_eeprom {
namespace parser {

/*!
 * Class is a parser of Platform Management FRU information stored in plaform EEPROM device.
 */
class FruEepromParser {
public:
    /*!
    * @brief FruEepromException type that will be thrown when error condition is
    * discovered while parsing EEPROM blob
    */
    class FruEepromException : public std::runtime_error {
    public:
        /*!
         * @brief constructs exception object
         * @param what_arg Describes what hapened
         */
        explicit FruEepromException(const std::string& what_arg) : std::runtime_error("FRU EEPROM parser - " + what_arg) { }

        FruEepromException(const FruEepromException &) = default;
        FruEepromException& operator=(const FruEepromException&) = default;
        FruEepromException(FruEepromException&&) = default;
        FruEepromException& operator=(FruEepromException&&) = default;

        virtual ~FruEepromException();
    };

    /*!
     * FRU EEPROM Parser Class constructor
     * @param buffer EEPROM data to be parsed
     */
    FruEepromParser(const uint8_t* buffer, const size_t size);

    FruEepromParser(const FruEepromParser&) = default;

    /*!
     * Common Header Area getter
     * @return Returns Common Header Area structure
     */
    const CommonHeaderArea& get_common_header() const {
        return  m_common_header;
    }

    /*!
     * Chassis Info Area getter
     * @return Returns Chassis Info Area structure
     */
    const ChassisInfoArea& get_chassis_info() const {
        return m_chassis_info;
    }

    /*!
     * Board Area getter
     * @return Returns Board Area structure
     */
    const BoardArea& get_board() const {
        return m_board;
    }

    /*!
     * Product Info Area getter
     * @return Returns Product Info Area structure
     */
    const ProductInfoArea& get_product_info() const {
        return m_product_info;
    }

    /*!
     * Main method of the class. Parses EEPROM content and stores output data in
     */
    void parse();

    virtual ~FruEepromParser();

private:
    std::vector<uint8_t> m_buffer{};

    CommonHeaderArea m_common_header{};
    ChassisInfoArea m_chassis_info{};
    BoardArea m_board{};
    ProductInfoArea m_product_info{};

    void init_buffer(const uint8_t* in_buffer, const size_t size);

    void parse_common_header_area();
    void parse_internal_area();
    void parse_chassis_info_area();
    void parse_board_area();
    void parse_product_info_area();
    void parse_multirecord_area();
    void parse_pad_area();

    template<typename T>
    void parse_area_header(T& area, uint8_t offset);

    bool is_area_format_valid(const uint8_t format);

    uint8_t read_field(Field& field, const uint8_t area, const uint8_t offset);
    uint8_t read_byte(const uint8_t area, const uint8_t offset);

    void check_crc(const uint8_t start, const size_t size);


};

}
}
