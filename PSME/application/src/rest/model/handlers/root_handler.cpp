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
 * */

#include "psme/rest/model/handlers/root_handler.hpp"

namespace psme {
namespace rest {
namespace model {
namespace handler {
RootHandler::~RootHandler() {}
}
}
}
}
namespace agent_framework {
namespace model {
FakeRoot::~FakeRoot() {}
namespace requests {
FakeRootInfo::~FakeRootInfo() {}
}
}
namespace module {

using FakeRootManager = agent_framework::module::GenericManager<agent_framework::model::FakeRoot>;

/*!
 * @brief Specialization of get_manager template function
 *
 * Will not be executed!
 */
template<>
FakeRootManager& get_manager<agent_framework::model::FakeRoot>() {
    static FakeRootManager manager{};
    return manager;
}

}
}
