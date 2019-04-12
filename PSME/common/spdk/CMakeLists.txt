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


add_library(spdk STATIC
    src/spdk/model/bdev.cpp
    src/spdk/model/supported_io_types.cpp
    src/spdk/model/lvol_store.cpp
    src/spdk/spdk_api.cpp
    src/spdk/model/nvmf_subsystem.cpp
    src/spdk/model/listen_address.cpp
    src/spdk/model/host.cpp
    src/spdk/model/namespace.cpp
    src/spdk/model/controller_status.cpp
    src/spdk/model/namespace_data.cpp
    src/spdk/model/version.cpp
    src/spdk/model/controller_data.cpp
    src/spdk/model/optional_admin_command_support.cpp
    src/spdk/model/drivers/lvol_driver.cpp
    src/spdk/model/drivers/nvme_driver.cpp
    src/spdk/model/assigned_rate_limits.cpp)


target_include_directories(spdk
    PUBLIC
    include
)

target_link_libraries(spdk
    PUBLIC
    optional
    json-rpc

    PRIVATE
    logger
)

add_subdirectory(tests)
add_subdirectory(examples)