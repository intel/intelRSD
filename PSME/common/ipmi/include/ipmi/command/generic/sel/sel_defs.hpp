/*!
 * @brief Typedefs used by SEL commands
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file sel_defs.hpp
 */

#pragma once

#include "ipmi/command/generic/enums.hpp"

#include <cstdint>

namespace ipmi {
namespace command {
namespace generic {

using SelRecordType = std::uint8_t;
using SelReservationId = std::uint16_t;
using SelEntryId = std::uint16_t;

/*! SEL timestamp, number of seconds since SEL device initialization */
using SelTimestamp = std::uint32_t;

/*! Free space in the SEL, in bytes */
using SelFreeSpace = std::uint16_t;

/*! Number of entries in the SEL */
using SelEntriesCount = std::uint16_t;

}
}
}
