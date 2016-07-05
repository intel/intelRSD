/*!
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
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
 * @file agent-framework/module/chassis_zone.hpp
 * @brief Chassis zone
 * */

#ifndef AGENT_FRAMEWORK_MODULE_CHASSIS_ZONE_HPP
#define AGENT_FRAMEWORK_MODULE_CHASSIS_ZONE_HPP

#include "agent-framework/module/resources.hpp"

/*! Psme namespace */
namespace agent_framework {
/*! Generic namespace */
namespace generic {

/*! Chassis zone class */
class ChassisZone : public Resources {
public:

    /*! Default constructor */
    ChassisZone() {}

    /*! Copy constructor */
    ChassisZone(const ChassisZone&) = default;

    /*! Assignment operator */
    ChassisZone& operator=(const ChassisZone&) = default;

    virtual ~ChassisZone();
};

}
}
#endif /* AGENT_FRAMEWORK_MODULE_CHASSIS_ZONE_HPP */
