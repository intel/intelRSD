/*!
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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

#include "status/state_machine_action.hpp"
#include "discovery/discovery_manager.hpp"
#include "tree_stability/storage_tree_stabilizer.hpp"
#include "agent-framework/state_machine/state_machine_exception.hpp"

using namespace agent::storage;
using namespace agent::storage::discovery;
using namespace agent_framework::state_machine;

namespace {
    bool is_enabled(const enums::State state) {
        return enums::State::ENABLED == state;
    }
}

StateMachineAction::StateMachineAction(DiscoveryManager& discover_manger) :
    m_discovery_manager{discover_manger} { }

StateMachineAction::~StateMachineAction() { }

void StateMachineAction::execute(StateThreadEntrySharedPtr entry) {
    auto state = entry->get_state();
    auto module = entry->get_module();

    if (is_enabled(state)) {
        try {
            /* start discovery manager */
            m_discovery_manager.discovery(module);
            module = StorageTreeStabilizer().stabilize(module);
            entry->update_module(module);
        }
        catch(const std::exception& e) {
            throw StateMachineError(e.what());
        }
    }
}


