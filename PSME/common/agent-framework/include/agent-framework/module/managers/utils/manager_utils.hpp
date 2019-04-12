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
 * @file manager_utils.hpp
 * @brief Utilities used by managers.
 * */

#pragma once

#include "agent-framework/module/managers/generic_manager.hpp"
#include "agent-framework/module/managers/many_to_many_manager.hpp"

namespace agent_framework {
namespace module {

/*!
 * @brief Get manager.
 * @return Reference to an instance of a manager.
 * */
template<typename T, typename U = GenericManager<T>>
auto get_manager() -> U&;

/*!
 * @brief Get weak relation manager.
 * @return Reference to an instance of a manager.
 * */
template<typename PARENT, typename CHILD>
managers::ManyToManyManager& get_m2m_manager();

}
}
