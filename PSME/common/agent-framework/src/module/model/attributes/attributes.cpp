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
 * */

#include "agent-framework/module/model/attributes/attributes.hpp"
#include <algorithm>

using namespace agent_framework::model::attribute;

Attributes::Attributes() { }

Attributes::~Attributes() { }

bool Attributes::contains(const std::string& name) const {
    const auto& names = get_names();
    return std::find(names.cbegin(), names.cend(), name) != names.cend();
}
