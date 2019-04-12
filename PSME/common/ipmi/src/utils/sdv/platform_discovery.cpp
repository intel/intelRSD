/*!
 * @brief Implementation of function that discovers BMC interface with particular platform support.
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
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file utils/sdv/platform_discovery.cpp
 */

#include <cstdint>

#include "ipmi/command/generic/get_device_id.hpp"
#include "ipmi/utils/sdv/platform_discovery.hpp"


using namespace ipmi;
using namespace ipmi::sdv;
using namespace ipmi::command;


ipmi::command::generic::BmcInterface ipmi::sdv::platform_discovery(const ipmi::command::generic::response::GetDeviceId& rsp) {
    auto product_id = rsp.get_product_id();
    auto manufacturer_id = rsp.get_manufacturer_id();

    if (ipmi::command::generic::ProductId::PRODUCT_ID_INTEL_XEON_BDC_R == product_id) {
        return ipmi::command::generic::BmcInterface::RSA_1_2;
    }
    else if (ipmi::command::generic::ProductId::PRODUCT_ID_INTEL_XEON_PURLEY == product_id) {
        if (ipmi::command::generic::ManufacturerId::MANUFACTURER_ID_QUANTA == manufacturer_id) {
            return ipmi::command::generic::BmcInterface::RSD_2_2;
        }
        else if (ipmi::command::generic::ManufacturerId::MANUFACTURER_ID_INTEL == manufacturer_id) {
            return ipmi::command::generic::BmcInterface::RSD_2_4;
        }
    }
    return ipmi::command::generic::BmcInterface::UNKNOWN;
}
