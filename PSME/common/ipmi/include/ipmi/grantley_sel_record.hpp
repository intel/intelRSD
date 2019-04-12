/*!
 * @brief Events defined for Grantley platform
 *
 * This is only definition class, not to be instantiated.
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
 * @file ipmi/grantley_sel_record.hpp
 */

#pragma once

#include "ipmi/sel_record.hpp"

namespace ipmi {


class GrantleySelRecord : public SelRecordGeneric {
public:
    /*!
     * @brief Events defined in F06_BMC_FW_Spec_rev021
     * @{
     */
    #ifdef __clang__
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
    #endif

    #define SS(e, t)      event_sensor_type(e##_EVENT_TYPE_CODE, ipmi::command::generic::SensorType::t)
    #define SE(e, t, ...) event_sensor_type(e##_EVENT_TYPE_CODE, ipmi::command::generic::SensorType::t, ##__VA_ARGS__)

    /* TODO define all necessary events on the spec if requested */

    #ifdef __clang__
    #pragma clang diagnostic pop
    #endif


    #undef SE
    #undef SS
    /*! @} */
};


}
