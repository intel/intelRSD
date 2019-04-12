/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2016-2019 Intel Corporation
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
 * @file chassis_builder_mf3.cpp
 *
 * @brief ChassisBuilder implementation for MF3
 * */

#include "discovery/builders/chassis_builder_mf3.hpp"
#include "discovery/discovery_utils.hpp"

#include <exception>

using namespace agent::pnc;
using namespace agent::pnc::discovery::builders;
using namespace agent_framework::model;
using namespace agent::pnc::discovery::utils;


void ChassisBuilder::build_default() {
}

[[ noreturn ]] ChassisBuilder::ReturnType ChassisBuilder::update_fru(const FruEeprom&) {
    // this is not implemented for MF3
    assert(false);
    // will be never thrown - needed for compilation
    throw std::runtime_error("Not implemented");
}
