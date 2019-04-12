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

if (NOT GTEST_FOUND)
    return()
endif ()

add_gtest(test spdk
    test_runner.cpp
    get_bdevs_test.cpp
    get_lvol_stores_test.cpp
    construct_lvol_store_test.cpp
    destroy_lvs_test.cpp
    construct_lvol_bdev_test.cpp
    nvmf_subsystem_add_host_test.cpp
    nvmf_subsystem_add_listener_test.cpp
    nvmf_subsystem_add_ns_test.cpp
    destroy_lvol_bdev_test.cpp
    clone_lvol_bdev_test.cpp
    snapshot_lvol_bdev_test.cpp
    inflate_lvol_bdev_test.cpp
    resize_lvol_bdev_test.cpp
    nvmf_subsystem_remove_host_test.cpp
    nvmf_subsystem_create_test.cpp

    connectors/destroy_lvs_connector.cpp
    connectors/destroy_lvol_bdev_connector.cpp
    connectors/get_lvol_stores_connector.cpp
    connectors/construct_lvol_store_connector.cpp
    connectors/get_bdevs_connector.cpp
    connectors/construct_lvol_bdev_connector.cpp
    connectors/nvmf_subsytem_add_host_connector.cpp
    connectors/nvmf_subsytem_add_listener_connector.cpp
    connectors/nvmf_subsytem_add_ns_connector.cpp
    connectors/clone_lvol_bdev_connector.cpp
    connectors/snapshot_lvol_bdev_connector.cpp
    connectors/inflate_lvol_bdev_connector.cpp
    connectors/resize_lvol_bdev_connector.cpp
    connectors/nvmf_subsystem_remove_host_connector.cpp
    connectors/nvmf_subsystem_create_connector.cpp
    connectors/construct_nvmf_subsystem_connector.cpp
    )

set_source_files_properties(get_bdevs_test.cpp
    PROPERTIES COMPILE_FLAGS "-Wno-unsafe-loop-optimizations"
    )

set_source_files_properties(get_lvol_stores_test.cpp
    PROPERTIES COMPILE_FLAGS "-Wno-unsafe-loop-optimizations"
    )

target_link_libraries(${test_target}
    spdk
    )

add_custom_target(unittest_spdk
    make
    )
add_custom_target(unittest_spdk_run
    ctest --output-on-failure
    )