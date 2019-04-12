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
 * @file storage.hpp
 * @brief string enums for model classes, see enum_builder.hpp for more info
 * */

#pragma once



#include "enum_builder.hpp"



namespace agent_framework {
namespace model {
namespace enums {

/*!
 * @brief ENUM TargetAuthenticationMethod for NetworkDeviceFunction class member
 */
ENUM(TargetAuthenticationMethod, uint32_t, OneWay, Mutual);


/*!
 * @brief ENUM VolumeInitializationType for Volume initialize action
 */
ENUM(VolumeInitializationType, uint32_t, Fast, Slow);

}
}
}
