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

if (NOT (GTEST_FOUND OR GMOCK_FOUND))
    return()
endif()

add_library(agent-mocks STATIC
    mock_logger_ext.cpp
)

target_include_directories(agent-mocks
    SYSTEM PUBLIC
    ${GTEST_INCLUDE_DIRS}
)

target_link_libraries(agent-mocks
    ${LOGGER_LIBRARIES}
    ${GTEST_LIBRARIES}
)

if (CMAKE_CXX_COMPILER_ID MATCHES GNU)
    set_target_properties(agent-mocks PROPERTIES
        COMPILE_FLAGS "-Wno-useless-cast -Wno-effc++ -Wno-inline"
    )
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES Clang)
        set_target_properties(agent-mocks PROPERTIES
        COMPILE_FLAGS "-Wno-global-constructors"
    )
endif()
