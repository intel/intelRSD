# <license_header>
#
# Copyright (c) 2017 Intel Corporation
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

add_gtest(endpoints psme-rest-server
    test_runner.cpp

    chassis_location.cpp
)

target_link_libraries(${test_target}
    application-rest
    application-core
    ${AGENT_FRAMEWORK_LIBRARIES}
    ${CONFIGURATION_LIBRARIES}
    ${JSONCXX_LIBRARIES}
    ${JSONPTR_LIBRARIES}
    ${CURL_LIBRARIES}
    ${MICROHTTPD_LIBRARIES}
    ${SSDP_LIBRARIES}
    ${LOGGER_LIBRARIES}
    ${DATABASE_LIBRARIES}
    ${SAFESTRING_LIBRARIES}
    ${UUID_LIBRARIES}
    ${MD5_LIBRARIES}
)
