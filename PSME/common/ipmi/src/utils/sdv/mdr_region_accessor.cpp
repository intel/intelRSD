/*!
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
 * @brief
 *
 * @file mdr_region_accessor.cpp
 */

#include "ipmi/utils/sdv/mdr_region_accessor.hpp"
#include "ipmi/utils/sdv/grantley_mdr_region_accessor.hpp"
#include "ipmi/utils/sdv/purley_mdr_region_accessor.hpp"
#include "ipmi/command/generic/enums.hpp"

using namespace ipmi::sdv;

MdrRegionAccessor::~MdrRegionAccessor() {}

MdrRegionAccessor::Ptr MdrRegionAccessorFactory::create(std::uint32_t platform_id,
                                                        IpmiController& ipmi_controller,
                                                        const command::sdv::DataRegionId region_id) const {
    if (platform_id == ipmi::command::generic::ProductId::PRODUCT_ID_INTEL_XEON_PURLEY) {
        return MdrRegionAccessor::Ptr(new PurleyMdrRegionAccessor(ipmi_controller, region_id));
    }
    else if (platform_id == ipmi::command::generic::ProductId::PRODUCT_ID_INTEL_XEON_BDC_R) {
        return MdrRegionAccessor::Ptr(new GrantleyMdrRegionAccessor(ipmi_controller, region_id));
    }
    throw std::runtime_error("Unsupported platform");
}


MdrRegionAccessorFactory::~MdrRegionAccessorFactory() {}
