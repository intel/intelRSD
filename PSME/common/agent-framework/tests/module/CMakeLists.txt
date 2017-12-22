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

if (NOT GTEST_FOUND)
    return()
endif()

add_gtest(module agent-framework
    test_runner.cpp
    generic_manager_test.cpp
    json_transformations_test.cpp
    optional_values_test.cpp
    persistent_uuid_generation_test.cpp
    obj_reference_test.cpp
    many_to_many_manager_test.cpp
    task_test.cpp
    enum_builder_test.cpp
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

if (CMAKE_CXX_COMPILER_ID MATCHES Clang)
    set_source_files_properties(
        json_transformations_test.cpp
        COMPILE_FLAGS "-Wno-missing-variable-declarations -Wno-missing-prototypes"
    )
endif()

target_link_libraries(${test_target}
    ${AGENT_FRAMEWORK_LIB}
    ${UUID_LIBRARIES}
    ${CONFIGURATION_LIBRARIES}
    ${SAFESTRING_LIBRARIES}
    ${JSONCXX_LIBRARIES}
)
