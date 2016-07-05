# <license_header>
#
# Copyright (c) 2015-2016 Intel Corporation
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

add_gtest(module-ref agent-framework
    test_runner.cpp
    generic_manager_test.cpp
    optional_values_test.cpp
)

set_source_files_properties(
    optional_values_test.cpp
    COMPILE_FLAGS "-Wno-unused-member-function"
)

set_source_files_properties(
    generic_manager_test.cpp
    COMPILE_FLAGS "-Wno-exit-time-destructors -Wno-global-constructors"
)

if (CMAKE_CXX_COMPILER_ID MATCHES GNU)
    set_source_files_properties(
        generic_manager_test.cpp
        PROPERTIES COMPILE_FLAGS "-Wno-unsafe-loop-optimizations"
    )
endif()

target_link_libraries(${test_target}
    ${UUID_LIBRARIES}
    ${AGENT_FRAMEWORK_LIB}
    ${SAFESTRING_LIBRARIES}
    ${JSONCPP_LIBRARIES}
    ${JSONCXX_LIBRARIES}
)
