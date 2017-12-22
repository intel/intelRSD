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

include_directories(
    ${GTEST_INCLUDE_DIRS}
    ${JSONPTR_INCLUDE_DIRS}
    ${JSONCXX_INCLUDE_DIRS}
)

add_gtest(test jsonptr
    test_runner.cpp
    jsonptr_test.cpp
)

if (CMAKE_CXX_COMPILER_ID MATCHES Clang)
    set_source_files_properties(
        jsonptr_test.cpp
        COMPILE_FLAGS "-Wno-missing-variable-declarations -Wno-missing-prototypes"
    )
elseif(CMAKE_CXX_COMPILER_ID MATCHES GNU)
    set_source_files_properties(
        jsonptr_test.cpp
        COMPILE_FLAGS "-Wno-missing-declarations"
    )
endif()

target_link_libraries(${test_target}
    ${JSONPTR_LIBRARIES}
    ${JSONCXX_LIBRARIES}
)

add_custom_target(unittest_jsonptr
    make
)

add_custom_target(unittest_jsonptr_run
    ctest --output-on-failure
)
