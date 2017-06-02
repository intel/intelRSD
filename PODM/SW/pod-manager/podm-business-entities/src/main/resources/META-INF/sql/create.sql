create table boot_boot_source_override_mode_supported (boot_id int8 not null, boot_source_override_mode_supported text, boot_source_override_mode_supported_order int4 not null, primary key (boot_id, boot_source_override_mode_supported_order))
create table boot_boot_source_override_supported (boot_id int8 not null, boot_source_override_supported text, boot_source_override_supported_order int4 not null, primary key (boot_id, boot_source_override_supported_order))
create table chassis (asset_tag text, chassis_type text, entity_id text, indicator_led text, location_id text, location_parent_id text, manufacturer text, model text, part_number text, geo_tag text, rack_supports_disaggregated_power_cooling boolean, rack_uuid text, serial_number text, sku text, id int8 not null, parent_chassis_id int8, power_id int8, thermal_id int8, primary key (id))
create table chassis_computer_system (chassis_id int8 not null, computer_system_id int8 not null, primary key (chassis_id, computer_system_id))
create table chassis_fabric_switch (chassis_id int8 not null, switch_id int8 not null, primary key (chassis_id, switch_id))
create table chassis_manager (chassis_id int8 not null, manager_id int8 not null, primary key (chassis_id, manager_id))
create table chassis_pcie_device (chassis_id int8 not null, pcie_device_id int8 not null, primary key (chassis_id, pcie_device_id))
create table command_shell_connect_type_supported (command_shell_id int8 not null, connect_type_supported text, connect_type_supported_order int4 not null, primary key (command_shell_id, connect_type_supported_order))
create table composed_node (id  bigserial not null, event_source_context text, version integer DEFAULT 0 not null, associated_compute_service_uuid text, associated_computer_system_uuid text, associated_remote_target_iqn text, associated_storage_service_uuid text, composed_node_state text, description text, eligible_for_recovery boolean, entity_id bigserial not null, name text, number_of_requested_drives int4, remote_drive_capacity_gib numeric, status text, computer_system_id int8, primary key (id))
create table computer_system (asset_tag text, bios_version text, boot_source_override_enabled text, boot_source_override_mode text, boot_source_override_target text, discovery_state text, entity_id text, host_name text, indicator_led text, manufacturer text, memory_sockets int4, memory_status text, model text, part_number text, power_state text, processor_model text, processor_socket int4, processor_status text, processors_count int4, serial_number text, sku text, system_type text, total_system_memory_gib numeric, uuid text, id int8 not null, computer_system_metadata_id int8, primary key (id))
create table computer_system_allowable_reset_type (computer_system_id int8 not null, allowable_reset_type text, allowable_reset_type_order int4 not null, primary key (computer_system_id, allowable_reset_type_order))
create table computer_system_manager (computer_system_id int8 not null, manager_id int8 not null, primary key (computer_system_id, manager_id))
create table computer_system_metadata (id  bigserial not null, event_source_context text, version integer DEFAULT 0 not null, allocated boolean, deep_discovery_state text, task_uuid text, primary key (id))
create table computer_system_pci_device (computer_system_id int8 not null, device_id text, vendor_id text, pci_device_order int4 not null, primary key (computer_system_id, pci_device_order))
create table computer_system_pcie_connection_id (computer_system_id int8 not null, pcie_connection_id text, pcie_connection_id_order int4 not null, primary key (computer_system_id, pcie_connection_id_order))
create table connected_entity (entity_id text, entity_role int4, entity_type int4, pci_class_code text, pci_function_number int4, device_id text, subsystem_id text, subsystem_vendor_id text, vendor_id text, id int8 not null, endpoint_id int8, drive_id int8, primary key (id))
create table connected_entity_identifiers (identifiers_id int8 not null, durable_name text, durable_name_format text)
create table discoverable_entity (discriminator_class text not null, id  bigserial not null, event_source_context text, version integer DEFAULT 0 not null, description text, name text, uri text, status text, external_service_id int8, primary key (id))
create table drive (asset_tag text, block_size_bytes int4, capable_speed_gbs numeric, capacity_bytes int8, drive_erased boolean, encryption_ability text, encryption_status text, entity_id text, erase_on_detach boolean, failure_predicted boolean, firmware_version text, hotspare_type text, indicator_led text, manufacturer text, media_type text, model text, negotiated_speed_gbs numeric, part_number text, predicted_media_life_left_percent numeric, protocol text, revision text, rotation_speed_rpm numeric, serial_number text, sku text, status_indicator text, id int8 not null, chassis_id int8, composed_node_id int8, drive_metadata_id int8, pcie_device_function_id int8, storage_id int8, primary key (id))
create table drive_identifiers (identifiers_id int8 not null, durable_name text, durable_name_format text, drive_identifiers_order int4 not null, primary key (identifiers_id, drive_identifiers_order))
create table drive_location (drive_id int8 not null, info text, info_format text, drive_location_order int4 not null, primary key (drive_id, drive_location_order))
create table drive_metadata (id  bigserial not null, event_source_context text, version integer DEFAULT 0 not null, allocated boolean, primary key (id))
create table endpoint (entity_id text, host_reservation_memory_bytes int4, device_id text, subsystem_id text, subsystem_vendor_id text, vendor_id text, endpoint_protocol text, id int8 not null, computer_system_id int8, fabric_id int8, zone_id int8, primary key (id))
create table endpoint_identifiers (identifiers_id int8 not null, durable_name text, durable_name_format text)
create table entity_related_items (item_owner_id int8 not null, item_id int8 not null, primary key (item_owner_id, item_id))
create table ethernet_interface (autoneg boolean, entity_id text, fqdn text, full_duplex boolean, hostname text, ip_ipv6_default_gateway text, interface_enabled boolean, mac_address text, max_ipv6_static_addresses int4, mtu_size int4, permanent_mac_address text, speed_mbps int4, vlan_enable boolean, vlan_id int4, id int8 not null, computer_system_id int8, primary key (id))
create table ethernet_interface_ipv4_address (ethernet_interface_id int8 not null, address text, address_origin text, gateway text, oem text, subnet_mask text, ipv4_address_order int4 not null, primary key (ethernet_interface_id, ipv4_address_order))
create table ethernet_interface_ipv6_address (ethernet_interface_id int8 not null, address text, address_origin text, address_state text, oem text, prefix_length int4, ipv6_address_order int4 not null, primary key (ethernet_interface_id, ipv6_address_order))
create table ethernet_interface_ipv6_address_policy (ethernet_interface_id int8 not null, label int4, precedence int4, prefix text, ipv6_address_policy_order int4 not null, primary key (ethernet_interface_id, ipv6_address_policy_order))
create table ethernet_interface_ipv6_static_address (ethernet_interface_id int8 not null, address text, address_origin text, address_state text, oem text, prefix_length int4, ipv6_static_address_order int4 not null, primary key (ethernet_interface_id, ipv6_static_address_order))
create table ethernet_interface_manager (ethernet_interface_id int8 not null, manager_id int8 not null, primary key (ethernet_interface_id, manager_id))
create table ethernet_interface_name_server (ethernet_interface_id int8 not null, name_server text, name_server_order int4 not null, primary key (ethernet_interface_id, name_server_order))
create table ethernet_interface_pcie_functions (ethernet_interface_id int8 not null, pcie_function_id int8 not null, primary key (ethernet_interface_id, pcie_function_id))
create table ethernet_switch (entity_id text, firmware_name text, firmware_version text, manufacturer text, manufacturing_date text, model text, part_number text, role text, serial_number text, switch_id text, id int8 not null, chassis_id int8, primary key (id))
create table ethernet_switch_manager (ethernet_switch_id int8 not null, manager_id int8 not null, primary key (ethernet_switch_id, manager_id))
create table ethernet_switch_port (administrative_state text, autosense boolean, entity_id text, frame_size int4, full_duplex boolean, link_speed_gbps int4, link_type text, mac_address text, neighbor_info text, neighbor_mac text, operational_state text, port_class text, port_id text, port_mode text, port_type text, id int8 not null, ethernet_switch_id int8, member_of_port_id int8, primary_vlan_id int8, primary key (id))
create table ethernet_switch_port_ipv4_address (ethernet_interface_id int8 not null, address text, address_origin text, gateway text, oem text, subnet_mask text, ipv4_address_order int4 not null, primary key (ethernet_interface_id, ipv4_address_order))
create table ethernet_switch_port_ipv6_address (ethernet_interface_id int8 not null, address text, address_origin text, address_state text, oem text, prefix_length int4, ipv6_address_order int4 not null, primary key (ethernet_interface_id, ipv6_address_order))
create table ethernet_switch_port_vlan (entity_id text, tagged boolean, vlan_enable boolean, vlan_id int4, id int8 not null, ethernet_interface_id int8, ethernet_switch_port_id int8, primary key (id))
create table event_subscription (id  bigserial not null, event_source_context text, version integer DEFAULT 0 not null, description text, destination text, entity_id bigserial not null, name text, protocol text, subscription_context text, primary key (id))
create table event_subscription_event_type (event_type_id int8 not null, event_type text, event_type_order int4 not null, primary key (event_type_id, event_type_order))
create table event_subscription_origin_resources (event_type_id int8 not null, origin_resources text, origin_resources_order int4 not null, primary key (event_type_id, origin_resources_order))
create table external_service (id  bigserial not null, event_source_context text, version integer DEFAULT 0 not null, uri text, entity_id bigserial not null, is_reachable boolean, service_type text, unreachable_since timestamp, uuid text, primary key (id))
create table fabric (entity_id text, fabric_type text, max_zones int4, id int8 not null, primary key (id))
create table fan (id  bigserial not null, event_source_context text, version integer DEFAULT 0 not null, name text, rack_unit text, u_height int4, u_location int4, x_location int4, reading_rpm int4, status text, thermal_zone_id int8, primary key (id))
create table graphical_console_connect_type_supported (graphical_console_id int8 not null, connect_type_supported text, connect_type_supported_order int4 not null, primary key (graphical_console_id, connect_type_supported_order))
create table logical_drive (bootable boolean, capacity_gib numeric, entity_id text, image text, mode text, snapshot boolean, type text, write_protected boolean, id int8 not null, composed_node_id int8, master_drive_id int8, storage_service_id int8, primary key (id))
create table logical_drive_logical_drive (used_by_logical_drive_id int8 not null, used_logical_drive_id int8 not null, primary key (used_by_logical_drive_id, used_logical_drive_id))
create table manager (command_shell_max_concurrent_sessions int4, command_shell_service_enabled boolean, entity_id text, firmware_version text, graphical_console_max_concurrent_sessions int4, graphical_console_service_enabled boolean, manager_type text, model text, power_state text, serial_console_max_concurrent_sessions int4, serial_console_service_enabled boolean, service_entry_point_uuid text, uuid text, id int8 not null, in_chassis_manager_id int8, primary key (id))
create table memory (base_module_type text, bus_width_bits int4, capacity_mib int4, data_width_bits int4, device_id text, device_locator text, entity_id text, error_correction text, firmware_api_version text, firmware_revision text, manufacturer text, memory_device_type text, channel int4, memory_controller int4, slot int4, socket int4, memory_type text, operating_speed_mhz int4, part_number text, rank_count int4, serial_number text, vendor_id text, voltage_volt numeric, id int8 not null, computer_system_id int8, primary key (id))
create table memory_allowed_speed_mhz (computer_system_id int8 not null, allowed_speed_mhz int4, allowed_speed_mhz_order int4 not null, primary key (computer_system_id, allowed_speed_mhz_order))
create table memory_function_class (memory_id int8 not null, function_class text, function_class_order int4 not null, primary key (memory_id, function_class_order))
create table memory_memory_media (memory_id int8 not null, memory_media text, memory_media_order int4 not null, primary key (memory_id, memory_media_order))
create table memory_operating_memory_mode (memory_id int8 not null, operating_memory_mode text, operating_memory_mode_order int4 not null, primary key (memory_id, operating_memory_mode_order))
create table memory_region (memory_id int8 not null, memory_type text, offset_mib int4, region_id text, size_mib int4, memory_region_order int4 not null, primary key (memory_id, memory_region_order))
create table network_device_function (device_enabled boolean, entity_id text, mac_address text, authentication_method text, chap_secret text, chap_username text, initiator_default_gateway text, initiator_ip_address text, initiator_name text, initiator_netmask text, ip_address_type text, ip_mask_dns_via_dhcp boolean, mutual_chap_secret text, mutual_chap_username text, primary_dns text, primary_lun int4, primary_target_ip_address text, primary_target_name text, primary_target_tcp_port int4, primary_vlan_enable boolean, primary_vlan_id int4, router_advertisement_enabled boolean, secondary_dns text, secondary_lun int4, secondary_target_ip_address text, secondary_target_name text, secondary_target_tcp_port int4, secondary_vlan_enable boolean, secondary_vlan_id int4, target_info_via_dhcp boolean, id int8 not null, network_interface_id int8, primary key (id))
create table network_interface (entity_id text, status text, id int8 not null, computer_system_id int8, primary key (id))
create table network_protocol (entity_id text, fqdn text, hostname text, http_port int4, http_protocol_enabled boolean, https_port int4, https_protocol_enabled boolean, ipmi_port int4, ipmi_protocol_enabled boolean, kvm_ip_port int4, kvm_ip_protocol_enabled boolean, snmp_port int4, snmp_protocol_enabled boolean, notify_ip_v6_scope text, ssdp_notify_multicast_interval_seconds int4, ssdp_notify_ttl int4, ssdp_port int4, ssdp_protocol_enabled boolean, ssh_port int4, ssh_protocol_enabled boolean, telnet_port int4, telnet_protocol_enabled boolean, virtual_media_port int4, virtual_media_protocol_enabled boolean, id int8 not null, manager_id int8, primary key (id))
create table pcie_device (asset_tag text, device_type text, entity_id text, firmware_version text, manufacturer text, model text, part_number text, serial_number text, sku text, id int8 not null, computer_system_id int8, primary key (id))
create table pcie_device_function (class_code text, device_class text, device_id text, entity_id text, function_id int4, function_type text, revision_id text, subsystem_id text, subsystem_vendor_id text, vendor_id text, id int8 not null, computer_system_id int8, pcie_device_id int8, primary key (id))
create table physical_drive (capacity_gib float4, entity_id text, manufacturer text, model text, rpm int4, serial_number text, storage_controller_interface text, type text, id int8 not null, storage_service_id int8, primary key (id))
create table physical_drive_logical_drive (physical_drive_id int8 not null, logical_drive_id int8 not null, primary key (physical_drive_id, logical_drive_id))
create table port (current_speed_gbps int8, entity_id text, max_speed_gbps int8, port_id text, port_protocol text, port_type text, width int8, id int8 not null, fabric_switch_id int8, primary key (id))
create table port_allowable_reset_type (port_id int8 not null, allowable_reset_type text, allowable_reset_type_order int4 not null, primary key (port_id, allowable_reset_type_order))
create table port_endpoints (endpoint_id int8 not null, port_id int8 not null, primary key (endpoint_id, port_id))
create table port_pcie_connection_id (port_id int8 not null, pcie_connection_id text, pcie_connection_id_order int4 not null, primary key (port_id, pcie_connection_id_order))
create table power (entity_id text, id int8 not null, chassis_id int8, primary key (id))
create table power_control (entity_id text, member_id text, oem text, power_allocated_watts int4, power_available_watts int4, power_capacity_watts int4, power_consumed_watts int4, correction_in_ms int4, limit_exception text, limit_in_watts int4, average_consumed_watts int4, interval_in_min int4, max_consumed_watts int4, min_consumed_watts int4, power_requested_watts int4, id int8 not null, power_id int8, primary key (id))
create table power_supply (entity_id text, firmware_version text, indicator_led text, last_power_output_watts numeric, line_input_voltage numeric, line_input_voltage_type text, manufacturer text, member_id text, model text, oem text, part_number text, power_capacity_watts numeric, power_supply_type text, serial_number text, spare_part_number text, id int8 not null, power_id int8, primary key (id))
create table power_supply_input_range (power_supply_id int8 not null, input_type text, maximum_frequency_hz numeric, maximum_voltage numeric, minimum_frequency_hz numeric, minimum_voltage numeric, oem text, output_wattage numeric, input_range_order int4 not null, primary key (power_supply_id, input_range_order))
create table power_voltage (entity_id text, lower_threshold_critical numeric, lower_threshold_fatal numeric, lower_threshold_non_critical numeric, max_reading_range numeric, member_id text, min_reading_range numeric, physical_context text, reading_volts numeric, sensor_number int4, upper_threshold_critical numeric, upper_threshold_fatal numeric, upper_threshold_non_critical numeric, id int8 not null, power_id int8, primary key (id))
create table power_zone (entity_id text, max_psus_supported int4, number_of_psus_present int4, power_capacity_watts int4, power_consumed_watts int4, power_output_watts int4, presence text, rack_unit text, u_height int4, u_location int4, x_location int4, id int8 not null, chassis_id int8, primary key (id))
create table power_zone_power_supply (id  bigserial not null, event_source_context text, version integer DEFAULT 0 not null, firmware_revision text, last_power_output_watts int4, manufacturer text, model_number text, name text, part_number text, power_capacity_watts int4, rack_unit text, u_height int4, u_location int4, x_location int4, serial_number text, status text, power_zone_id int8, primary key (id))
create table processor (brand text, entity_id text, instruction_set text, manufacturer text, max_speed_mhz int4, model text, processor_architecture text, effective_family text, effective_model text, identification_registers text, microcode_info text, step text, vendor_id text, processor_type text, socket text, total_cores int4, total_threads int4, id int8 not null, computer_system_id int8, primary key (id))
create table processor_capability (processor_id int8 not null, capability text, capability_order int4 not null, primary key (processor_id, capability_order))
create table redundancy (entity_id text, max_num_supported int4, member_id text, min_num_needed int4, mode text, redundancy_enabled boolean, id int8 not null, redundancy_owner_id int8, primary key (id))
create table redundancy_member (redundancy_id int8 not null, member_id int8 not null, primary key (redundancy_id, member_id))
create table remote_target (entity_id text, iscsi_initiator_iqn text, type text, id int8 not null, composed_node_id int8, remote_target_metadata_id int8, storage_service_id int8, primary key (id))
create table remote_target_iscsi_address (id  bigserial not null, event_source_context text, version integer DEFAULT 0 not null, mutual_username text, type text, username text, target_iqn text, target_portal_ip text, target_portal_port int4, remote_target_id int8, primary key (id))
create table remote_target_iscsi_address_target_lun (remote_target_iscsi_address_id int8 not null, target_lun int4, target_lun_order int4 not null, primary key (remote_target_iscsi_address_id, target_lun_order))
create table remote_target_logical_drive (remote_target_id int8 not null, logical_drive_id int8 not null, primary key (remote_target_id, logical_drive_id))
create table remote_target_metadata (id  bigserial not null, event_source_context text, version integer DEFAULT 0 not null, allocated boolean, primary key (id))
create table serial_console_connect_type_supported (serial_console_id int8 not null, connect_type_supported text, connect_type_supported_order int4 not null, primary key (serial_console_id, connect_type_supported_order))
create table simple_storage (entity_id text, uefi_device_path text, id int8 not null, computer_system_id int8, primary key (id))
create table simple_storage_device (id  bigserial not null, event_source_context text, version integer DEFAULT 0 not null, capacity_bytes numeric, entity_id bigserial not null, manufacturer text, model text, name text, oem text, status text, simple_storage_id int8, primary key (id))
create table storage (entity_id text, id int8 not null, chassis_id int8, computer_system_id int8, primary key (id))
create table storage_controller (id  bigserial not null, event_source_context text, version integer DEFAULT 0 not null, asset_tag text, entity_id bigserial not null, firmware_version text, manufacturer text, member_id text, model text, part_number text, serial_number text, sku text, speed_gbps numeric, status text, storage_id int8, storage_adapter_id int8, primary key (id))
create table storage_controller_identifiers (storage_controller_id int8 not null, durable_name text, durable_name_format text, storage_controller_identifiers_order int4 not null, primary key (storage_controller_id, storage_controller_identifiers_order))
create table storage_controller_pcie_functions (ethernet_interface_id int8 not null, pcie_function_id int8 not null, primary key (ethernet_interface_id, pcie_function_id))
create table storage_controller_supported_controller_protocol (storage_controller_id int8 not null, supported_controller_protocol text, supported_controller_protocol_order int4 not null, primary key (storage_controller_id, supported_controller_protocol_order))
create table storage_controller_supported_device_protocol (storage_controller_id int8 not null, supported_device_protocol text, supported_device_protocol_order int4 not null, primary key (storage_controller_id, supported_device_protocol_order))
create table storage_service (entity_id text, id int8 not null, primary key (id))
create table storage_service_manager (storage_service_id int8 not null, manager_id int8 not null, primary key (storage_service_id, manager_id))
create table switch (asset_tag text, domain_id int4, entity_id text, indicator_led text, is_managed boolean, manufacturer text, model text, part_number text, power_state text, serial_number text, sku text, switch_type text, total_switch_width int4, id int8 not null, fabric_id int8, primary key (id))
create table switch_allowable_reset_type (switch_id int8 not null, allowable_reset_type text, allowable_reset_type_order int4 not null, primary key (switch_id, allowable_reset_type_order))
create table switch_manager (switch_id int8 not null, manager_id int8 not null, primary key (switch_id, manager_id))
create table temperature (id  bigserial not null, event_source_context text, version integer DEFAULT 0 not null, name text, physical_context text, reading_celsius int4, status text, thermal_zone_id int8, primary key (id))
create table thermal (entity_id text, id int8 not null, chassis_id int8, primary key (id))
create table thermal_fan (entity_id text, lower_threshold_critical int4, lower_threshold_fatal int4, lower_threshold_non_critical int4, max_reading_range int4, member_id text, min_reading_range int4, physical_context int4, reading int4, reading_units text, upper_threshold_critical int4, upper_threshold_fatal int4, upper_threshold_non_critical int4, id int8 not null, thermal_id int8, primary key (id))
create table thermal_temperature (entity_id text, lower_threshold_critical numeric, lower_threshold_fatal numeric, lower_threshold_non_critical numeric, max_reading_range numeric, member_id text, min_reading_range numeric, physical_context int4, reading_celsius numeric, sensor_number int4, upper_threshold_critical numeric, upper_threshold_fatal numeric, upper_threshold_non_critical numeric, id int8 not null, thermal_id int8, primary key (id))
create table thermal_zone (desired_speed_pwm int4, desired_speed_rpm int4, entity_id text, max_fans_supported int4, number_of_fans_present int4, presence text, rack_unit text, u_height int4, u_location int4, x_location int4, volumetric_airflow int4, id int8 not null, chassis_id int8, primary key (id))
create table unknown_oems (entity_id int8 not null, oem_path text, oem_value text, unknown_oem_order int4 not null, primary key (entity_id, unknown_oem_order))
create table zone (entity_id text, id int8 not null, fabric_id int8, primary key (id))
create table zone_switches (switch_id int8 not null, zone_id int8 not null, primary key (switch_id, zone_id))
alter table chassis add constraint idx_chassis_entity_id  unique (entity_id)
alter table composed_node add constraint idx_composed_node_entity_id  unique (entity_id)
alter table computer_system add constraint idx_computer_system_entity_id  unique (entity_id)
alter table connected_entity add constraint idx_connected_entity_id  unique (entity_id)
alter table drive add constraint idx_drive_entity_id  unique (entity_id)
alter table endpoint add constraint idx_endpoint_entity_id  unique (entity_id)
alter table ethernet_interface add constraint idx_ethernet_interface_entity_id  unique (entity_id)
alter table ethernet_switch add constraint idx_ethernet_switch_entity_id  unique (entity_id)
alter table ethernet_switch_port add constraint idx_ethernet_switch_port_entity_id  unique (entity_id)
alter table ethernet_switch_port_vlan add constraint idx_ethernet_switch_port_vlan_entity_id  unique (entity_id)
alter table event_subscription add constraint idx_event_subscription_entity_id  unique (entity_id)
alter table external_service add constraint idx_external_service_entity_id  unique (entity_id)
alter table external_service add constraint UK_gu9cd38kcy8owp3muxw0by9mj  unique (uuid)
alter table fabric add constraint idx_fabric_entity_id  unique (entity_id)
alter table logical_drive add constraint idx_logical_drive_entity_id  unique (entity_id)
alter table manager add constraint idx_manager_entity_id  unique (entity_id)
alter table memory add constraint idx_memory_entity_id  unique (entity_id)
alter table network_device_function add constraint idx_network_device_function_entity_id  unique (entity_id)
alter table network_interface add constraint idx_network_interface_entity_id  unique (entity_id)
alter table network_protocol add constraint idx_network_protocol_entity_id  unique (entity_id)
alter table pcie_device add constraint idx_pcie_device_entity_id  unique (entity_id)
alter table pcie_device_function add constraint idx_pcie_device_function_entity_id  unique (entity_id)
alter table physical_drive add constraint idx_physical_drive_entity_id  unique (entity_id)
alter table port add constraint idx_port_entity_id  unique (entity_id)
alter table power add constraint idx_power_entity_id  unique (entity_id)
alter table power_control add constraint idx_power_control_entity_id  unique (entity_id)
alter table power_supply add constraint idx_power_supply_id  unique (entity_id)
alter table power_voltage add constraint idx_power_voltage_entity_id  unique (entity_id)
alter table power_zone add constraint idx_power_zone_entity_id  unique (entity_id)
alter table processor add constraint idx_processor_entity_id  unique (entity_id)
alter table redundancy add constraint idx_redundancy_entity_id  unique (entity_id)
alter table remote_target add constraint idx_remote_target_entity_id  unique (entity_id)
alter table simple_storage add constraint idx_simple_storage_entity_id  unique (entity_id)
alter table simple_storage_device add constraint idx_simple_storage_device_entity_id  unique (entity_id)
alter table storage add constraint idx_storage_entity_id  unique (entity_id)
alter table storage_controller add constraint idx_storage_controller_entity_id  unique (entity_id)
alter table storage_service add constraint idx_storage_service_entity_id  unique (entity_id)
alter table switch add constraint idx_switch_entity_id  unique (entity_id)
alter table thermal add constraint idx_thermal_entity_id  unique (entity_id)
alter table thermal_fan add constraint idx_thermal_fan_entity_id  unique (entity_id)
alter table thermal_temperature add constraint idx_thermal_temperature_entity_id  unique (entity_id)
alter table thermal_zone add constraint idx_thermal_zone_entity_id  unique (entity_id)
alter table zone add constraint idx_zone_entity_id  unique (entity_id)
alter table boot_boot_source_override_mode_supported add constraint FK_5s5sope9k484hvv3ujcbbw108 foreign key (boot_id) references computer_system
alter table boot_boot_source_override_supported add constraint FK_i8r660cenjyj95upx9kwp6in8 foreign key (boot_id) references computer_system
alter table chassis add constraint FK_fdaesv3uw07hhfk9rlmaynxbp foreign key (parent_chassis_id) references chassis
alter table chassis add constraint FK_e35tqm7ig655wyxsou6ta5ei4 foreign key (power_id) references power
alter table chassis add constraint FK_gma81rk2h2kmntid0y9g4kuji foreign key (thermal_id) references thermal
alter table chassis add constraint FK_rn2i9y69jbo0ta2046abdtj8u foreign key (id) references discoverable_entity
alter table chassis_computer_system add constraint FK_hseban2kwly4brj8wyrrqvey0 foreign key (computer_system_id) references computer_system
alter table chassis_computer_system add constraint FK_t2f44367kc2tylbabdtjska31 foreign key (chassis_id) references chassis
alter table chassis_fabric_switch add constraint FK_8mvl7c6cw9dh3vv9w9x9g7awd foreign key (switch_id) references switch
alter table chassis_fabric_switch add constraint FK_hn0xonsw2n12ttcg8uja39r6a foreign key (chassis_id) references chassis
alter table chassis_manager add constraint FK_h8h298vwc0mrybdblrgarol22 foreign key (manager_id) references manager
alter table chassis_manager add constraint FK_kr1h0ayly65i5t3tsqea3v34 foreign key (chassis_id) references chassis
alter table chassis_pcie_device add constraint FK_bwbmyt4g5627wfilvxygxsiqr foreign key (pcie_device_id) references pcie_device
alter table chassis_pcie_device add constraint FK_e71b8tftjlqno48so21oibbwt foreign key (chassis_id) references chassis
alter table command_shell_connect_type_supported add constraint FK_p27rh6q8nu69iohinyk1pxl51 foreign key (command_shell_id) references manager
alter table composed_node add constraint FK_6dvurerir9p7quf3b4bq6vq0t foreign key (computer_system_id) references computer_system
alter table computer_system add constraint FK_md6awm92wh5cq8dmsgwkdbpy1 foreign key (computer_system_metadata_id) references computer_system_metadata
alter table computer_system add constraint FK_cwca10t6xao4h4cdqea0dxita foreign key (id) references discoverable_entity
alter table computer_system_allowable_reset_type add constraint FK_q1iv5xwi4hqrxlwwy374amwth foreign key (computer_system_id) references computer_system
alter table computer_system_manager add constraint FK_5qjqbrmviflp7dgyh3imnvkaw foreign key (manager_id) references manager
alter table computer_system_manager add constraint FK_9gbeankygenoq464djmovjy0c foreign key (computer_system_id) references computer_system
alter table computer_system_pci_device add constraint FK_2du70hhi6tao2ht9yv0110si9 foreign key (computer_system_id) references computer_system
alter table computer_system_pcie_connection_id add constraint FK_csh08v7q5vjxvgt2csgwb9fp6 foreign key (computer_system_id) references computer_system
alter table connected_entity add constraint FK_17a70jj241lk1okb0brdu5m44 foreign key (endpoint_id) references endpoint
alter table connected_entity add constraint FK_gdik340299pfi7vusb10moffb foreign key (drive_id) references discoverable_entity
alter table connected_entity add constraint FK_trradgh56rj8ff6v04rsk94ug foreign key (id) references discoverable_entity
alter table connected_entity_identifiers add constraint FK_gcjkq1neyty3mi581qyfi53y3 foreign key (identifiers_id) references connected_entity
alter table discoverable_entity add constraint FK_le91xyho6pfmihmirxrpttdd5 foreign key (external_service_id) references external_service
alter table drive add constraint FK_jnpr25lir5pn6sct8eu2sdhjf foreign key (chassis_id) references chassis
alter table drive add constraint FK_hg2i3dcoyr30041ams83q6k1c foreign key (composed_node_id) references composed_node
alter table drive add constraint FK_rnn8cx4x4xma4clftuqi3boy4 foreign key (drive_metadata_id) references drive_metadata
alter table drive add constraint FK_stwmvhg01t9xvrifgeo7en331 foreign key (pcie_device_function_id) references pcie_device_function
alter table drive add constraint FK_k0edqaee47me4mjoffv3poh77 foreign key (storage_id) references storage
alter table drive add constraint FK_k7uvdl7n7gcdpko42179klcvl foreign key (id) references discoverable_entity
alter table drive_identifiers add constraint FK_73au0jly5e2kj5djg2aohfpys foreign key (identifiers_id) references drive
alter table drive_location add constraint FK_dlrx37ioi7c79f0irlrgeokp7 foreign key (drive_id) references drive
alter table endpoint add constraint FK_6p15jxa7qa2u2iylyig3n4sx4 foreign key (computer_system_id) references computer_system
alter table endpoint add constraint FK_ofmdqutkvp6sw3g905brjxuhv foreign key (fabric_id) references fabric
alter table endpoint add constraint FK_s3uwsvug4plk1dpprwdln4smr foreign key (zone_id) references zone
alter table endpoint add constraint FK_7j9v08qwbrpl4wxrjog3cy1u7 foreign key (id) references discoverable_entity
alter table endpoint_identifiers add constraint FK_k2ahas7cdprjfq32dyx4774ms foreign key (identifiers_id) references endpoint
alter table entity_related_items add constraint FK_dkqekqmeuqf8f9tqs9qsqaewr foreign key (item_id) references discoverable_entity
alter table entity_related_items add constraint FK_7xapdwhoafh8etjic10487qrs foreign key (item_owner_id) references discoverable_entity
alter table ethernet_interface add constraint FK_bm8wr6mkdvjpxuvqqpr0iobq2 foreign key (computer_system_id) references computer_system
alter table ethernet_interface add constraint FK_36pt3bdrw0ew81twg7n0kltg7 foreign key (id) references discoverable_entity
alter table ethernet_interface_ipv4_address add constraint FK_ien8cwxqp5dfsgvhsvrefivow foreign key (ethernet_interface_id) references ethernet_interface
alter table ethernet_interface_ipv6_address add constraint FK_3uxtn8e1kx25nrm0f0eubxyov foreign key (ethernet_interface_id) references ethernet_interface
alter table ethernet_interface_ipv6_address_policy add constraint FK_mqaxrr7p0p3v1ksg1vyg01238 foreign key (ethernet_interface_id) references ethernet_interface
alter table ethernet_interface_ipv6_static_address add constraint FK_e474vy1s8c5uk4pp9w08ioul0 foreign key (ethernet_interface_id) references ethernet_interface
alter table ethernet_interface_manager add constraint FK_8gp4j6l1o2fupptmvroccuajh foreign key (manager_id) references manager
alter table ethernet_interface_manager add constraint FK_mutac7v6nf5rqktqva4g4cqos foreign key (ethernet_interface_id) references ethernet_interface
alter table ethernet_interface_name_server add constraint FK_mfve44159eukp4rfsxtk9wkqt foreign key (ethernet_interface_id) references ethernet_interface
alter table ethernet_interface_pcie_functions add constraint FK_tjrn2q5thdqmqaumrn8a8o2he foreign key (pcie_function_id) references pcie_device_function
alter table ethernet_interface_pcie_functions add constraint FK_g5158uccil9t0kkvewhuxhotr foreign key (ethernet_interface_id) references ethernet_interface
alter table ethernet_switch add constraint FK_n5jyrct0lcdxo4irimihnymjv foreign key (chassis_id) references chassis
alter table ethernet_switch add constraint FK_6rkxinq1qe60ks1g5sninouf7 foreign key (id) references discoverable_entity
alter table ethernet_switch_manager add constraint FK_shgg8cmud8i2834kkw1nq5mrp foreign key (manager_id) references manager
alter table ethernet_switch_manager add constraint FK_kpnqccbgvhhulnpc3bvk78qil foreign key (ethernet_switch_id) references ethernet_switch
alter table ethernet_switch_port add constraint FK_14oc3d5wv6m3e9ptni5kolwj7 foreign key (ethernet_switch_id) references ethernet_switch
alter table ethernet_switch_port add constraint FK_bvfx8t6rxlm45t5o6jdg4jqfo foreign key (member_of_port_id) references ethernet_switch_port
alter table ethernet_switch_port add constraint FK_ex12fvee2h96igx56fnookgb foreign key (primary_vlan_id) references ethernet_switch_port_vlan
alter table ethernet_switch_port add constraint FK_gv0tspm65f7sfuc8pgipel0s8 foreign key (id) references discoverable_entity
alter table ethernet_switch_port_ipv4_address add constraint FK_jvip9l3yi28b8ph3shfhnmb9k foreign key (ethernet_interface_id) references ethernet_switch_port
alter table ethernet_switch_port_ipv6_address add constraint FK_s2uqypdah5o9lt29yeop303p0 foreign key (ethernet_interface_id) references ethernet_switch_port
alter table ethernet_switch_port_vlan add constraint FK_qummguchfcuwyfj0yt95hf3by foreign key (ethernet_interface_id) references ethernet_interface
alter table ethernet_switch_port_vlan add constraint FK_llscecvwe3wcu265sxbwjfko0 foreign key (ethernet_switch_port_id) references ethernet_switch_port
alter table ethernet_switch_port_vlan add constraint FK_8ejp883if3pjm176b5cic3o2o foreign key (id) references discoverable_entity
alter table event_subscription_event_type add constraint FK_e29kasrkyv5lnwpme7fx63ho2 foreign key (event_type_id) references event_subscription
alter table event_subscription_origin_resources add constraint FK_pjagh089dpf4shsjnl2v6r6dd foreign key (event_type_id) references event_subscription
alter table fabric add constraint FK_4uvir8mle70pabjldo72m198f foreign key (id) references discoverable_entity
alter table fan add constraint FK_sk353jd3cuhlkwiqhe3akq0qe foreign key (thermal_zone_id) references thermal_zone
alter table graphical_console_connect_type_supported add constraint FK_5v1h7ihd2032vxkk8m6j7c8y2 foreign key (graphical_console_id) references manager
alter table logical_drive add constraint FK_runrctve8699q12fuobya7tt6 foreign key (composed_node_id) references composed_node
alter table logical_drive add constraint FK_bnj8wxrgpt62nypubnll5ex5l foreign key (master_drive_id) references logical_drive
alter table logical_drive add constraint FK_owbo3qa7a5syc1r4hgid5vcgn foreign key (storage_service_id) references storage_service
alter table logical_drive add constraint FK_mss1x51uw62d1ef8py0n5g7hl foreign key (id) references discoverable_entity
alter table logical_drive_logical_drive add constraint FK_5l40h92wemulbrbbmrej2uvfm foreign key (used_logical_drive_id) references logical_drive
alter table logical_drive_logical_drive add constraint FK_s75x7febkac6ffypvvp39cnt8 foreign key (used_by_logical_drive_id) references logical_drive
alter table manager add constraint FK_bieak6xoeha76j3uwbpbwm62q foreign key (in_chassis_manager_id) references chassis
alter table manager add constraint FK_dk9y35huoo6qkm48fj2n3s1kb foreign key (id) references discoverable_entity
alter table memory add constraint FK_61vlwhnkrir23pfwe7y6wd50s foreign key (computer_system_id) references computer_system
alter table memory add constraint FK_imr6buoy6xv7iok8k16u7auxi foreign key (id) references discoverable_entity
alter table memory_allowed_speed_mhz add constraint FK_bow00nbqvlr62p8kaavcuhh87 foreign key (computer_system_id) references memory
alter table memory_function_class add constraint FK_3nb1yms0sa0oetx1ik71mrw0v foreign key (memory_id) references memory
alter table memory_memory_media add constraint FK_aici2r35qam1hts069n333uj2 foreign key (memory_id) references memory
alter table memory_operating_memory_mode add constraint FK_ignnqj24680s8o0xevv1xkygm foreign key (memory_id) references memory
alter table memory_region add constraint FK_iu7ep5ud63j6vcxgjcww8njgg foreign key (memory_id) references memory
alter table network_device_function add constraint FK_jchhhhbmfa9rly6mf7nrw5tf9 foreign key (network_interface_id) references network_interface
alter table network_device_function add constraint FK_dmc4vvphx2kk2t9riycu1vtq4 foreign key (id) references discoverable_entity
alter table network_interface add constraint FK_5dft6yhrjq4d3c7gegqmwiqkb foreign key (computer_system_id) references computer_system
alter table network_interface add constraint FK_6crdctyjvdt7tx5iuy74uwdp1 foreign key (id) references discoverable_entity
alter table network_protocol add constraint FK_ou0yf3ra32tcuoji832ngbx4s foreign key (manager_id) references manager
alter table network_protocol add constraint FK_h2mlxihk0iqt3vou43jc8m7pi foreign key (id) references discoverable_entity
alter table pcie_device add constraint FK_2sbbiyxjjodac86ppkpma75tq foreign key (computer_system_id) references computer_system
alter table pcie_device add constraint FK_btgk9gcjgrc308q7af4pu237d foreign key (id) references discoverable_entity
alter table pcie_device_function add constraint FK_4ubawk4c8x2ttldy44aomr0oe foreign key (computer_system_id) references computer_system
alter table pcie_device_function add constraint FK_qeyudqcco39drbfdsr53j1s6u foreign key (pcie_device_id) references pcie_device
alter table pcie_device_function add constraint FK_ogvc8drh5vmc13gp8pwo2fab8 foreign key (id) references discoverable_entity
alter table physical_drive add constraint FK_d5hw89brg12ft25cp6v5k6k5h foreign key (storage_service_id) references storage_service
alter table physical_drive add constraint FK_k3nl7raayfpq6jy7tmpg4n1vt foreign key (id) references discoverable_entity
alter table physical_drive_logical_drive add constraint FK_8q5kh22qj2x7leq2j08k813e1 foreign key (logical_drive_id) references logical_drive
alter table physical_drive_logical_drive add constraint FK_1g0qhj42vc35x3rygmkm9oyoe foreign key (physical_drive_id) references physical_drive
alter table port add constraint FK_il9vsjj7uakouf5c7driq2v9m foreign key (fabric_switch_id) references switch
alter table port add constraint FK_dvgc32pm231nt5p6hnjjpbo9s foreign key (id) references discoverable_entity
alter table port_allowable_reset_type add constraint FK_ljleq3dp6jq5onsyhoen0ohhg foreign key (port_id) references port
alter table port_endpoints add constraint FK_qt24utrq6w1fneq7ympc890ds foreign key (port_id) references endpoint
alter table port_endpoints add constraint FK_g26g8vqn8b6r636bdubesic6a foreign key (endpoint_id) references port
alter table port_pcie_connection_id add constraint FK_ol24bw8ag6yr0x3hi6a468nbk foreign key (port_id) references port
alter table power add constraint FK_1iq8rrn5hvpo8qwd98isygt6a foreign key (chassis_id) references chassis
alter table power add constraint FK_w5rxk3lri25wp7k41d9rg9x7 foreign key (id) references discoverable_entity
alter table power_control add constraint FK_l36j7mc9uduv71xk113kh3yj2 foreign key (power_id) references power
alter table power_control add constraint FK_e5qrc7kxnoy09l42a4hw64anm foreign key (id) references discoverable_entity
alter table power_supply add constraint FK_t0y85vi413s5jeq2qt139msqv foreign key (power_id) references power
alter table power_supply add constraint FK_c9hg7p1v8qjeyooephti9k022 foreign key (id) references discoverable_entity
alter table power_supply_input_range add constraint FK_p80ruyp4iy1vxoetuwo780gtb foreign key (power_supply_id) references power_supply
alter table power_voltage add constraint FK_mplja81rtyg4gn970pexw1ml9 foreign key (power_id) references power
alter table power_voltage add constraint FK_m4ur7rxv21lm8fv0osc3aro7f foreign key (id) references discoverable_entity
alter table power_zone add constraint FK_nxxm14qyk4j643hohsre7seme foreign key (chassis_id) references chassis
alter table power_zone add constraint FK_p8kfb5c2fd7ohjy0391ifa7nc foreign key (id) references discoverable_entity
alter table power_zone_power_supply add constraint FK_8jwnhw1t82vm17vmcm69jhb foreign key (power_zone_id) references power_zone
alter table processor add constraint FK_c0ryxm4p5bj9737gvef5kglfw foreign key (computer_system_id) references computer_system
alter table processor add constraint FK_2tcqjwetvafjrncwxdgfxqmg0 foreign key (id) references discoverable_entity
alter table processor_capability add constraint FK_kseg4fqdmhk3w58imcvikarn1 foreign key (processor_id) references processor
alter table redundancy add constraint FK_p2xhovnbncw8tsr2mpdnnegim foreign key (redundancy_owner_id) references discoverable_entity
alter table redundancy add constraint FK_g95xs7iwq1vwj17rreeamxlip foreign key (id) references discoverable_entity
alter table redundancy_member add constraint FK_tf9dqivigi9clulp696ivvg86 foreign key (member_id) references redundancy
alter table redundancy_member add constraint FK_br8sw85ec61iippj8x61lmi5o foreign key (redundancy_id) references discoverable_entity
alter table remote_target add constraint FK_13j3lmubat3y8i1tbd6hoyx2a foreign key (composed_node_id) references composed_node
alter table remote_target add constraint FK_kjors7w0udpsd0sriv87oeyyh foreign key (remote_target_metadata_id) references remote_target_metadata
alter table remote_target add constraint FK_mo625282dhb25rpmprd3ik8f4 foreign key (storage_service_id) references storage_service
alter table remote_target add constraint FK_g9h49t4vt7ss6mv9ptigip0df foreign key (id) references discoverable_entity
alter table remote_target_iscsi_address add constraint FK_fixy2uancxoloy9xkm839x8e9 foreign key (remote_target_id) references remote_target
alter table remote_target_iscsi_address_target_lun add constraint FK_grywh0gpm1a978qthghs6i8by foreign key (remote_target_iscsi_address_id) references remote_target_iscsi_address
alter table remote_target_logical_drive add constraint FK_jkvc746rnaiikp1inwpogavks foreign key (logical_drive_id) references logical_drive
alter table remote_target_logical_drive add constraint FK_cqcvvhxamudutbh59389ygnw4 foreign key (remote_target_id) references remote_target
alter table serial_console_connect_type_supported add constraint FK_l2dih1yqlnjj3m7ugbdv72ewm foreign key (serial_console_id) references manager
alter table simple_storage add constraint FK_3863hyymhay9xrguda7x76inw foreign key (computer_system_id) references computer_system
alter table simple_storage add constraint FK_pcbos2cmcqofu5aii39b9713x foreign key (id) references discoverable_entity
alter table simple_storage_device add constraint FK_3hw06r8lalse8hhqska4u25px foreign key (simple_storage_id) references simple_storage
alter table storage add constraint FK_ea0hs48jmmlouy8ku36smxvoy foreign key (chassis_id) references chassis
alter table storage add constraint FK_9d9s1f6ryg37k1u44w1elucb8 foreign key (computer_system_id) references computer_system
alter table storage add constraint FK_303v22f6u27remio9g3wkjgs4 foreign key (id) references discoverable_entity
alter table storage_controller add constraint FK_n69yhp10frad0v71617exy1ot foreign key (storage_id) references storage
alter table storage_controller add constraint FK_h4ywc7mg92b5qxytlch0g6h9r foreign key (storage_adapter_id) references storage
alter table storage_controller_identifiers add constraint FK_m6mdi4ruiy10hbk75ahojq2g foreign key (storage_controller_id) references storage_controller
alter table storage_controller_pcie_functions add constraint FK_k5i97hjkt2ysfx4ks7ehu1lgt foreign key (pcie_function_id) references pcie_device_function
alter table storage_controller_pcie_functions add constraint FK_95pct13enurtbbpw2oaxf4ebt foreign key (ethernet_interface_id) references storage_controller
alter table storage_controller_supported_controller_protocol add constraint FK_nsmsvtjba9gln1t5snr9bguyg foreign key (storage_controller_id) references storage_controller
alter table storage_controller_supported_device_protocol add constraint FK_902q8gdwv8jpfvlrisvmg61vn foreign key (storage_controller_id) references storage_controller
alter table storage_service add constraint FK_jqmsra7bcla6voptd2dgvethj foreign key (id) references discoverable_entity
alter table storage_service_manager add constraint FK_kl8lw8718nq4qamryjb7mw4cr foreign key (manager_id) references manager
alter table storage_service_manager add constraint FK_a5pw738ejj33n0al7d0s3t6sa foreign key (storage_service_id) references storage_service
alter table switch add constraint FK_hrjog9vjw78fsq9lf7e4ni4hv foreign key (fabric_id) references fabric
alter table switch add constraint FK_6xxu8f9txm6ub9qrh1um7k44y foreign key (id) references discoverable_entity
alter table switch_allowable_reset_type add constraint FK_jewchglyx24hsif1k1kqeb55b foreign key (switch_id) references switch
alter table switch_manager add constraint FK_pa97j31fvr8pa3b5rvkekws4s foreign key (manager_id) references manager
alter table switch_manager add constraint FK_l3kgrc0kls4fvm3bgmcwo4i5m foreign key (switch_id) references switch
alter table temperature add constraint FK_55hrg11ep5t9573h6hvsjtuur foreign key (thermal_zone_id) references thermal_zone
alter table thermal add constraint FK_g9dk9xsm6vix174n6bx6d9smj foreign key (chassis_id) references chassis
alter table thermal add constraint FK_hgrnb6ky9jlfbhacmq386s4fk foreign key (id) references discoverable_entity
alter table thermal_fan add constraint FK_4pip60vb2oqar3wub3joioj8m foreign key (thermal_id) references thermal
alter table thermal_fan add constraint FK_5lvlfhlxtary3auouk03jld2 foreign key (id) references discoverable_entity
alter table thermal_temperature add constraint FK_ewo2wh7k7qd67u1uwn8o7t7sy foreign key (thermal_id) references thermal
alter table thermal_temperature add constraint FK_49qdcbh7rro8iww9pxvukxqb3 foreign key (id) references discoverable_entity
alter table thermal_zone add constraint FK_mqic2fiqbyvhxr4njcf6gg65g foreign key (chassis_id) references chassis
alter table thermal_zone add constraint FK_ptyknu64nt9dk080dt3ffohwo foreign key (id) references discoverable_entity
alter table unknown_oems add constraint FK_amsotndnf16qgy715jb1jj55f foreign key (entity_id) references discoverable_entity
alter table zone add constraint FK_1qx30aoet3jq3k8y2ffyoi5al foreign key (fabric_id) references fabric
alter table zone add constraint FK_7jhic4whhk83teqpjvv0c0h8t foreign key (id) references discoverable_entity
alter table zone_switches add constraint FK_tcxwa8ius9i0rkej6ukagn3ns foreign key (zone_id) references zone
alter table zone_switches add constraint FK_hva3ulpyx4l694bcdnbtqpx9k foreign key (switch_id) references switch
