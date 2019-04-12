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
 * @file fru_eeprom.cpp
 * @brief FRU Eeeprom access implementation
 * */

#include "fru_eeprom.hpp"

using namespace agent::pnc;
using namespace agent::pnc::gas;
using namespace agent::pnc::i2c;

FruEeprom::~FruEeprom(){}

void FruEeprom::read(const I2cAccessInterfacePtr i2c) {

    i2c->read(static_cast<uint8_t>(PM85X6TwiPort::PORT9),
              static_cast<uint16_t>(PM85X6TwiDeviceAddress::MF3_FRU_EEPROM),
              0,
              reinterpret_cast<uint8_t*>(&fields),
              FRU_EEPROM_SIZE,
              true);
}
