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
    server/methods_handler.cpp
    server/content_types.cpp
    server/http_headers.cpp
    server/utils.cpp

    server/error/error_factory.cpp
    server/error/server_error.cpp
    server/error/server_exception.cpp
    server/error/message_object.cpp

    server/connector/connector.cpp
    server/connector/connector_factory.cpp
    server/connector/connector_options.cpp
    server/connector/connector_options_loader.cpp
    server/connector/microhttpd/mhd_connector_options.cpp
    server/connector/microhttpd/mhd_connector.cpp
    rest_server.cpp

    security/account/account.cpp
    security/account/account_manager.cpp
    security/account/role.cpp
    security/account/role_manager.cpp

    constants/constants.cpp
    constants/chassis.cpp
    constants/ethernet.cpp
    constants/storage.cpp
    constants/system.cpp
    constants/fabric.cpp
    constants/routes.cpp
    constants/telemetry.cpp

    endpoints/endpoint_base.cpp
    endpoints/redfish.cpp
    endpoints/root.cpp
    endpoints/odata_service_document.cpp
    endpoints/metadata_root.cpp
    endpoints/metadata.cpp
    endpoints/utils.cpp
    endpoints/task_service/task_service_utils.cpp
    endpoints/path_builder.cpp
    endpoints/manager/manager_collection.cpp
    endpoints/manager/manager.cpp
    endpoints/manager/manager_reset.cpp
    endpoints/manager/manager_load_factory_defaults.cpp
    endpoints/manager/network_protocol.cpp
    endpoints/manager/manager_network_interface_collection.cpp
    endpoints/manager/manager_network_interface.cpp
    endpoints/manager/manager_vlan_network_interface_collection.cpp
    endpoints/manager/manager_vlan_network_interface.cpp
    endpoints/update_service.cpp
    endpoints/simple_update.cpp
    endpoints/simple_update_action_info.cpp
    endpoints/event_service.cpp
    endpoints/event_subscriptions_collection.cpp
    endpoints/event_subscription.cpp
    endpoints/session_service.cpp
    endpoints/session_collection.cpp
    endpoints/session.cpp
    endpoints/task_service/task_service.cpp
    endpoints/task_service/task_collection.cpp
    endpoints/task_service/task.cpp
    endpoints/message_registry_file_collection.cpp
    endpoints/message_registry_file.cpp
    endpoints/message_registry.cpp
    endpoints/task_service/monitor.cpp
    endpoints/task_service/monitor_content_builder.cpp
    endpoints/intel_registry.cpp

    endpoints/system/systems_collection.cpp
    endpoints/system/system.cpp
    endpoints/system/system_reset.cpp
    endpoints/system/system_change_tpm_state.cpp
    endpoints/system/processors_collection.cpp
    endpoints/system/processor.cpp
    endpoints/system/processor_secure_erase.cpp
    endpoints/system/processor_metrics.cpp
    endpoints/system/storage_subsystems_collection.cpp
    endpoints/system/storage_subsystem.cpp
    endpoints/system/memory_collection.cpp
    endpoints/system/memory.cpp
    endpoints/system/memory_metrics.cpp
    endpoints/system/system_dcpmem_secure_erase.cpp
    endpoints/system/system_network_interface_collection.cpp
    endpoints/system/system_network_interface.cpp
    endpoints/system/system_metrics.cpp
    endpoints/system/volumes_collection.cpp
    endpoints/system/network_interface.cpp
    endpoints/system/network_interfaces_collection.cpp
    endpoints/system/network_device_functions_collection.cpp
    endpoints/system/network_device_function.cpp
    endpoints/system/hosted_storage_services.cpp

    endpoints/storage/drive_collection.cpp
    endpoints/storage/storage_service.cpp
    endpoints/storage/storage_services_collection.cpp
    endpoints/storage/storage_pool.cpp
    endpoints/storage/storage_pool_collection.cpp
    endpoints/storage/volume.cpp
    endpoints/storage/volume_metrics.cpp
    endpoints/storage/volume_collection.cpp
    endpoints/storage/volume_initialize.cpp
    endpoints/storage/allocated_volumes_collection.cpp
    endpoints/storage/allocated_pools_collection.cpp
    endpoints/storage/providing_drives_collection.cpp
    endpoints/storage/providing_pools_collection.cpp
    endpoints/storage/providing_volumes_collection.cpp
    endpoints/storage/capacity_source.cpp

    endpoints/ethernet/vlan_network_interface_collection.cpp
    endpoints/ethernet/vlan_network_interface.cpp
    endpoints/ethernet/ethernet_switch_collection.cpp
    endpoints/ethernet/ethernet_switch.cpp
    endpoints/ethernet/ethernet_switch_metrics.cpp
    endpoints/ethernet/ethernet_switch_port_collection.cpp
    endpoints/ethernet/ethernet_switch_port.cpp
    endpoints/ethernet/ethernet_switch_port_metrics.cpp
    endpoints/ethernet/acl.cpp
    endpoints/ethernet/acl_bind.cpp
    endpoints/ethernet/acl_collection.cpp
    endpoints/ethernet/rule.cpp
    endpoints/ethernet/rule_collection.cpp
    endpoints/ethernet/static_mac.cpp
    endpoints/ethernet/static_mac_collection.cpp

    endpoints/chassis/chassis_collection.cpp
    endpoints/chassis/chassis.cpp
    endpoints/chassis/chassis_reset.cpp
    endpoints/chassis/thermal.cpp
    endpoints/chassis/power.cpp
    endpoints/chassis/power_supply_request_state_change.cpp
    endpoints/chassis/drives_collection.cpp
    endpoints/chassis/drive.cpp
    endpoints/chassis/drive_metrics.cpp
    endpoints/chassis/drive_secure_erase.cpp
    endpoints/chassis/pcie_device.cpp
    endpoints/chassis/pcie_devices_collection.cpp
    endpoints/chassis/pcie_function.cpp
    endpoints/chassis/pcie_functions_collection.cpp

    endpoints/fabric/fabric.cpp
    endpoints/fabric/fabrics_collection.cpp
    endpoints/fabric/endpoint.cpp
    endpoints/fabric/endpoints_collection.cpp
    endpoints/fabric/switch.cpp
    endpoints/fabric/switches_collection.cpp
    endpoints/fabric/switch_reset.cpp
    endpoints/fabric/port.cpp
    endpoints/fabric/port_metrics.cpp
    endpoints/fabric/port_reset.cpp
    endpoints/fabric/ports_collection.cpp
    endpoints/fabric/zone.cpp
    endpoints/fabric/zones_collection.cpp

    endpoints/telemetry/telemetry_service.cpp
    endpoints/telemetry/metric_definition.cpp
    endpoints/telemetry/metric_definitions_collection.cpp
    endpoints/telemetry/metric_report_definition.cpp
    endpoints/telemetry/metric_report_definitions_collection.cpp
    endpoints/telemetry/trigger.cpp
    endpoints/telemetry/triggers_collection.cpp

    endpoints/account_service/account_service.cpp
    endpoints/account_service/account.cpp
    endpoints/account_service/account_collection.cpp
    endpoints/account_service/role.cpp
    endpoints/account_service/role_collection.cpp


    endpoints/endpoint_builder.cpp


    eventing/event.cpp
    eventing/event_array.cpp
    eventing/event_service.cpp
    eventing/event_array_queue.cpp
    eventing/rest_client.cpp
    eventing/model/subscription.cpp
    eventing/manager/subscription_manager.cpp

    model/watcher.cpp
    model/handlers/generic_handler.cpp
    model/handlers/handler_manager.cpp
    model/handlers/root_handler.cpp
    model/handlers/id_memoizer.cpp

    registries/config/registry_configurator.cpp
    registries/managers/message_registry_file_manager.cpp
    registries/managers/message_registry_manager.cpp
    registries/model/attributes/location.cpp
    registries/model/message_registry_file.cpp
    registries/model/message_registry.cpp

    utils/time_utils.cpp
    utils/lag_utils.cpp
    utils/zone_utils.cpp

    validators/json_validator.cpp
    validators/schemas/common.cpp
    validators/schemas/acl.cpp
    validators/schemas/subscription_collection.cpp
    validators/schemas/session_collection.cpp
    validators/schemas/session_service.cpp
    validators/schemas/endpoint.cpp
    validators/schemas/endpoints_collection.cpp
    validators/schemas/ethernet_switch.cpp
    validators/schemas/ethernet_switch_port.cpp
    validators/schemas/ethernet_switch_port_collection.cpp
    validators/schemas/rule.cpp
    validators/schemas/rule_collection.cpp
    validators/schemas/static_mac.cpp
    validators/schemas/vlan_network_interface_collection.cpp
    validators/schemas/vlan_network_interface.cpp
    validators/schemas/zone.cpp
    validators/schemas/zones_collection.cpp
    validators/schemas/drive.cpp
    validators/schemas/switch.cpp
    validators/schemas/storage_subsystem.cpp
    validators/schemas/thermal.cpp
    validators/schemas/chassis.cpp
    validators/schemas/request_state_change.cpp
    validators/schemas/pcie_device.cpp
    validators/schemas/system.cpp
    validators/schemas/reset.cpp
    validators/schemas/change_tpm_state.cpp
    validators/schemas/simple_update.cpp
    validators/schemas/network_device_function.cpp
    validators/schemas/static_mac_collection.cpp
    validators/schemas/volume.cpp
    validators/schemas/storage_pool.cpp
    validators/schemas/system_dcpmem_secure_erase.cpp
    validators/schemas/processor.cpp
)

if (CMAKE_CXX_COMPILER_ID MATCHES GNU)
    set_source_files_properties(
        rest_server.cpp
        PROPERTIES COMPILE_FLAGS "-Wno-unsafe-loop-optimizations"
    )
    set_source_files_properties(
        server/error/error_factory.cpp
        PROPERTIES COMPILE_FLAGS "-Wno-suggest-attribute=format"
    )
    set_source_files_properties(
        utils/time_utils.cpp
        PROPERTIES COMPILE_FLAGS "-Wno-format-nonliteral"
    )
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES Clang)
    set_source_files_properties(
        endpoints/simple_update.cpp
        COMPILE_FLAGS "-Wno-disabled-macro-expansion"
    )
    set_source_files_properties(
        server/error/error_factory.cpp
        PROPERTIES COMPILE_FLAGS "-Wno-format-nonliteral"
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
        security/authentication/basic_authentication.cpp
        security/authentication/session_authentication.cpp
        security/authentication/authentication_factory.cpp
        security/session/session.cpp
        security/session/session_manager.cpp
        security/session/session_service_manager.cpp
        security/authentication/client_cert_authentication.cpp
        security/session/session_service.cpp
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
