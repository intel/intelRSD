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
 *
 *
 * @file boot_options.hpp
 *
 * @brief Constants used by IPMI Get and Set Chassis Boot Options Commands.
 * */

#pragma once

namespace ipmi {
namespace command {
namespace generic {

static constexpr std::uint8_t MASK_VALID = 0x80;
static constexpr std::uint8_t MASK_PERSISTENT = 0x40;
static constexpr std::uint8_t MASK_BOOT_MODE = 0x20;

static constexpr std::uint8_t MASK_DEVICE = 0x3C;
static constexpr std::uint8_t DEVICE_NONE = 0x00;
static constexpr std::uint8_t DEVICE_PXE = 0x04;
static constexpr std::uint8_t DEVICE_HDD = 0x08;
static constexpr std::uint8_t DEVICE_REMOTE_HDD = 0x2C;

}
}
}
