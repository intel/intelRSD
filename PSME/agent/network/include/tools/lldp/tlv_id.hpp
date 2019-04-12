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
 * @file tlv_id.hpp
 *
 * @brief LLDP TLV ID declaration
 * */

#pragma once

#include <cstdint>

namespace agent {
namespace network {
namespace api {
namespace lldp {

/*!
 * @enum TlvId
 * @brief LLDP TLV ID
 *
 * LLDP TLV ids are defined based on 802.1AB-2009 spec
 *
 * @var TlvId::END_OF_LLDPDU
 * The End Of LLDPDU TLV
 *
 * @var TlvId::PORT_DESCRIPTION
 * The Port Description TLV
 *
 * @var TlvId::UNKNOWN
 * Unknown TLV
 *
 * */
enum class TlvId : uint32_t {
    END_OF_LLDPDU = 0,
    PORT_DESCRIPTION = 4,
    UNKNOWN = 127
};

}
}
}
}
