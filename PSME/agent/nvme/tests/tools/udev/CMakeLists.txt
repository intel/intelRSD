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
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES GNU)
    set_source_files_properties(udev_stub.cpp
        PROPERTIES COMPILE_FLAGS "-Wno-strict-aliasing"
    )
endif()

add_gtest(tools_udev_test psme-gpt-nvme
    udev_stub.cpp
    udev_test.cpp
    test_runner.cpp
)

target_sources(${test_target} PRIVATE
    ../../../src/tools/udev.cpp
)


target_include_directories(${test_target}
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../../include
)