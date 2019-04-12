/*!
 * @copyright
 * Copyright (c) 2017-2019 Intel Corporation
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
 * @file system_power_state.hpp
 *
 * @brief Declaration of update_system_power_state function
 * */

#pragma once

/*! Agent namespace */
namespace agent {
/*! Compute namespace */
namespace compute {

// forward declaration
class Bmc;

/*! Status namespace */
namespace status {


void update_system_power_state(agent::compute::Bmc&);

}
}
}
