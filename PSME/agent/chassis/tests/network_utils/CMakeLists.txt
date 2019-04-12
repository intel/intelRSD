# <license_header>
#
# Copyright (c) 2015-2019 Intel Corporation
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
if (NOT GTEST_FOUND)
    return()
endif()

add_gtest(addr_to_bytes psme-chassis
    test_runner.cpp
    addr_to_bytes.cpp
)

add_library(chassis_test_objects
    $<TARGET_OBJECTS:ipmb-service>
)
target_link_libraries(${test_target}
    chassis_test_objects
    agent-framework
    database
    ${LOGGER_LIBRARIES}
    ${UUID_LIBRARIES}
    ${SAFESTRING_LIBRARIES}
    configuration
)
