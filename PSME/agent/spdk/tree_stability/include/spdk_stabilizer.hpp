/*!
 * @brief Provides class for stabilizing storage agent resources
 *
 * @copyright
 * Copyright (c) 2018-2019 Intel Corporation
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
 * @file spdk_stabilizer.hpp
 * */

#pragma once



#include "agent-framework/tree_stability/stabilizer.hpp"
#include "spdk_key_generator.hpp"

#include <string>



namespace agent {
namespace spdk {

/*!
 * @brief Tree stability managing class for spdk agent.
 * */
using SpdkStabilizer = agent_framework::Stabilizer<agent::spdk::SpdkKeyGenerator>;

}
}
