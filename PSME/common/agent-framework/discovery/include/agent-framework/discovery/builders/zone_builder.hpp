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
 * @file zone_builder.hpp
 *
 * @brief ZoneBuilder interface
 * */

#pragma once

#include "agent-framework/module/model/zone.hpp"

namespace agent_framework {
namespace discovery {

class ZoneBuilder {
public:

    /*!
     * @brief Build default zone object.
     * @param[in] parent_uuid UUID of the parent component.
     * @return Default zone object.
     */
    static agent_framework::model::Zone build_default(const Uuid& parent_uuid);
};

}
}
