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

cmake_minimum_required(VERSION 3.4)
project("NVME" CXX)

add_library(nvme STATIC
    src/commands/generic_nvme_command.cpp
    src/commands/controller_reset.cpp
    src/commands/flush.cpp
    src/commands/format_nvm.cpp
    src/commands/identify.cpp
    src/commands/namespace_management.cpp
    src/commands/namespace_attachment.cpp
    src/commands/get_log_page.cpp
    src/commands/get_features.cpp
    src/commands/set_features.cpp

    src/utils.cpp
    src/abstract_nvme_invoker.cpp
    src/abstract_nvme_interface.cpp
    src/nvme_invoker.cpp
    src/nvme_interface.cpp
    src/nvme_exception.cpp
)

target_include_directories(nvme
    PUBLIC
    include
)

add_subdirectory(tests)