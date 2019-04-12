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

if (NOT GTEST_FOUND)
    return()
endif ()


add_gtest(parser iscsi_mdr
    test_runner.cpp
    parser_test.cpp
    builder_test.cpp
    test_fixture.hpp
    builder_fixture.hpp
    )

target_link_libraries(${test_target}
    iscsi_mdr
    ipmi
    agent-framework
    ${SAFESTRING_LIBRARIES}
    )

target_include_directories(${test_target}
    PRIVATE
    ${AGENT_FRAMEWORK_INCLUDE_DIRS}
    )

add_custom_target(unittest_iscsi_mdr
    make
    )
add_custom_target(unittest_iscsi_mdr_run
    ctest --output-on-failure
    )
