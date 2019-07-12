# <license_header>
#
# Copyright (c) 2018-2019 Intel Corporation
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

add_library(fpgaof-loader STATIC
    src/fpgaof_loader.cpp
    src/fpgaof_configuration.cpp
    )

target_link_libraries(fpgaof-loader
    PUBLIC
    agent-framework
    json
    opaepp
    PRIVATE
    fpgaof-utils
    )

target_include_directories(fpgaof-loader
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    )