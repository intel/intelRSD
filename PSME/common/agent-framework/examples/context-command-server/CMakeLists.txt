# <license_header>
#
# Copyright (c) 2017-2019 Intel Corporation
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

add_executable(context-command-server-example context_command_server_example.cpp)

target_link_libraries(context-command-server-example
    agent-framework
    ${JSON_LIBRARIES}
    ${JSON_RPC_LIBRARIES}
    ${LOGGER_LIBRARIES}
    ${SAFESTRING_LIBRARIES}
)
