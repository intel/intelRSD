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
 * @file system_boot_options_thread.hpp
 *
 * @brief Declaration of System boot options thread class
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

/*!
 * @brief Reads systems' BootOptions
 *  i.e. active boot overrides using the IPMI getSystemBootOptions command.
 *  In case of any change, this function updates the model of the agent.
 */
void update_system_boot_options(agent::compute::Bmc&);

}
}
}
