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

if (NOT GTEST_FOUND)
    return()
endif ()

add_gtest(discovery_discover psme-compute
    #       Intentional. They need to be rethought.
    #        discovery_discover_test.cpp
    #        mock_send_method_test.cpp
    #        smbios_discovery_test.cpp
    purley_discoverer_test.cpp
    discovery_manager_test.cpp
    test_runner.cpp
)

generate_config_file(discovery_discover_test.cpp.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mock/configuration_full.hpp
    "COMPUTE_FULL_CONFIGURATION"
    "Declaration of full configuration"
)

set_property(SOURCE mdr_region_discovery_test.cpp
    APPEND PROPERTY OBJECT_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/mock/configuration_full.hpp)
set_property(SOURCE mock_send_method_test.cpp
    APPEND PROPERTY OBJECT_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/mock/configuration_full.hpp)
set_property(SOURCE smbios_discovery_test.cpp
    APPEND PROPERTY OBJECT_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/mock/configuration_full.hpp)

target_link_libraries(${test_target}
    compute-utils
    ${AGENT_COMPUTE_LIBRARIES}
    ${UUID_LIBRARIES}
    ${LOGGER_LIBRARIES}
    ${PCA95XX_LIBRARIES}
    configuration
    ${ISCSI_MDR_LIBRARIES}
    ipmi
    ${IPMITOOL_LIBRARIES}
    pthread
    ${MICROHTTPD_LIBRARIES}
    ${CURL_LIBRARIES}
    md5
    netlink
    smbios
    acpi
    ${SAFESTRING_LIBRARIES}
    ${NL3_LIBRARIES}
    telemetry
    ${IPMI_LIBRARIES}
)
