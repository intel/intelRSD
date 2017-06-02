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

set (SOURCES main.cpp)

add_library(iscsi-tgt-module $<TARGET_OBJECTS:iscsi-tgt-ref>)

target_link_libraries(iscsi-tgt-module ${LOGGER_LIBRARIES} ${SAFESTRING_LIBRARIES} ${AGENT_FRAMEWORK_LIBRARIES})

add_executable(iscsi-tgt-example-ref ${SOURCES})

target_link_libraries(iscsi-tgt-example-ref
    ${LOGGER_LIBRARIES}
    ${SAFESTRING_LIBRARIES}
    iscsi-tgt-module
    ${NET_LIBRARIES}
    ${CONFIGURATION_LIBRARIES}
    ${JSONCXX_LIBRARIES}
)
