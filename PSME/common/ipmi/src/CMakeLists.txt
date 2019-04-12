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

add_subdirectory(command)
add_subdirectory(manager)

set(SOURCES
    connection_data.cpp
    bridge_info.cpp
    management_controller.cpp
    ipmi_controller.cpp
    ipmi_interface.cpp
    ipmi_interface_factory.cpp
    management_controller.cpp
    message.cpp
    request.cpp
    response.cpp
    response_error.cpp
    sel_record.cpp
    sel.cpp
    utils/byte_buffer.cpp

    telemetry/telemetry_hub.cpp
)

# Clang does not understand that attribute [[noreturn]] on throw_error_on_completion_code
# is not inherited in child class implementations
if (CMAKE_CXX_COMPILER_ID MATCHES Clang)
    set_source_files_properties(
            response.cpp
            PROPERTIES COMPILE_FLAGS "-Wno-unreachable-code"
    )
endif()

set(UTILS_SDV_SOURCES
    utils/sdv/mdr_region_accessor.cpp
    utils/sdv/purley_mdr_region_accesor.cpp
    utils/sdv/grantley_mdr_region_accessor.cpp
    utils/sdv/linear_to_double.cpp
    utils/sdv/dcpmem_data_accessor.cpp
    utils/sdv/platform_discovery.cpp
)

if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/utils/sdv")
    add_library(ipmi-base OBJECT ${SOURCES} ${UTILS_SDV_SOURCES})
else()
    add_library(ipmi-base OBJECT ${SOURCES})
endif()

if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/command/sdv")
    add_library(ipmi STATIC
        $<TARGET_OBJECTS:ipmi-base>
        $<TARGET_OBJECTS:ipmi-command-generic>
        $<TARGET_OBJECTS:ipmi-command-sdv>
        $<TARGET_OBJECTS:ipmi-manager-ipmitool>
    )
else()
    add_library(ipmi STATIC
        $<TARGET_OBJECTS:ipmi-base>
        $<TARGET_OBJECTS:ipmi-command-generic>
        $<TARGET_OBJECTS:ipmi-manager-ipmitool>
    )
endif()

target_link_libraries(ipmi
    ${IPMITOOL_LIBRARIES}
    ${LOGGER_LIBRARIES}
    pthread
)

install(TARGETS ipmi
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
