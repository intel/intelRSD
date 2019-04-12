# <license_header>
#
# Copyright (c) 2016-2019 Intel Corporation
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

add_gtest(ipmb psme-chassis
    network_utils_test.cpp
    ipmb_certificate_test.cpp
    ipmb_ipmi_message_test.cpp
    test_runner.cpp
)

add_library(chassis_ipmb_objects_test
    $<TARGET_OBJECTS:ipmb-service>
)

target_link_libraries(${test_target}
    chassis_ipmb_objects_test
    agent-framework
    database
    ${LOGGER_LIBRARIES}
    ${UUID_LIBRARIES}
    ${SAFESTRING_LIBRARIES}
    configuration
    md5
)
