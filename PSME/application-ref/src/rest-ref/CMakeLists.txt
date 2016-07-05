# <license_header>
#
# Copyright (c) 2015-2016 Intel Corporation
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

add_subdirectory(metadata)

set(SOURCES
    server/mux/empty_matcher.cpp
    server/mux/segment_matcher.cpp
    server/mux/regex_matcher.cpp
    server/mux/static_matcher.cpp
    server/mux/variable_matcher.cpp
    server/mux/utils.cpp

    server/status.cpp
    server/response.cpp
    server/request.cpp
    server/parameters.cpp
    server/multiplexer.cpp
    server/server.cpp
    server/methods_handler.cpp
    server/content_types.cpp
    server/utils.cpp

    server/error/error_factory.cpp
    server/error/server_error.cpp
    server/error/server_exception.cpp
    server/connector/connector.cpp
    server/connector/connector_factory.cpp
    server/connector/connector_options.cpp
    server/connector/microhttpd/mhd_connector_options.cpp
    server/connector/microhttpd/mhd_connector.cpp
    rest_server.cpp

    constants/constants.cpp
    constants/ethernet.cpp
    constants/storage.cpp
    constants/system.cpp

    endpoints/endpoint.cpp
    endpoints/redfish.cpp
    endpoints/root.cpp
    endpoints/metadata_root.cpp
    endpoints/metadata.cpp
    endpoints/systems_collection.cpp
    endpoints/system.cpp
    endpoints/system_reset.cpp
    endpoints/chassis_collection.cpp
    endpoints/chassis.cpp
    endpoints/processors_collection.cpp
    endpoints/processor.cpp
    endpoints/storage_adapters_collection.cpp
    endpoints/storage_adapter.cpp
    endpoints/devices_collection.cpp
    endpoints/device.cpp
    endpoints/physical_drives_collection.cpp
    endpoints/physical_drive.cpp
    endpoints/memory_chunks_collection.cpp
    endpoints/memory_chunk.cpp
    endpoints/dimm_config_collection.cpp
    endpoints/dimm_config.cpp
    endpoints/manager_collection.cpp
    endpoints/manager.cpp
    endpoints/network_protocol.cpp
    endpoints/manager_network_interface_collection.cpp
    endpoints/manager_network_interface.cpp
    endpoints/system_network_interface_collection.cpp
    endpoints/system_network_interface.cpp
    endpoints/vlan_network_interface_collection.cpp
    endpoints/vlan_network_interface.cpp
    endpoints/ethernet_switch_collection.cpp
    endpoints/ethernet_switch.cpp
    endpoints/ethernet_switch_port_collection.cpp
    endpoints/ethernet_switch_port.cpp
    endpoints/acl_collection.cpp
    endpoints/acl.cpp
    endpoints/acl_bind.cpp
    endpoints/rule_collection.cpp
    endpoints/rule.cpp
    endpoints/static_mac_collection.cpp
    endpoints/static_mac.cpp
    endpoints/utils.cpp
    endpoints/path_builder.cpp
    endpoints/logical_drive_collection.cpp
    endpoints/logical_drive.cpp
    endpoints/storage_services.cpp
    endpoints/storage_services_collection.cpp
    endpoints/remote_target.cpp
    endpoints/remote_target_collection.cpp
    endpoints/thermal_zone.cpp
    endpoints/thermal_zones_collection.cpp
    endpoints/power_zone.cpp
    endpoints/power_zones_collection.cpp
    endpoints/test_event_subscription.cpp
    endpoints/event_service.cpp
    endpoints/event_subscriptions_collection.cpp
    endpoints/event_subscription.cpp

    endpoints/endpoint_builder.cpp

    eventing/event.cpp
    eventing/event_service.cpp
    eventing/event_queue.cpp
    eventing/rest_client.cpp
    eventing/config/subscription_config.cpp
    eventing/model/subscription.cpp
    eventing/manager/subscription_manager.cpp
    http/http_defs.cpp

    cache/cache.cpp

    model/watcher.cpp
    model/handlers/generic_handler.cpp
    model/handlers/handler_manager.cpp
    model/handlers/root_handler.cpp
    model/handlers/id_policy.cpp

    utils/network_interface_info.cpp
    utils/time_utils.cpp
    utils/lag_utils.cpp
    utils/mapper.cpp
    utils/json_validator.cpp
)
#add_library(application-rest ${SOURCES})

if (CMAKE_CXX_COMPILER_ID MATCHES GNU)
    set_source_files_properties(
        rest_server.cpp
        PROPERTIES COMPILE_FLAGS "-Wno-unsafe-loop-optimizations"
    )
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES Clang|GNU)
    set_source_files_properties(
        eventing/rest_client.cpp
        PROPERTIES COMPILE_FLAGS "-Wno-disabled-macro-expansion -Wno-old-style-cast"
    )
    set_source_files_properties(
        eventing/event_service.cpp
        model/handlers/handler_manager.cpp
        PROPERTIES COMPILE_FLAGS "-Wno-exit-time-destructors"
    )
    set_source_files_properties(
        endpoints/metadata_root.cpp
        endpoints/metadata.cpp
        PROPERTIES COMPILE_FLAGS "-Wno-exit-time-destructors -Wno-global-constructors"
    )
endif()

if(ENABLE_HTTPS)
    set(EXTRA_SOURCES ${EXTRA_SOURCES}
        server/certs/cert_loader.cpp
        server/certs/file_cert_loader.cpp
        server/certs/rmm_cert_loader.cpp
        server/certs/cert_manager.cpp
        server/connector/microhttpd/mhd_ssl_connector.cpp
    )

    add_definitions(-DENABLE_HTTPS)

    set_source_files_properties(
        server/cert_manager.cpp
        model/handlers/handler_manager.cpp
        model/handlers/root_handler.cpp
        PROPERTIES COMPILE_FLAGS "-Wno-exit-time-destructors"
    )
endif()

set_source_files_properties(
        utils/mapper.cpp
        PROPERTIES COMPILE_FLAGS "-Wno-exit-time-destructors -Wno-global-constructors"
)
add_library(application-rest ${SOURCES} ${EXTRA_SOURCES})

# Since app also uses generated metadata, it should wait for the headers
# to be generated
add_dependencies(application-rest metadata_generate_headers)

# Add directory for storing database files (during install)
install(DIRECTORY DESTINATION /var/opt/psme)
