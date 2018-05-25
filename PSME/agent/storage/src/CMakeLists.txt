# <license_header>
#
# Copyright (c) 2015-2018 Intel Corporation
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
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(command)
add_subdirectory(discovery)
add_subdirectory(sysfs)
add_subdirectory(iscsi)
add_subdirectory(lvm)
add_subdirectory(watcher)
add_subdirectory(loader)
add_subdirectory(common)
add_subdirectory(tree_stability)


add_library(storage-loader-utils-ref $<TARGET_OBJECTS:storage-loaders-ref>)

add_library(storage-common-utils-ref $<TARGET_OBJECTS:common-utils-ref>)

add_library(storage-commands-ref STATIC $<TARGET_OBJECTS:storage-command-conf-based-ref>)

add_library(storage-discovery-status-utils-ref
    $<TARGET_OBJECTS:discovery-impl-storage>
    $<TARGET_OBJECTS:iscsi-tgt-ref>
    $<TARGET_OBJECTS:iscsi-tgt-config-ref>
    $<TARGET_OBJECTS:sysfs-api-ref>
    $<TARGET_OBJECTS:lvm-api-ref>
    $<TARGET_OBJECTS:hotswap-impl-storage>
    $<TARGET_OBJECTS:storage-tree-stability>
)

add_executable(psme-storage main.cpp)

generate_default_config_file(
    main.cpp
    agent
    generic
    "AGENT_STORAGE_CONFIGURATION_FILE"
)

target_link_libraries(psme-storage
    ${AGENT_STORAGE_LIBRARIES}
    storage-discovery-status-utils-ref
    storage-loader-utils-ref
    storage-common-utils-ref
    ${NET_LIBRARIES}
    ${DATABASE_LIBRARIES}
    ${SYSFSREF_LIBRARIES}
    ${AGENT_FRAMEWORK_LIBRARIES}
    ${UUID_LIBRARIES}
    ${LOGGER_LIBRARIES}
    ${SAFESTRING_LIBRARIES}
    ${PCA95XX_LIBRARIES}
    ${CONFIGURATION_LIBRARIES}
    ${JSONCXX_LIBRARIES}
    ${LVM2APP_LIBRARIES}
    ${LVM2DEVMAPPER_LIBRARIES}
    ${MICROHTTPD_LIBRARIES}
    ${CURL_LIBRARIES}
    ${MD5_LIBRARIES}
    pthread
)
