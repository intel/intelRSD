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

add_gtest(tree_stability psme-storage
    storage_tree_stabilizer_test.cpp
    test_runner.cpp
)

add_library(storage-tree_stability-libs
    $<TARGET_OBJECTS:storage-tree-stability>
)

target_link_libraries(${test_target}
    ${AGENT_STORAGE_LIBRARIES}
    storage-discovery-status-utils-ref
    storage-loader-utils-ref
    storage-common-utils-ref
    ${NET_LIBRARIES}
    ${AGENT_FRAMEWORK_LIBRARIES}
    ${UUID_LIBRARIES}
    ${LOGGER_LIBRARIES}
    ${SAFESTRING_LIBRARIES}
    ${PCA95XX_LIBRARIES}
    ${CONFIGURATION_LIBRARIES}
    ${JSONCXX_LIBRARIES}
    ${LVM2APP_LIBRARIES}
    ${LVM2DEVMAPPER_LIBRARIES}
    pthread
    ${MICROHTTPD_LIBRARIES}
    ${CURL_LIBRARIES}
    md5
)
