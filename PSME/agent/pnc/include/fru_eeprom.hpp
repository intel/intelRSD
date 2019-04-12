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
 * @file fru_eeprom.hpp
 * @brief FRU EEPROM reading implementation
 * */

#pragma once

#include "i2c/i2c_access_interface.hpp"
#include "gas/utils.hpp"
#include "agent-framework/module/enum/chassis.hpp"

/*! Agent namespace */
namespace agent {
/*! PNC namespace */
namespace pnc {

class FruEeprom {
public:
    /*! Fru Eeeprom size in bytes */
    static constexpr uint8_t FRU_EEPROM_SIZE = 192;

    /*! Default constructor */
    FruEeprom() {}

    /*! Copy constructor */
    FruEeprom(const FruEeprom&) = default;

    /*! Assignment operator */
    FruEeprom& operator=(const FruEeprom&) = default;

#pragma pack(push, 1)
    /*! Fru Eeprom memory map */
    struct {
        uint8_t data[FRU_EEPROM_SIZE];
    } fields{};
#pragma pack(pop)

    /*!
     * @brief Reads FRU Eeprom data.
     * */
    void read(const i2c::I2cAccessInterfacePtr i2c);

    virtual ~FruEeprom();
};

}
}
