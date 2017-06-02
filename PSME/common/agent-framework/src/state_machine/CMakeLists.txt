# <license_header>
#
# Copyright (c) 2015-2017 Intel Corporation
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# </license_header>

set(SOURCES
    module_state.cpp
    module_state_offline.cpp
    module_state_absent.cpp
    module_state_unknown.cpp
    module_state_enabled.cpp
    module_state_starting.cpp
    state_machine.cpp
    state_machine_exception.cpp
    state_machine_module_thread.cpp
    state_machine_thread.cpp
    state_machine_thread_action.cpp
    state_thread_entry.cpp
    state_machine_transition.cpp
)

include_directories(
    ${AGENT_FRAMEWORK_DIR}/src/status
    ${AGENT_FRAMEWORK_DIR}/src/eventing
    ${AGENT_FRAMEWORK_DIR}/src/state_machine
)

add_library(state_machine OBJECT ${SOURCES})
