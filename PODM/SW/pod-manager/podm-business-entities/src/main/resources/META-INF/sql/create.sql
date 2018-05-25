create table associated_endpoint_identifier (
	associated_endpoint_identifier_id int8 not null,
	durable_name text,
	durable_name_format text
);

create table associated_volume_identifier (
	associated_volume_identifier_id int8 not null,
	durable_name text,
	durable_name_format text
);

create table boot_boot_source_override_mode_supported (
	boot_id int8 not null,
	boot_source_override_mode_supported text,
	boot_source_override_mode_supported_order int4 not null,
	primary key (boot_id,boot_source_override_mode_supported_order)
);

create table boot_boot_source_override_supported (
	boot_id int8 not null,
	boot_source_override_supported text,
	boot_source_override_supported_order int4 not null,
	primary key (boot_id,boot_source_override_supported_order)
);

create table capacity_source (
	entity_id text,
	data_allocated_bytes numeric,
	data_consumed_bytes numeric,
	data_guaranteed_bytes numeric,
	data_provisioned_bytes numeric,
	is_thin_provisioned boolean,
	metadata_allocated_bytes numeric,
	metadata_consumed_bytes numeric,
	metadata_guaranteed_bytes numeric,
	metadata_provisioned_bytes numeric,
	snapshot_allocated_bytes numeric,
	snapshot_consumed_bytes numeric,
	snapshot_guaranteed_bytes numeric,
	snapshot_provisioned_bytes numeric,
	id int8 not null,
	storage_pool_id int8,
	volume_id int8,
	primary key (id)
);

create table chassis (
	asset_tag text,
	chassis_type text,
	entity_id text,
	indicator_led text,
	location_id text,
	location_parent_id text,
	manufacturer text,
	model text,
	part_number text,
	power_state text,
	geo_tag text,
	rack_supports_disaggregated_power_cooling boolean,
	rack_uuid text,
	serial_number text,
	sku text,
	id int8 not null,
	parent_chassis_id int8,
	primary key (id)
);

create table chassis_computer_system (
	chassis_id int8 not null,
	computer_system_id int8 not null,
	primary key (chassis_id,computer_system_id)
);

create table chassis_fabric_switch (
	chassis_id int8 not null,
	fabric_switch_id int8 not null,
	primary key (chassis_id,fabric_switch_id)
);

create table chassis_manager (
	chassis_id int8 not null,
	manager_id int8 not null,
	primary key (chassis_id,manager_id)
);

create table chassis_pcie_device (
	chassis_id int8 not null,
	pcie_device_id int8 not null,
	primary key (chassis_id,pcie_device_id)
);

create table chassis_power (
	chassis_id int8 not null,
	power_id int8 not null,
	primary key (chassis_id,power_id)
);

create table chassis_thermal (
	chassis_id int8 not null,
	thermal_id int8 not null,
	primary key (chassis_id,thermal_id)
);

create table command_shell_connect_type_supported (
	command_shell_id int8 not null,
	connect_type_supported text,
	connect_type_supported_order int4 not null,
	primary key (command_shell_id,connect_type_supported_order)
);

create table composed_node (
	id  bigserial not null,
	event_source_context text,
	version integer DEFAULT 0 not null,
	associated_compute_service_uuid text,
	associated_computer_system_uuid text,
	associated_storage_service_uuid text,
	clear_tpm_on_delete boolean,
	composed_node_state text,
	description text,
	eligible_for_recovery boolean,
	entity_id bigserial not null,
	name text,
	number_of_requested_drives int4,
	prior_untagged_vlan_id int4,
	remote_drive_capacity_gib numeric,
	status text,
	computer_system_id int8,
	primary key (id)
);

create table computer_system (
	asset_tag text,
	bios_version text,
	boot_source_override_enabled text,
	boot_source_override_mode text,
	boot_source_override_target text,
	discovery_state text,
	entity_id text,
	host_name text,
	indicator_led text,
	manufacturer text,
	memory_sockets int4,
	memory_summary_status text,
	memory_summary_total_system_memory_gib numeric,
	model text,
	part_number text,
	power_state text,
	processor_sockets int4,
	processor_summary_count int4,
	processor_summary_model text,
	processor_summary_status text,
	serial_number text,
	sku text,
	system_type text,
	trusted_execution_technology_enabled boolean,
	user_mode_enabled boolean,
	uuid text,
	id int8 not null,
	computer_system_metadata_id int8,
	primary key (id)
);

create table computer_system_allowable_interface_type (
	computer_system_id int8 not null,
	allowable_interface_type text,
	allowable_interface_type_order int4 not null,
	primary key (computer_system_id,allowable_interface_type_order)
);

create table computer_system_allowable_reset_type (
	computer_system_id int8 not null,
	allowable_reset_type text,
	allowable_reset_type_order int4 not null,
	primary key (computer_system_id,allowable_reset_type_order)
);

create table computer_system_hosting_role (
	computer_system_id int8 not null,
	hosting_role text,
	hosting_role_order int4 not null,
	primary key (computer_system_id,hosting_role_order)
);

create table computer_system_manager (
	computer_system_id int8 not null,
	manager_id int8 not null,
	primary key (computer_system_id,manager_id)
);

create table computer_system_metadata (
	id  bigserial not null,
	event_source_context text,
	version integer DEFAULT 0 not null,
	allocated boolean,
	deep_discovery_state text,
	task_uuid text,
	primary key (id)
);

create table computer_system_metrics (
	entity_id text,
	io_bandwidth_gbps numeric,
	memory_bandwidth_percent numeric,
	memory_power_watt numeric,
	memory_throttled_cycles_percent numeric,
	processor_bandwidth_percent numeric,
	processor_power_watt numeric,
	id int8 not null,
	computer_system_id int8,
	primary key (id)
);

create table computer_system_metrics_health (
	computer_system_metrics_id int8 not null,
	health text,
	health_order int4 not null,
	primary key (computer_system_metrics_id,health_order)
);

create table computer_system_pci_device (
	computer_system_id int8 not null,
	device_id text,
	vendor_id text,
	pci_device_order int4 not null,
	primary key (computer_system_id,pci_device_order)
);

create table computer_system_pcie_connection_id (
	computer_system_id int8 not null,
	pcie_connection_id text,
	pcie_connection_id_order int4 not null,
	primary key (computer_system_id,pcie_connection_id_order)
);

create table computer_system_trusted_module (
	computer_system_id int8 not null,
	firmware_version text,
	firmware_version_2 text,
	interface_type text,
	interface_type_selection text,
	status text,
	trusted_module_order int4 not null,
	primary key (computer_system_id,trusted_module_order)
);

create table connected_entity (
	entity_id text,
	entity_role text,
	pci_class_code text,
	pci_function_number int4,
	device_id text,
	subsystem_id text,
	subsystem_vendor_id text,
	vendor_id text,
	id int8 not null,
	endpoint_id int8,
	discoverable_entity_id int8,
	primary key (id)
);

create table connected_entity_identifier (
	connected_entity_id int8 not null,
	durable_name text,
	durable_name_format text
);

create table dcbx_shared_configuration (
	id  bigserial not null,
	event_source_context text,
	version integer DEFAULT 0 not null,
	entity_id text,
	ethernet_switch_id int8,
	primary key (id)
);

create table dcbx_shared_configuration_application_protocol (
	application_protocol_id int8 not null,
	port int4,
	priority int4,
	protocol text,
	application_protocol_order int4 not null,
	primary key (application_protocol_id,application_protocol_order)
);

create table dcbx_shared_configuration_bandwidth_allocation (
	bandwidth_allocation_id int8 not null,
	bandwidth_percent int4,
	priority_group int4,
	bandwidth_allocation_order int4 not null,
	primary key (bandwidth_allocation_id,bandwidth_allocation_order)
);

create table dcbx_shared_configuration_priority_to_priority_group_mapping (
	priority_to_priority_group_mapping_id int8 not null,
	priority int4,
	priority_group int4,
	priority_to_priority_group_mapping_order int4 not null,
	primary key (priority_to_priority_group_mapping_id,priority_to_priority_group_mapping_order)
);

create table discoverable_entity (
	discriminator_class text not null,
	id  bigserial not null,
	event_source_context text,
	version integer DEFAULT 0 not null,
	description text,
	global_id text,
	is_complementary boolean,
	name text,
	status text,
	primary key (id)
);

create table drive (
	asset_tag text,
	block_size_bytes int4,
	capable_speed_gbs numeric,
	capacity_bytes int8,
	drive_erased boolean,
	encryption_ability text,
	encryption_status text,
	entity_id text,
	erase_on_detach boolean,
	failure_predicted boolean,
	firmware_version text,
	hotspare_type text,
	indicator_led text,
	manufacturer text,
	media_type text,
	model text,
	multi_source_discriminator text,
	negotiated_speed_gbs numeric,
	part_number text,
	predicted_media_life_left_percent numeric,
	protocol text,
	revision text,
	rotation_speed_rpm numeric,
	serial_number text,
	sku text,
	status_indicator text,
	id int8 not null,
	chassis_id int8,
	composed_node_id int8,
	drive_metadata_id int8,
	drive_metrics_id int8,
	pcie_device_function_id int8,
	storage_id int8,
	storage_service_id int8,
	volume_id int8,
	primary key (id)
);

create table drive_capacity_sources (
	drive_id int8 not null,
	capacity_source_id int8 not null,
	primary key (drive_id,capacity_source_id)
);

create table drive_identifier (
	drive_id int8 not null,
	durable_name text,
	durable_name_format text,
	identifier_order int4 not null,
	primary key (drive_id,identifier_order)
);

create table drive_location (
	drive_id int8 not null,
	info text,
	info_format text,
	location_order int4 not null,
	primary key (drive_id,location_order)
);

create table drive_metadata (
	id  bigserial not null,
	event_source_context text,
	version integer DEFAULT 0 not null,
	allocated boolean,
	primary key (id)
);

create table drive_metrics (
	entity_id text,
	available_spare_percentage numeric,
	media_errors numeric,
	predicted_media_life_used_percent numeric,
	unsafe_shutdowns numeric,
	controller_busy_time_minutes numeric,
	host_read_commands numeric,
	host_write_commands numeric,
	power_cycles numeric,
	power_on_hours numeric,
	unit_size_bytes int8,
	units_read numeric,
	units_written numeric,
	temperature_kelvin numeric,
	id int8 not null,
	drive_id int8,
	primary key (id)
);

create table endpoint (
	endpoint_authentication_password text,
	endpoint_authentication_username text,
	entity_id text,
	host_reservation_memory_bytes int4,
	device_id text,
	subsystem_id text,
	subsystem_vendor_id text,
	vendor_id text,
	endpoint_protocol text,
	id int8 not null,
	composed_node_id int8,
	computer_system_id int8,
	fabric_id int8,
	endpoint_metadata_id int8,
	processor_id int8,
	storage_service_id int8,
	zone_id int8,
	primary key (id)
);

create table endpoint_identifier (
	endpoint_id int8 not null,
	durable_name text,
	durable_name_format text
);

create table endpoint_metadata (
	id  bigserial not null,
	event_source_context text,
	version integer DEFAULT 0 not null,
	allocated boolean,
	primary key (id)
);

create table entity_related_item (
	item_owner_id int8 not null,
	item_id int8 not null,
	primary key (item_owner_id,item_id)
);

create table ethernet_interface (
	auto_neg boolean,
	entity_id text,
	fqdn text,
	full_duplex boolean,
	hostname text,
	interface_enabled boolean,
	ipv6_default_gateway text,
	link_status text,
	mac_address text,
	max_ipv6_static_addresses int4,
	mtu_size int4,
	multi_source_discriminator text,
	permanent_mac_address text,
	speed_mbps int4,
	vlan_enable boolean,
	vlan_id int4,
	id int8 not null,
	computer_system_id int8,
	primary key (id)
);

create table ethernet_interface_endpoint (
	ethernet_interface_id int8 not null,
	endpoint_id int8 not null,
	primary key (ethernet_interface_id,endpoint_id)
);

create table ethernet_interface_ipv4_address (
	ethernet_interface_id int8 not null,
	address text,
	address_origin text,
	gateway text,
	oem text,
	subnet_mask text,
	ipv4_address_order int4 not null,
	primary key (ethernet_interface_id,ipv4_address_order)
);

create table ethernet_interface_ipv6_address (
	ethernet_interface_id int8 not null,
	address text,
	address_origin text,
	address_state text,
	oem text,
	prefix_length int4,
	ipv6_address_order int4 not null,
	primary key (ethernet_interface_id,ipv6_address_order)
);

create table ethernet_interface_ipv6_address_policy (
	ethernet_interface_id int8 not null,
	label int4,
	precedence int4,
	prefix text,
	ipv6_address_policy_order int4 not null,
	primary key (ethernet_interface_id,ipv6_address_policy_order)
);

create table ethernet_interface_ipv6_static_address (
	ethernet_interface_id int8 not null,
	address text,
	address_origin text,
	address_state text,
	oem text,
	prefix_length int4,
	ipv6_static_address_order int4 not null,
	primary key (ethernet_interface_id,ipv6_static_address_order)
);

create table ethernet_interface_manager (
	ethernet_interface_id int8 not null,
	manager_id int8 not null,
	primary key (ethernet_interface_id,manager_id)
);

create table ethernet_interface_name_server (
	ethernet_interface_id int8 not null,
	name_server text,
	name_server_order int4 not null,
	primary key (ethernet_interface_id,name_server_order)
);

create table ethernet_interface_pcie_function (
	ethernet_interface_id int8 not null,
	pcie_function_id int8 not null,
	primary key (ethernet_interface_id,pcie_function_id)
);

create table ethernet_interface_supported_protocol (
	ethernet_interface_id int8 not null,
	supported_protocol text,
	supported_protocol_order int4 not null,
	primary key (ethernet_interface_id,supported_protocol_order)
);

create table ethernet_switch (
	dcbx_enabled boolean,
	entity_id text,
	ets_enabled boolean,
	firmware_name text,
	firmware_version text,
	lldp_enabled boolean,
	manufacturer text,
	manufacturing_date text,
	model text,
	part_number text,
	pfc_enabled boolean,
	role text,
	serial_number text,
	switch_id text,
	id int8 not null,
	chassis_id int8,
	ethernet_switch_metrics_id int8,
	primary key (id)
);

create table ethernet_switch_acl (
	entity_id text,
	id int8 not null,
	ethernet_switch_id int8,
	primary key (id)
);

create table ethernet_switch_acl_bind_action_allowable_value_port (
	acl_id int8 not null,
	port_id int8 not null,
	primary key (acl_id,port_id)
);

create table ethernet_switch_acl_bound_port (
	acl_id int8 not null,
	port_id int8 not null,
	primary key (acl_id,port_id)
);

create table ethernet_switch_acl_rule (
	action text,
	destination_ip_v4_address text,
	destination_ip_mask text,
	source_ip_v4_address text,
	source_ip_mask text,
	destination_port_mask int8,
	destination_port int8,
	l4_protocol int8,
	source_port_mask int8,
	source_port int8,
	destination_mac_address text,
	destination_mac_mask text,
	source_mac_address text,
	source_mac_mask text,
	vlan_id text,
	vlan_mask text,
	entity_id text,
	mirror_type text,
	rule_id int8,
	id int8 not null,
	ethernet_switch_acl_id int8,
	ethernet_switch_acl_rule_forward_mirror_interface_id int8,
	primary key (id)
);

create table ethernet_switch_acl_rule_mirror_port_region (
	rule_id int8 not null,
	port_id int8 not null,
	primary key (rule_id,port_id)
);

create table ethernet_switch_manager (
	ethernet_switch_id int8 not null,
	manager_id int8 not null,
	primary key (ethernet_switch_id,manager_id)
);

create table ethernet_switch_metrics (
	entity_id text,
	health text,
	id int8 not null,
	ethernet_switch_id int8,
	primary key (id)
);

create table ethernet_switch_port (
	administrative_state text,
	autosense boolean,
	dcbx_state text,
	entity_id text,
	frame_size int4,
	full_duplex boolean,
	link_speed_gbps int4,
	link_type text,
	lldp_enabled boolean,
	mac_address text,
	neighbor_info text,
	neighbor_mac text,
	operational_state text,
	port_class text,
	port_id text,
	port_mode text,
	port_type text,
	priority_flow_control_enabled boolean,
	id int8 not null,
	ethernet_switch_id int8,
	member_of_port_id int8,
	ethernet_switch_port_metrics_id int8,
	primary_vlan_id int8,
	primary key (id)
);

create table ethernet_switch_port_enabled_priorities (
	ethernet_switch_port_id int8 not null,
	enabled_priorities int4,
	enabled_priorities_order int4 not null,
	primary key (ethernet_switch_port_id,enabled_priorities_order)
);

create table ethernet_switch_port_ipv4_address (
	ethernet_switch_port_id int8 not null,
	address text,
	address_origin text,
	gateway text,
	oem text,
	subnet_mask text,
	ipv4_address_order int4 not null,
	primary key (ethernet_switch_port_id,ipv4_address_order)
);

create table ethernet_switch_port_ipv6_address (
	ethernet_switch_port_id int8 not null,
	address text,
	address_origin text,
	address_state text,
	oem text,
	prefix_length int4,
	ipv6_address_order int4 not null,
	primary key (ethernet_switch_port_id,ipv6_address_order)
);

create table ethernet_switch_port_metrics (
	collisions int8,
	entity_id text,
	received_broadcast_packets int8,
	received_bytes int8,
	received_dropped_packets int8,
	received_error_packets int8,
	received_errors int8,
	received_multicast_packets int8,
	received_packets int8,
	transmitted_broadcast_packets int8,
	transmitted_bytes int8,
	transmitted_dropped_packets int8,
	transmitted_error_packets int8,
	transmitted_errors int8,
	transmitted_multicast_packets int8,
	transmitted_packets int8,
	id int8 not null,
	ethernet_switch_port_id int8,
	primary key (id)
);

create table ethernet_switch_port_vlan (
	entity_id text,
	tagged boolean,
	vlan_enable boolean,
	vlan_id int4,
	id int8 not null,
	ethernet_interface_id int8,
	ethernet_switch_port_id int8,
	primary key (id)
);

create table ethernet_switch_static_mac (
	entity_id text,
	mac_address text,
	vlan_id int4,
	id int8 not null,
	ethernet_switch_port_id int8,
	primary key (id)
);

create table event_subscription (
	id  bigserial not null,
	event_source_context text,
	version integer DEFAULT 0 not null,
	description text,
	destination text,
	entity_id bigserial not null,
	name text,
	protocol text,
	subscription_context text,
	primary key (id)
);

create table event_subscription_event_type (
	event_subscription_id int8 not null,
	event_type text,
	event_type_order int4 not null,
	primary key (event_subscription_id,event_type_order)
);

create table event_subscription_origin_resource (
	event_subscription_id int8 not null,
	origin_resource text,
	origin_resource_order int4 not null,
	primary key (event_subscription_id,origin_resource_order)
);

create table external_link (
	id  bigserial not null,
	event_source_context text,
	version integer DEFAULT 0 not null,
	uri text,
	discoverable_entity_id int8,
	external_service_id int8,
	primary key (id)
);

create table external_service (
	id  bigserial not null,
	event_source_context text,
	version integer DEFAULT 0 not null,
	uri text,
	entity_id bigserial not null,
	is_eventing_available boolean,
	is_complementary_data_source boolean,
	is_reachable boolean,
	service_type text,
	unreachable_since timestamp,
	uuid text,
	primary key (id)
);

create table fabric (
	entity_id text,
	fabric_type text,
	max_zones int4,
	id int8 not null,
	primary key (id)
);

create table graphical_console_connect_type_supported (
	graphical_console_id int8 not null,
	connect_type_supported text,
	connect_type_supported_order int4 not null,
	primary key (graphical_console_id,connect_type_supported_order)
);

create table ip_transport_details (
	entity_id text,
	ipv4_address text,
	ipv4_address_origin text,
	ipv4_gateway text,
	ipv4_oem text,
	ipv4_subnet_mask text,
	ipv6_address text,
	ipv6_address_origin text,
	address_state text,
	ipv6_oem text,
	prefix_length int4,
	port int4,
	transport_protocol text,
	id int8 not null,
	endpoint_id int8,
	primary key (id)
);

create table manager (
	command_shell_concurrent_sessions int4,
	command_shell_service_enabled boolean,
	date_time text,
	date_time_local_offset text,
	entity_id text,
	firmware_version text,
	graphical_console_max_concurrent_sessions int4,
	graphical_console_service_enabled boolean,
	manager_type text,
	model text,
	power_state text,
	serial_console_max_concurrent_sessions int4,
	serial_console_service_enabled boolean,
	service_entry_point_uuid text,
	uuid text,
	id int8 not null,
	in_chassis_manager_id int8,
	primary key (id)
);

create table memory (
	base_module_type text,
	bus_width_bits int4,
	capacity_mib int4,
	data_width_bits int4,
	device_id text,
	device_locator text,
	entity_id text,
	error_correction text,
	firmware_api_version text,
	firmware_revision text,
	manufacturer text,
	memory_device_type text,
	channel int4,
	memory_controller int4,
	slot int4,
	socket int4,
	memory_type text,
	multi_source_discriminator text,
	operating_speed_mhz int4,
	part_number text,
	rank_count int4,
	serial_number text,
	vendor_id text,
	voltage_volt numeric,
	id int8 not null,
	computer_system_id int8,
	primary key (id)
);

create table memory_allowed_speed_mhz (
	memory_id int8 not null,
	allowed_speed_mhz int4,
	allowed_speed_mhz_order int4 not null,
	primary key (memory_id,allowed_speed_mhz_order)
);

create table memory_function_class (
	memory_id int8 not null,
	function_class text,
	function_class_order int4 not null,
	primary key (memory_id,function_class_order)
);

create table memory_memory_media (
	memory_id int8 not null,
	memory_media text,
	memory_media_order int4 not null,
	primary key (memory_id,memory_media_order)
);

create table memory_metrics (
	bandwidth_percent numeric,
	block_size_bytes int8,
	consumed_power_watt numeric,
	current_period_blocks_read int8,
	current_period_blocks_written int8,
	ecc_errors_count int8,
	entity_id text,
	address_parity_error boolean,
	correctable_ecc_error boolean,
	spare_block boolean,
	temperature boolean,
	uncorrectable_ecc_error boolean,
	data_loss_detected boolean,
	last_shutdown_success boolean,
	performance_degraded boolean,
	remaining_spare_block_percentage numeric,
	life_time_blocks_read int8,
	life_time_blocks_written int8,
	temperature_celsius numeric,
	thermal_margin_celsius numeric,
	throttled_cycles_percent numeric,
	id int8 not null,
	memory_id int8,
	primary key (id)
);

create table memory_metrics_health (
	memory_metrics_id int8 not null,
	health text,
	health_order int4 not null,
	primary key (memory_metrics_id,health_order)
);

create table memory_operating_memory_mode (
	memory_id int8 not null,
	operating_memory_mode text,
	operating_memory_mode_order int4 not null,
	primary key (memory_id,operating_memory_mode_order)
);

create table memory_region (
	memory_id int8 not null,
	memory_type text,
	offset_mib int4,
	region_id text,
	size_mib int4,
	memory_region_order int4 not null,
	primary key (memory_id,memory_region_order)
);

create table metric_definition (
	accuracy numeric,
	calculable text,
	calculation_algorithm text,
	calculation_precision float8,
	calculation_time_interval text,
	calibration numeric,
	data_type text,
	discrete_metric_type text,
	entity_id text,
	implementation text,
	is_linear boolean,
	max_reading_range numeric,
	metric_type text,
	min_reading_range numeric,
	physical_context text,
	precision int4,
	sensing_interval text,
	sensor_type text,
	time_stamp_accuracy text,
	units text,
	id int8 not null,
	metric_item_id int8,
	primary key (id)
);

create table metric_definition_calculation_parameters (
	metric_definition_id int8 not null,
	result_metric text,
	source_metric text,
	calculation_parameters_values_order int4 not null,
	primary key (metric_definition_id,calculation_parameters_values_order)
);

create table metric_definition_discrete_values (
	metric_definition_id int8 not null,
	discrete_values text,
	discrete_values_order int4 not null,
	primary key (metric_definition_id,discrete_values_order)
);

create table metric_definition_metric_properties (
	metric_definition_id int8 not null,
	metric_properties text,
	metric_properties_order int4 not null,
	primary key (metric_definition_id,metric_properties_order)
);

create table metric_definition_wildcard (
	id  bigserial not null,
	event_source_context text,
	version integer DEFAULT 0 not null,
	name text,
	metric_definition_wildcard_id int8,
	primary key (id)
);

create table metric_definition_wildcard_value (
	metric_definition_wildcard_id int8 not null,
	metric_definition_wildcard_value text,
	metric_definition_wildcard_value_order int4 not null,
	primary key (metric_definition_wildcard_id,metric_definition_wildcard_value_order)
);

create table metric_item (
	entity_id text,
	metric_value text,
	id int8 not null,
	metric_report_definition_id int8,
	primary key (id)
);

create table metric_report_definition (
	entity_id text,
	metric_report_destination text,
	metric_report_type text,
	polling_interval_milli_seconds numeric,
	report_interval_milli_seconds numeric,
	transmit_format text,
	id int8 not null,
	primary key (id)
);

create table metric_report_definition_metric_report_action (
	metric_report_definition_id int8 not null,
	metric_report_action text,
	metric_report_action_order int4 not null,
	primary key (metric_report_definition_id,metric_report_action_order)
);

create table network_device_function (
	device_enabled boolean,
	entity_id text,
	mac_address text,
	authentication_method text,
	chap_username text,
	initiator_default_gateway text,
	initiator_ip_address text,
	initiator_name text,
	initiator_netmask text,
	ip_address_type text,
	ip_mask_dns_via_dhcp boolean,
	mutual_chap_username text,
	primary_dns text,
	primary_lun int4,
	primary_target_ip_address text,
	primary_target_name text,
	primary_target_tcp_port int4,
	primary_vlan_enable boolean,
	primary_vlan_id int4,
	router_advertisement_enabled boolean,
	secondary_dns text,
	secondary_lun int4,
	secondary_target_ip_address text,
	secondary_target_name text,
	secondary_target_tcp_port int4,
	secondary_vlan_enable boolean,
	secondary_vlan_id int4,
	target_info_via_dhcp boolean,
	id int8 not null,
	network_interface_id int8,
	primary key (id)
);

create table network_interface (
	entity_id text,
	id int8 not null,
	computer_system_id int8,
	primary key (id)
);

create table network_protocol (
	entity_id text,
	fqdn text,
	hostname text,
	http_port int4,
	http_protocol_enabled boolean,
	https_port int4,
	https_protocol_enabled boolean,
	ipmi_port int4,
	ipmi_protocol_enabled boolean,
	kvm_ip_port int4,
	kvm_ip_protocol_enabled boolean,
	snmp_port int4,
	snmp_protocol_enabled boolean,
	ssdp_notify_ipv6_scope text,
	ssdp_notify_multicast_interval_seconds int4,
	ssdp_notify_ttl int4,
	ssdp_port int4,
	ssdp_protocol_enabled boolean,
	ssh_port int4,
	ssh_protocol_enabled boolean,
	telnet_port int4,
	telnet_protocol_enabled boolean,
	virtual_media_port int4,
	virtual_media_protocol_enabled boolean,
	id int8 not null,
	manager_id int8,
	primary key (id)
);

create table pcie_device (
	asset_tag text,
	device_type text,
	entity_id text,
	firmware_version text,
	manufacturer text,
	model text,
	part_number text,
	serial_number text,
	sku text,
	id int8 not null,
	computer_system_id int8,
	primary key (id)
);

create table pcie_device_function (
	class_code text,
	device_class text,
	device_id text,
	entity_id text,
	function_id int4,
	function_type text,
	revision_id text,
	subsystem_id text,
	subsystem_vendor_id text,
	vendor_id text,
	id int8 not null,
	computer_system_id int8,
	pcie_device_id int8,
	primary key (id)
);

create table port (
	current_speed_gbps int8,
	entity_id text,
	max_speed_gbps int8,
	port_id text,
	port_protocol text,
	port_type text,
	width int8,
	id int8 not null,
	fabric_switch_id int8,
	primary key (id)
);

create table port_allowable_reset_type (
	port_id int8 not null,
	allowable_reset_type text,
	allowable_reset_type_order int4 not null,
	primary key (port_id,allowable_reset_type_order)
);

create table port_endpoint (
	port_id int8 not null,
	endpoint_id int8 not null,
	primary key (port_id,endpoint_id)
);

create table port_metrics (
	entity_id text,
	health text,
	id int8 not null,
	port_id int8,
	primary key (id)
);

create table port_pcie_connection_id (
	port_id int8 not null,
	pcie_connection_id text,
	pcie_connection_id_order int4 not null,
	primary key (port_id,pcie_connection_id_order)
);

create table power (
	entity_id text,
	input_ac_power_watts numeric,
	id int8 not null,
	chassis_id int8,
	primary key (id)
);

create table power_control (
	entity_id text,
	member_id text,
	oem text,
	power_allocated_watts numeric,
	power_available_watts numeric,
	power_capacity_watts numeric,
	power_consumed_watts numeric,
	correction_in_ms int4,
	limit_exception text,
	limit_in_watts int4,
	average_consumed_watts numeric,
	interval_in_min int4,
	max_consumed_watts numeric,
	min_consumed_watts numeric,
	power_requested_watts numeric,
	id int8 not null,
	power_id int8,
	primary key (id)
);

create table power_supply (
	entity_id text,
	firmware_version text,
	last_power_output_watts numeric,
	line_input_voltage numeric,
	line_input_voltage_type text,
	manufacturer text,
	member_id text,
	model text,
	oem text,
	part_number text,
	power_capacity_watts numeric,
	power_supply_type text,
	serial_number text,
	spare_part_number text,
	id int8 not null,
	power_id int8,
	primary key (id)
);

create table power_supply_input_range (
	power_supply_id int8 not null,
	input_type text,
	maximum_frequency_hz numeric,
	maximum_voltage numeric,
	minimum_frequency_hz numeric,
	minimum_voltage numeric,
	oem text,
	output_wattage numeric,
	input_range_order int4 not null,
	primary key (power_supply_id,input_range_order)
);

create table power_voltage (
	entity_id text,
	lower_threshold_critical numeric,
	lower_threshold_fatal numeric,
	lower_threshold_non_critical numeric,
	max_reading_range numeric,
	member_id text,
	min_reading_range numeric,
	physical_context text,
	reading_volts numeric,
	sensor_number int4,
	upper_threshold_critical numeric,
	upper_threshold_fatal numeric,
	upper_threshold_non_critical numeric,
	id int8 not null,
	power_id int8,
	primary key (id)
);

create table processor (
	brand text,
	entity_id text,
	extended_identification_registers text,
	bit_stream_version text,
	hssi_configuration text,
	hssi_sideband text,
	reconfiguration_slots int4,
	type text,
	instruction_set text,
	manufacturer text,
	max_speed_mhz int4,
	model text,
	multi_source_discriminator text,
	processor_architecture text,
	effective_family text,
	effective_model text,
	identification_registers text,
	microcode_info text,
	step text,
	vendor_id text,
	processor_type text,
	socket text,
	thermal_design_power_watt numeric,
	total_cores int4,
	total_threads int4,
	id int8 not null,
	computer_system_id int8,
	primary key (id)
);

create table processor_capability (
	processor_id int8 not null,
	capability text,
	capability_order int4 not null,
	primary key (processor_id,capability_order)
);

create table processor_metrics (
	average_frequency_mhz int4,
	bandwidth_percent numeric,
	consumed_power_watt int4,
	entity_id text,
	temperature_celsius int4,
	throttling_celsius int4,
	id int8 not null,
	processor_id int8,
	primary key (id)
);

create table processor_metrics_health (
	processor_metrics_id int8 not null,
	health text,
	health_order int4 not null,
	primary key (processor_metrics_id,health_order)
);

create table processor_on_package_memory (
	processor_id int8 not null,
	capacity_mb int4,
	speed_mhz int4,
	type text,
	on_package_memory_order int4 not null,
	primary key (processor_id,on_package_memory_order)
);

create table redundancy (
	entity_id text,
	max_num_supported int4,
	member_id text,
	min_num_needed int4,
	mode text,
	redundancy_enabled boolean,
	id int8 not null,
	redundancy_owner_id int8,
	primary key (id)
);

create table redundancy_member (
	redundancy_id int8 not null,
	member_id int8 not null,
	primary key (redundancy_id,member_id)
);

create table replica_info (
	entity_id text,
	replica_role text,
	replica_type text,
	id int8 not null,
	replica_id int8,
	volume_id int8,
	primary key (id)
);

create table serial_console_connect_type_supported (
	serial_console_id int8 not null,
	connect_type_supported text,
	connect_type_supported_order int4 not null,
	primary key (serial_console_id,connect_type_supported_order)
);

create table simple_storage (
	entity_id text,
	multi_source_discriminator text,
	uefi_device_path text,
	id int8 not null,
	computer_system_id int8,
	primary key (id)
);

create table simple_storage_device (
	id  bigserial not null,
	event_source_context text,
	version integer DEFAULT 0 not null,
	capacity_bytes numeric,
	entity_id bigserial not null,
	manufacturer text,
	model text,
	name text,
	oem text,
	status text,
	simple_storage_id int8,
	primary key (id)
);

create table storage (
	entity_id text,
	multi_source_discriminator text,
	id int8 not null,
	chassis_id int8,
	computer_system_id int8,
	primary key (id)
);

create table storage_controller (
	asset_tag text,
	entity_id text,
	firmware_version text,
	manufacturer text,
	member_id text,
	model text,
	multi_source_discriminator text,
	part_number text,
	serial_number text,
	sku text,
	speed_gbps numeric,
	id int8 not null,
	storage_id int8,
	storage_adapter_id int8,
	primary key (id)
);

create table storage_controller_identifier (
	storage_controller_id int8 not null,
	durable_name text,
	durable_name_format text,
	identifier_order int4 not null,
	primary key (storage_controller_id,identifier_order)
);

create table storage_controller_pcie_device_function (
	storage_controller_id int8 not null,
	pcie_device_function_id int8 not null,
	primary key (storage_controller_id,pcie_device_function_id)
);

create table storage_controller_supported_controller_protocol (
	storage_controller_id int8 not null,
	supported_controller_protocol text,
	supported_controller_protocol_order int4 not null,
	primary key (storage_controller_id,supported_controller_protocol_order)
);

create table storage_controller_supported_device_protocol (
	storage_controller_id int8 not null,
	supported_device_protocol text,
	supported_device_protocol_order int4 not null,
	primary key (storage_controller_id,supported_device_protocol_order)
);

create table storage_pool (
	block_size_bytes numeric,
	data_allocated_bytes numeric,
	data_consumed_bytes numeric,
	data_guaranteed_bytes numeric,
	data_provisioned_bytes numeric,
	is_thin_provisioned boolean,
	metadata_allocated_bytes numeric,
	metadata_consumed_bytes numeric,
	metadata_guaranteed_bytes numeric,
	metadata_provisioned_bytes numeric,
	snapshot_allocated_bytes numeric,
	snapshot_consumed_bytes numeric,
	snapshot_guaranteed_bytes numeric,
	snapshot_provisioned_bytes numeric,
	entity_id text,
	durable_name text,
	durable_name_format text,
	id int8 not null,
	storage_pool_id int8,
	storage_service_id int8,
	primary key (id)
);

create table storage_pool_composed_node (
	storage_pool_id int8 not null,
	composed_node_id int8 not null,
	primary key (storage_pool_id,composed_node_id)
);

create table storage_providing_pool_capacity_source (
	storage_pool_id int8 not null,
	capacity_source_id int8 not null,
	primary key (storage_pool_id,capacity_source_id)
);

create table storage_service (
	entity_id text,
	id int8 not null,
	computer_system_id int8,
	primary key (id)
);

create table storage_service_manager (
	storage_service_id int8 not null,
	manager_id int8 not null,
	primary key (storage_service_id,manager_id)
);

create table switch (
	asset_tag text,
	domain_id int4,
	entity_id text,
	indicator_led text,
	is_managed boolean,
	manufacturer text,
	model text,
	part_number text,
	power_state text,
	serial_number text,
	sku text,
	switch_type text,
	total_switch_width int4,
	id int8 not null,
	fabric_id int8,
	primary key (id)
);

create table switch_allowable_reset_type (
	switch_id int8 not null,
	allowable_reset_type text,
	allowable_reset_type_order int4 not null,
	primary key (switch_id,allowable_reset_type_order)
);

create table switch_manager (
	switch_id int8 not null,
	manager_id int8 not null,
	primary key (switch_id,manager_id)
);

create table switch_zone (
	switch_id int8 not null,
	zone_id int8 not null,
	primary key (switch_id,zone_id)
);

create table thermal (
	desired_speed_pwm int4,
	entity_id text,
	volumetric_air_flow_cfm int4,
	id int8 not null,
	chassis_id int8,
	primary key (id)
);

create table thermal_fan (
	entity_id text,
	lower_threshold_critical int4,
	lower_threshold_fatal int4,
	lower_threshold_non_critical int4,
	max_reading_range int4,
	member_id text,
	min_reading_range int4,
	physical_context text,
	reading int4,
	reading_units text,
	upper_threshold_critical int4,
	upper_threshold_fatal int4,
	upper_threshold_non_critical int4,
	id int8 not null,
	thermal_id int8,
	primary key (id)
);

create table thermal_temperature (
	entity_id text,
	lower_threshold_critical numeric,
	lower_threshold_fatal numeric,
	lower_threshold_non_critical numeric,
	max_reading_range_temp numeric,
	member_id text,
	min_reading_range_temp numeric,
	physical_context text,
	reading_celsius numeric,
	sensor_number int4,
	upper_threshold_critical numeric,
	upper_threshold_fatal numeric,
	upper_threshold_non_critical numeric,
	id int8 not null,
	thermal_id int8,
	primary key (id)
);

create table unknown_oem (
	entity_id int8 not null,
	oem_path text,
	oem_value text,
	unknown_oem_order int4 not null,
	primary key (entity_id,unknown_oem_order)
);

create table volume (
	block_size_bytes numeric,
	bootable boolean,
	data_allocated_bytes numeric,
	data_consumed_bytes numeric,
	data_guaranteed_bytes numeric,
	data_provisioned_bytes numeric,
	is_thin_provisioned boolean,
	metadata_allocated_bytes numeric,
	metadata_consumed_bytes numeric,
	metadata_guaranteed_bytes numeric,
	metadata_provisioned_bytes numeric,
	snapshot_allocated_bytes numeric,
	snapshot_consumed_bytes numeric,
	snapshot_guaranteed_bytes numeric,
	snapshot_provisioned_bytes numeric,
	capacity_bytes numeric,
	encrypted boolean,
	encryption_types text,
	entity_id text,
	erase_on_detach boolean,
	erased boolean,
	initialize_action_supported boolean,
	manufacturer text,
	model text,
	optimum_io_size_bytes numeric,
	volume_type text,
	id int8 not null,
	composed_node_id int8,
	volume_metadata_id int8,
	volume_metrics_id int8,
	storage_pool_id int8,
	storage_service_id int8,
	primary key (id)
);

create table volume_access_capability (
	volume_id int8 not null,
	access_capability text,
	access_capability_order int4 not null,
	primary key (volume_id,access_capability_order)
);

create table volume_allowable_initialize_type (
	volume_id int8 not null,
	allowable_initialize_type text,
	allowable_initialize_type_order int4 not null,
	primary key (volume_id,allowable_initialize_type_order)
);

create table volume_identifier (
	volume_id int8 not null,
	durable_name text,
	durable_name_format text
);

create table volume_metadata (
	id  bigserial not null,
	event_source_context text,
	version integer DEFAULT 0 not null,
	allocated boolean,
	primary key (id)
);

create table volume_metrics (
	capacity_used_bytes int8,
	entity_id text,
	id int8 not null,
	volume_id int8,
	primary key (id)
);

create table volume_operation (
	volume_id int8 not null,
	operation_name text,
	percentage_complete numeric,
	operation_order int4 not null,
	primary key (volume_id,operation_order)
);

create table zone (
	entity_id text,
	id int8 not null,
	fabric_id int8,
	primary key (id)
);

alter table capacity_source
	add constraint idx_capacity_source_entity_id unique (entity_id);

alter table chassis
	add constraint idx_chassis_entity_id unique (entity_id);

alter table composed_node
	add constraint idx_composed_node_entity_id unique (entity_id);

alter table computer_system
	add constraint idx_computer_system_entity_id unique (entity_id);

alter table computer_system_metrics
	add constraint idx_computer_system_metrics_entity_id unique (entity_id);

alter table connected_entity
	add constraint idx_connected_entity_entity_id unique (entity_id);

alter table dcbx_shared_configuration
	add constraint idx_dcbx_shared_configuration_entity_id unique (entity_id);

alter table discoverable_entity
	add constraint idx_discoverable_entity_global_id unique (global_id);

alter table drive
	add constraint idx_drive_entity_id unique (entity_id);

alter table drive_metrics
	add constraint idx_drive_metrics_entity_id unique (entity_id);

alter table endpoint
	add constraint idx_endpoint_entity_id unique (entity_id);

alter table ethernet_interface
	add constraint idx_ethernet_interface_entity_id unique (entity_id);

alter table ethernet_switch
	add constraint idx_ethernet_switch_entity_id unique (entity_id);

alter table ethernet_switch_acl
	add constraint idx_ethernet_switch_acl_entity_id unique (entity_id);

alter table ethernet_switch_acl_rule
	add constraint idx_ethernet_switch_acl_rule_entity_id unique (entity_id);

alter table ethernet_switch_metrics
	add constraint idx_ethernet_switch_metrics_entity_id unique (entity_id);

alter table ethernet_switch_port
	add constraint idx_ethernet_switch_port_entity_id unique (entity_id);

alter table ethernet_switch_port_metrics
	add constraint idx_ethernet_switch_port_metrics_entity_id unique (entity_id);

alter table ethernet_switch_port_vlan
	add constraint idx_ethernet_switch_port_vlan_entity_id unique (entity_id);

alter table ethernet_switch_static_mac
	add constraint idx_ethernet_switch_static_mac_entity_id unique (entity_id);

alter table event_subscription
	add constraint idx_event_subscription_entity_id unique (entity_id);

alter table external_service
	add constraint idx_external_service_entity_id unique (entity_id);

alter table external_service
	add constraint idx_external_service_uuid unique (uuid);

alter table fabric
	add constraint idx_fabric_entity_id unique (entity_id);

alter table ip_transport_details
	add constraint idx_transport_entity_id unique (entity_id);

alter table manager
	add constraint idx_manager_entity_id unique (entity_id);

alter table memory
	add constraint idx_memory_entity_id unique (entity_id);

alter table memory_metrics
	add constraint idx_memory_metrics_entity_id unique (entity_id);

alter table metric_definition
	add constraint idx_metric_definition_entity_id unique (entity_id);

alter table metric_item
	add constraint idx_metric_item_entity_id unique (entity_id);

alter table metric_report_definition
	add constraint idx_metric_report_definition_entity_id unique (entity_id);

alter table network_device_function
	add constraint idx_network_device_function_entity_id unique (entity_id);

alter table network_interface
	add constraint idx_network_interface_entity_id unique (entity_id);

alter table network_protocol
	add constraint idx_network_protocol_entity_id unique (entity_id);

alter table pcie_device
	add constraint idx_pcie_device_entity_id unique (entity_id);

alter table pcie_device_function
	add constraint idx_pcie_device_function_entity_id unique (entity_id);

alter table port
	add constraint idx_port_entity_id unique (entity_id);

alter table port_metrics
	add constraint idx_port_metrics_entity_id unique (entity_id);

alter table power
	add constraint idx_power_entity_id unique (entity_id);

alter table power_control
	add constraint idx_power_control_entity_id unique (entity_id);

alter table power_supply
	add constraint idx_power_supply_id unique (entity_id);

alter table power_voltage
	add constraint idx_power_voltage_entity_id unique (entity_id);

alter table processor
	add constraint idx_processor_entity_id unique (entity_id);

alter table processor_metrics
	add constraint idx_processor_metrics_entity_id unique (entity_id);

alter table redundancy
	add constraint idx_redundancy_entity_id unique (entity_id);

alter table replica_info
	add constraint idx_replica_info_entity_id unique (entity_id);

alter table simple_storage
	add constraint idx_simple_storage_entity_id unique (entity_id);

alter table simple_storage_device
	add constraint idx_simple_storage_device_entity_id unique (entity_id);

alter table storage
	add constraint idx_storage_entity_id unique (entity_id);

alter table storage_controller
	add constraint idx_storage_controller_entity_id unique (entity_id);

alter table storage_pool
	add constraint idx_storage_pool_entity_id unique (entity_id);

alter table storage_service
	add constraint idx_storage_service_entity_id unique (entity_id);

alter table switch
	add constraint idx_switch_entity_id unique (entity_id);

alter table thermal
	add constraint idx_thermal_entity_id unique (entity_id);

alter table thermal_fan
	add constraint idx_thermal_fan_entity_id unique (entity_id);

alter table thermal_temperature
	add constraint idx_thermal_temperature_entity_id unique (entity_id);

alter table volume
	add constraint idx_volume_entity_id unique (entity_id);

alter table volume_metrics
	add constraint idx_volume_metrics_entity_id unique (entity_id);

alter table zone
	add constraint idx_zone_entity_id unique (entity_id);

alter table associated_endpoint_identifier
	add constraint FK_abcs4hjgtqfod5lhl0ts5734l foreign key (associated_endpoint_identifier_id)references composed_node;

alter table associated_volume_identifier
	add constraint FK_qv26wu9iyl0e77bop68ujkn0s foreign key (associated_volume_identifier_id)references composed_node;

alter table boot_boot_source_override_mode_supported
	add constraint FK_5s5sope9k484hvv3ujcbbw108 foreign key (boot_id)references computer_system;

alter table boot_boot_source_override_supported
	add constraint FK_i8r660cenjyj95upx9kwp6in8 foreign key (boot_id)references computer_system;

alter table capacity_source
	add constraint FK_df9w9ff128xegljoat4bamr64 foreign key (storage_pool_id)references storage_pool;

alter table capacity_source
	add constraint FK_aid7k5fb4mit5x55r5y3imblg foreign key (volume_id)references volume;

alter table capacity_source
	add constraint FK_fybc548xnj1ugmfle7xceib3u foreign key (id)references discoverable_entity;

alter table chassis
	add constraint FK_fdaesv3uw07hhfk9rlmaynxbp foreign key (parent_chassis_id)references chassis;

alter table chassis
	add constraint FK_rn2i9y69jbo0ta2046abdtj8u foreign key (id)references discoverable_entity;

alter table chassis_computer_system
	add constraint FK_hseban2kwly4brj8wyrrqvey0 foreign key (computer_system_id)references computer_system;

alter table chassis_computer_system
	add constraint FK_t2f44367kc2tylbabdtjska31 foreign key (chassis_id)references chassis;

alter table chassis_fabric_switch
	add constraint FK_anp0qu78n8tyijjr97covl9jp foreign key (fabric_switch_id)references switch;

alter table chassis_fabric_switch
	add constraint FK_hn0xonsw2n12ttcg8uja39r6a foreign key (chassis_id)references chassis;

alter table chassis_manager
	add constraint FK_h8h298vwc0mrybdblrgarol22 foreign key (manager_id)references manager;

alter table chassis_manager
	add constraint FK_kr1h0ayly65i5t3tsqea3v34 foreign key (chassis_id)references chassis;

alter table chassis_pcie_device
	add constraint FK_bwbmyt4g5627wfilvxygxsiqr foreign key (pcie_device_id)references pcie_device;

alter table chassis_pcie_device
	add constraint FK_e71b8tftjlqno48so21oibbwt foreign key (chassis_id)references chassis;

alter table chassis_power
	add constraint FK_qgbayic9ghdlh1is10qohaehe foreign key (power_id)references power;

alter table chassis_power
	add constraint FK_f3s9d57jamfvvmftrwy8ltf2d foreign key (chassis_id)references chassis;

alter table chassis_thermal
	add constraint FK_89mu803x1kvx4fop6vo9y68r0 foreign key (thermal_id)references thermal;

alter table chassis_thermal
	add constraint FK_pk4gl23ftyguwebpqshn018p3 foreign key (chassis_id)references chassis;

alter table command_shell_connect_type_supported
	add constraint FK_p27rh6q8nu69iohinyk1pxl51 foreign key (command_shell_id)references manager;

alter table composed_node
	add constraint FK_6dvurerir9p7quf3b4bq6vq0t foreign key (computer_system_id)references computer_system;

alter table computer_system
	add constraint FK_md6awm92wh5cq8dmsgwkdbpy1 foreign key (computer_system_metadata_id)references computer_system_metadata;

alter table computer_system
	add constraint FK_cwca10t6xao4h4cdqea0dxita foreign key (id)references discoverable_entity;

alter table computer_system_allowable_interface_type
	add constraint FK_5u1agd9dqwspbsp9wmbb01pmf foreign key (computer_system_id)references computer_system;

alter table computer_system_allowable_reset_type
	add constraint FK_q1iv5xwi4hqrxlwwy374amwth foreign key (computer_system_id)references computer_system;

alter table computer_system_hosting_role
	add constraint FK_bs4ncoynlxi642yu5d0aphuv3 foreign key (computer_system_id)references computer_system;

alter table computer_system_manager
	add constraint FK_5qjqbrmviflp7dgyh3imnvkaw foreign key (manager_id)references manager;

alter table computer_system_manager
	add constraint FK_9gbeankygenoq464djmovjy0c foreign key (computer_system_id)references computer_system;

alter table computer_system_metrics
	add constraint FK_mx1yfnnpp1dk6o1h1sw333q41 foreign key (computer_system_id)references computer_system;

alter table computer_system_metrics
	add constraint FK_5wcc9t543vevexv7v5f6guq43 foreign key (id)references discoverable_entity;

alter table computer_system_metrics_health
	add constraint FK_7hmw73nl7atl7vqpyrg5lqho2 foreign key (computer_system_metrics_id)references computer_system_metrics;

alter table computer_system_pci_device
	add constraint FK_2du70hhi6tao2ht9yv0110si9 foreign key (computer_system_id)references computer_system;

alter table computer_system_pcie_connection_id
	add constraint FK_csh08v7q5vjxvgt2csgwb9fp6 foreign key (computer_system_id)references computer_system;

alter table computer_system_trusted_module
	add constraint FK_m8q9osn0tsbcgi7uwpgns6pf3 foreign key (computer_system_id)references computer_system;

alter table connected_entity
	add constraint FK_17a70jj241lk1okb0brdu5m44 foreign key (endpoint_id)references endpoint;

alter table connected_entity
	add constraint FK_1ik8qkv4ffnby88eylp3dd2ua foreign key (discoverable_entity_id)references discoverable_entity;

alter table connected_entity
	add constraint FK_trradgh56rj8ff6v04rsk94ug foreign key (id)references discoverable_entity;

alter table connected_entity_identifier
	add constraint FK_bqbj5ls5dmxlt4ka5q4unyjty foreign key (connected_entity_id)references connected_entity;

alter table dcbx_shared_configuration
	add constraint FK_i9mddhagkj3466wnd6vx6rasx foreign key (ethernet_switch_id)references ethernet_switch;

alter table dcbx_shared_configuration_application_protocol
	add constraint FK_tqbpo1hx5hqbpeswac5kfgld0 foreign key (application_protocol_id)references dcbx_shared_configuration;

alter table dcbx_shared_configuration_bandwidth_allocation
	add constraint FK_mcyx7yutfr27tl45epoa1edpa foreign key (bandwidth_allocation_id)references dcbx_shared_configuration;

alter table dcbx_shared_configuration_priority_to_priority_group_mapping
	add constraint FK_dqmkwstnia0edsw9h1e0qn24 foreign key (priority_to_priority_group_mapping_id)references dcbx_shared_configuration;

alter table drive
	add constraint FK_jnpr25lir5pn6sct8eu2sdhjf foreign key (chassis_id)references chassis;

alter table drive
	add constraint FK_hg2i3dcoyr30041ams83q6k1c foreign key (composed_node_id)references composed_node;

alter table drive
	add constraint FK_rnn8cx4x4xma4clftuqi3boy4 foreign key (drive_metadata_id)references drive_metadata;

alter table drive
	add constraint FK_cocim0qwrt1tttxfqilcfb4bb foreign key (drive_metrics_id)references drive_metrics;

alter table drive
	add constraint FK_stwmvhg01t9xvrifgeo7en331 foreign key (pcie_device_function_id)references pcie_device_function;

alter table drive
	add constraint FK_k0edqaee47me4mjoffv3poh77 foreign key (storage_id)references storage;

alter table drive
	add constraint FK_59a786omqyycd76jpp7ja4ca4 foreign key (storage_service_id)references storage_service;

alter table drive
	add constraint FK_o8ffpxxn9aqpniay6ji6rn3yx foreign key (volume_id)references volume;

alter table drive
	add constraint FK_k7uvdl7n7gcdpko42179klcvl foreign key (id)references discoverable_entity;

alter table drive_capacity_sources
	add constraint FK_e7yv3le6m73gwgmlgkspb7ebe foreign key (capacity_source_id)references capacity_source;

alter table drive_capacity_sources
	add constraint FK_bakj3sgb2gecwjkpsrqhce9kp foreign key (drive_id)references drive;

alter table drive_identifier
	add constraint FK_acbq14nesbx37ryswgjdodyim foreign key (drive_id)references drive;

alter table drive_location
	add constraint FK_dlrx37ioi7c79f0irlrgeokp7 foreign key (drive_id)references drive;

alter table drive_metrics
	add constraint FK_196fmgn7e6ce2s96cte14ne70 foreign key (drive_id)references drive;

alter table drive_metrics
	add constraint FK_7g05613h8akgva6mtclmewvn7 foreign key (id)references discoverable_entity;

alter table endpoint
	add constraint FK_gt4bk9ytuwb0lyt44d78jj0s7 foreign key (composed_node_id)references composed_node;

alter table endpoint
	add constraint FK_6p15jxa7qa2u2iylyig3n4sx4 foreign key (computer_system_id)references computer_system;

alter table endpoint
	add constraint FK_ofmdqutkvp6sw3g905brjxuhv foreign key (fabric_id)references fabric;

alter table endpoint
	add constraint FK_p7teicxqruuhp6jj6e6mfuv17 foreign key (endpoint_metadata_id)references endpoint_metadata;

alter table endpoint
	add constraint FK_e75i55djxc9qmmb3lvokf67qv foreign key (processor_id)references processor;

alter table endpoint
	add constraint FK_abj470s1wq95lixg45iro1lyh foreign key (storage_service_id)references storage_service;

alter table endpoint
	add constraint FK_s3uwsvug4plk1dpprwdln4smr foreign key (zone_id)references zone;

alter table endpoint
	add constraint FK_7j9v08qwbrpl4wxrjog3cy1u7 foreign key (id)references discoverable_entity;

alter table endpoint_identifier
	add constraint FK_bkav1mey33701095ndikid2tb foreign key (endpoint_id)references endpoint;

alter table entity_related_item
	add constraint FK_ar1ktaq5uw056lf1pv85yrhwy foreign key (item_id)references discoverable_entity;

alter table entity_related_item
	add constraint FK_f4j4c1anuccj7vumd99g8isjx foreign key (item_owner_id)references discoverable_entity;

alter table ethernet_interface
	add constraint FK_bm8wr6mkdvjpxuvqqpr0iobq2 foreign key (computer_system_id)references computer_system;

alter table ethernet_interface
	add constraint FK_36pt3bdrw0ew81twg7n0kltg7 foreign key (id)references discoverable_entity;

alter table ethernet_interface_endpoint
	add constraint FK_a79h1utxwn29r7oljhatav94s foreign key (endpoint_id)references endpoint;

alter table ethernet_interface_endpoint
	add constraint FK_wb9wkddiltnfev1phs8q44rj foreign key (ethernet_interface_id)references ethernet_interface;

alter table ethernet_interface_ipv4_address
	add constraint FK_ien8cwxqp5dfsgvhsvrefivow foreign key (ethernet_interface_id)references ethernet_interface;

alter table ethernet_interface_ipv6_address
	add constraint FK_3uxtn8e1kx25nrm0f0eubxyov foreign key (ethernet_interface_id)references ethernet_interface;

alter table ethernet_interface_ipv6_address_policy
	add constraint FK_mqaxrr7p0p3v1ksg1vyg01238 foreign key (ethernet_interface_id)references ethernet_interface;

alter table ethernet_interface_ipv6_static_address
	add constraint FK_e474vy1s8c5uk4pp9w08ioul0 foreign key (ethernet_interface_id)references ethernet_interface;

alter table ethernet_interface_manager
	add constraint FK_8gp4j6l1o2fupptmvroccuajh foreign key (manager_id)references manager;

alter table ethernet_interface_manager
	add constraint FK_mutac7v6nf5rqktqva4g4cqos foreign key (ethernet_interface_id)references ethernet_interface;

alter table ethernet_interface_name_server
	add constraint FK_mfve44159eukp4rfsxtk9wkqt foreign key (ethernet_interface_id)references ethernet_interface;

alter table ethernet_interface_pcie_function
	add constraint FK_8dd4w9fxpqttjetorhw8uickl foreign key (pcie_function_id)references pcie_device_function;

alter table ethernet_interface_pcie_function
	add constraint FK_mkv517r2miq436km475hrtohw foreign key (ethernet_interface_id)references ethernet_interface;

alter table ethernet_interface_supported_protocol
	add constraint FK_ce9it6x1r3qaxvdilh4x3e32q foreign key (ethernet_interface_id)references ethernet_interface;

alter table ethernet_switch
	add constraint FK_n5jyrct0lcdxo4irimihnymjv foreign key (chassis_id)references chassis;

alter table ethernet_switch
	add constraint FK_2sthwuoca7eyygxnk71b3755k foreign key (ethernet_switch_metrics_id)references ethernet_switch_metrics;

alter table ethernet_switch
	add constraint FK_6rkxinq1qe60ks1g5sninouf7 foreign key (id)references discoverable_entity;

alter table ethernet_switch_acl
	add constraint FK_6ami74nv9m48xl0nbpy0fmf0j foreign key (ethernet_switch_id)references ethernet_switch;

alter table ethernet_switch_acl
	add constraint FK_kjc9dob8p5p81q56cnayeq97 foreign key (id)references discoverable_entity;

alter table ethernet_switch_acl_bind_action_allowable_value_port
	add constraint FK_pg4kxrnx8aqdp24t7ykffh6uw foreign key (port_id)references ethernet_switch_port;

alter table ethernet_switch_acl_bind_action_allowable_value_port
	add constraint FK_i5m82oplb6ayw6ydp9vvvrkf9 foreign key (acl_id)references ethernet_switch_acl;

alter table ethernet_switch_acl_bound_port
	add constraint FK_evny3xgkwdr881tixgfp4n8g2 foreign key (port_id)references ethernet_switch_port;

alter table ethernet_switch_acl_bound_port
	add constraint FK_ci7vuu9b41xjl7921dffyr4gi foreign key (acl_id)references ethernet_switch_acl;

alter table ethernet_switch_acl_rule
	add constraint FK_i04bd5ofepgknj35pn132lyqs foreign key (ethernet_switch_acl_id)references ethernet_switch_acl;

alter table ethernet_switch_acl_rule
	add constraint FK_l4h5f6f74du6tu4dxhm5infb foreign key (ethernet_switch_acl_rule_forward_mirror_interface_id)references ethernet_switch_port;

alter table ethernet_switch_acl_rule
	add constraint FK_niu6cuhxv3yt39vrr7nidcsn2 foreign key (id)references discoverable_entity;

alter table ethernet_switch_acl_rule_mirror_port_region
	add constraint FK_2rewgf49c47snnf6hvsvrcu5r foreign key (port_id)references ethernet_switch_port;

alter table ethernet_switch_acl_rule_mirror_port_region
	add constraint FK_5m22x571ofjt67qi5luu2xw65 foreign key (rule_id)references ethernet_switch_acl_rule;

alter table ethernet_switch_manager
	add constraint FK_shgg8cmud8i2834kkw1nq5mrp foreign key (manager_id)references manager;

alter table ethernet_switch_manager
	add constraint FK_kpnqccbgvhhulnpc3bvk78qil foreign key (ethernet_switch_id)references ethernet_switch;

alter table ethernet_switch_metrics
	add constraint FK_rcxvcpbgli4y27lhl4seqgphm foreign key (ethernet_switch_id)references ethernet_switch;

alter table ethernet_switch_metrics
	add constraint FK_4sef4brey1llmwk00sjuqgu8y foreign key (id)references discoverable_entity;

alter table ethernet_switch_port
	add constraint FK_14oc3d5wv6m3e9ptni5kolwj7 foreign key (ethernet_switch_id)references ethernet_switch;

alter table ethernet_switch_port
	add constraint FK_bvfx8t6rxlm45t5o6jdg4jqfo foreign key (member_of_port_id)references ethernet_switch_port;

alter table ethernet_switch_port
	add constraint FK_7nl60dcw3haow1yknes58k7f2 foreign key (ethernet_switch_port_metrics_id)references ethernet_switch_port_metrics;

alter table ethernet_switch_port
	add constraint FK_ex12fvee2h96igx56fnookgb foreign key (primary_vlan_id)references ethernet_switch_port_vlan;

alter table ethernet_switch_port
	add constraint FK_gv0tspm65f7sfuc8pgipel0s8 foreign key (id)references discoverable_entity;

alter table ethernet_switch_port_enabled_priorities
	add constraint FK_he8biy9u6h7fpobtgm9rte0lj foreign key (ethernet_switch_port_id)references ethernet_switch_port;

alter table ethernet_switch_port_ipv4_address
	add constraint FK_ckb3epk3ljfj2g3l22gh51ogg foreign key (ethernet_switch_port_id)references ethernet_switch_port;

alter table ethernet_switch_port_ipv6_address
	add constraint FK_5l9noh9gou2qb2ctad88kc98f foreign key (ethernet_switch_port_id)references ethernet_switch_port;

alter table ethernet_switch_port_metrics
	add constraint FK_e5c0p11db21idlexrglvcj724 foreign key (ethernet_switch_port_id)references ethernet_switch_port;

alter table ethernet_switch_port_metrics
	add constraint FK_ijlkjvkfbulb7gkth0i41os78 foreign key (id)references discoverable_entity;

alter table ethernet_switch_port_vlan
	add constraint FK_qummguchfcuwyfj0yt95hf3by foreign key (ethernet_interface_id)references ethernet_interface;

alter table ethernet_switch_port_vlan
	add constraint FK_llscecvwe3wcu265sxbwjfko0 foreign key (ethernet_switch_port_id)references ethernet_switch_port;

alter table ethernet_switch_port_vlan
	add constraint FK_8ejp883if3pjm176b5cic3o2o foreign key (id)references discoverable_entity;

alter table ethernet_switch_static_mac
	add constraint FK_1dhlbfa4tkh97wc92ourpt5g4 foreign key (ethernet_switch_port_id)references ethernet_switch_port;

alter table ethernet_switch_static_mac
	add constraint FK_24dbvbd17vyepinnap8xfy90 foreign key (id)references discoverable_entity;

alter table event_subscription_event_type
	add constraint FK_7y5kn8fyehga5nh7yf8k1upga foreign key (event_subscription_id)references event_subscription;

alter table event_subscription_origin_resource
	add constraint FK_eqa0pq8pi7l3no4rmx7vqtgfh foreign key (event_subscription_id)references event_subscription;

alter table external_link
	add constraint FK_3vm4lelx90hpxm92vosj692mx foreign key (discoverable_entity_id)references discoverable_entity;

alter table external_link
	add constraint FK_jh4jq158uiewbq3p7ch1umxxc foreign key (external_service_id)references external_service;

alter table fabric
	add constraint FK_4uvir8mle70pabjldo72m198f foreign key (id)references discoverable_entity;

alter table graphical_console_connect_type_supported
	add constraint FK_5v1h7ihd2032vxkk8m6j7c8y2 foreign key (graphical_console_id)references manager;

alter table ip_transport_details
	add constraint FK_qa9v1pip5eqpwwkkk497yvhi1 foreign key (endpoint_id)references endpoint;

alter table ip_transport_details
	add constraint FK_lv4ye0wvy8v0aopppbd8ueb1o foreign key (id)references discoverable_entity;

alter table manager
	add constraint FK_bieak6xoeha76j3uwbpbwm62q foreign key (in_chassis_manager_id)references chassis;

alter table manager
	add constraint FK_dk9y35huoo6qkm48fj2n3s1kb foreign key (id)references discoverable_entity;

alter table memory
	add constraint FK_61vlwhnkrir23pfwe7y6wd50s foreign key (computer_system_id)references computer_system;

alter table memory
	add constraint FK_imr6buoy6xv7iok8k16u7auxi foreign key (id)references discoverable_entity;

alter table memory_allowed_speed_mhz
	add constraint FK_15kb5125q2pfj914u2oedix09 foreign key (memory_id)references memory;

alter table memory_function_class
	add constraint FK_3nb1yms0sa0oetx1ik71mrw0v foreign key (memory_id)references memory;

alter table memory_memory_media
	add constraint FK_aici2r35qam1hts069n333uj2 foreign key (memory_id)references memory;

alter table memory_metrics
	add constraint FK_5cc1pd96qdpyf5ctxs964ulgs foreign key (memory_id)references memory;

alter table memory_metrics
	add constraint FK_au1kfiaak5icymifyejga6r1p foreign key (id)references discoverable_entity;

alter table memory_metrics_health
	add constraint FK_50wftsio76n8f124mcdb95536 foreign key (memory_metrics_id)references memory_metrics;

alter table memory_operating_memory_mode
	add constraint FK_ignnqj24680s8o0xevv1xkygm foreign key (memory_id)references memory;

alter table memory_region
	add constraint FK_iu7ep5ud63j6vcxgjcww8njgg foreign key (memory_id)references memory;

alter table metric_definition
	add constraint FK_9a7rk2dltegmsly0irrfs1105 foreign key (metric_item_id)references metric_item;

alter table metric_definition
	add constraint FK_df3hjy96bp6hvu4u2t8s7vtkj foreign key (id)references discoverable_entity;

alter table metric_definition_calculation_parameters
	add constraint FK_6tp7jo3963l7o46bvp4ys93lp foreign key (metric_definition_id)references metric_definition;

alter table metric_definition_discrete_values
	add constraint FK_rb7xw2f97tt1lj1s3n0eclsne foreign key (metric_definition_id)references metric_definition;

alter table metric_definition_metric_properties
	add constraint FK_23rmvmrt0mscxa5jwfruhd54k foreign key (metric_definition_id)references metric_definition;

alter table metric_definition_wildcard
	add constraint FK_9vn4v8ogprlngb6t3imcn0uda foreign key (metric_definition_wildcard_id)references metric_definition;

alter table metric_definition_wildcard_value
	add constraint FK_dnoro2s63k6eaw2lb9tutvwlo foreign key (metric_definition_wildcard_id)references metric_definition_wildcard;

alter table metric_item
	add constraint FK_l7nh7vnhx5c4u9bcnfb0rd2ma foreign key (metric_report_definition_id)references metric_report_definition;

alter table metric_item
	add constraint FK_hsswjdndnpye49ycfbs3r24mv foreign key (id)references discoverable_entity;

alter table metric_report_definition
	add constraint FK_n94ulnutbyle2e4uunidyqj5g foreign key (id)references discoverable_entity;

alter table metric_report_definition_metric_report_action
	add constraint FK_bl294gd3brggujht394414d2t foreign key (metric_report_definition_id)references metric_report_definition;

alter table network_device_function
	add constraint FK_jchhhhbmfa9rly6mf7nrw5tf9 foreign key (network_interface_id)references network_interface;

alter table network_device_function
	add constraint FK_dmc4vvphx2kk2t9riycu1vtq4 foreign key (id)references discoverable_entity;

alter table network_interface
	add constraint FK_5dft6yhrjq4d3c7gegqmwiqkb foreign key (computer_system_id)references computer_system;

alter table network_interface
	add constraint FK_6crdctyjvdt7tx5iuy74uwdp1 foreign key (id)references discoverable_entity;

alter table network_protocol
	add constraint FK_ou0yf3ra32tcuoji832ngbx4s foreign key (manager_id)references manager;

alter table network_protocol
	add constraint FK_h2mlxihk0iqt3vou43jc8m7pi foreign key (id)references discoverable_entity;

alter table pcie_device
	add constraint FK_2sbbiyxjjodac86ppkpma75tq foreign key (computer_system_id)references computer_system;

alter table pcie_device
	add constraint FK_btgk9gcjgrc308q7af4pu237d foreign key (id)references discoverable_entity;

alter table pcie_device_function
	add constraint FK_4ubawk4c8x2ttldy44aomr0oe foreign key (computer_system_id)references computer_system;

alter table pcie_device_function
	add constraint FK_qeyudqcco39drbfdsr53j1s6u foreign key (pcie_device_id)references pcie_device;

alter table pcie_device_function
	add constraint FK_ogvc8drh5vmc13gp8pwo2fab8 foreign key (id)references discoverable_entity;

alter table port
	add constraint FK_il9vsjj7uakouf5c7driq2v9m foreign key (fabric_switch_id)references switch;

alter table port
	add constraint FK_dvgc32pm231nt5p6hnjjpbo9s foreign key (id)references discoverable_entity;

alter table port_allowable_reset_type
	add constraint FK_ljleq3dp6jq5onsyhoen0ohhg foreign key (port_id)references port;

alter table port_endpoint
	add constraint FK_cqdrxblk501fansv2o90havj6 foreign key (endpoint_id)references endpoint;

alter table port_endpoint
	add constraint FK_hlt8ahyofrovcajtvw4sf2c8n foreign key (port_id)references port;

alter table port_metrics
	add constraint FK_mgjw40cn8x5ixua95rcil21d8 foreign key (port_id)references port;

alter table port_metrics
	add constraint FK_r2vk4b6w4rhjkqeq3jlgndyog foreign key (id)references discoverable_entity;

alter table port_pcie_connection_id
	add constraint FK_ol24bw8ag6yr0x3hi6a468nbk foreign key (port_id)references port;

alter table power
	add constraint FK_1iq8rrn5hvpo8qwd98isygt6a foreign key (chassis_id)references chassis;

alter table power
	add constraint FK_w5rxk3lri25wp7k41d9rg9x7 foreign key (id)references discoverable_entity;

alter table power_control
	add constraint FK_l36j7mc9uduv71xk113kh3yj2 foreign key (power_id)references power;

alter table power_control
	add constraint FK_e5qrc7kxnoy09l42a4hw64anm foreign key (id)references discoverable_entity;

alter table power_supply
	add constraint FK_t0y85vi413s5jeq2qt139msqv foreign key (power_id)references power;

alter table power_supply
	add constraint FK_c9hg7p1v8qjeyooephti9k022 foreign key (id)references discoverable_entity;

alter table power_supply_input_range
	add constraint FK_p80ruyp4iy1vxoetuwo780gtb foreign key (power_supply_id)references power_supply;

alter table power_voltage
	add constraint FK_mplja81rtyg4gn970pexw1ml9 foreign key (power_id)references power;

alter table power_voltage
	add constraint FK_m4ur7rxv21lm8fv0osc3aro7f foreign key (id)references discoverable_entity;

alter table processor
	add constraint FK_c0ryxm4p5bj9737gvef5kglfw foreign key (computer_system_id)references computer_system;

alter table processor
	add constraint FK_2tcqjwetvafjrncwxdgfxqmg0 foreign key (id)references discoverable_entity;

alter table processor_capability
	add constraint FK_kseg4fqdmhk3w58imcvikarn1 foreign key (processor_id)references processor;

alter table processor_metrics
	add constraint FK_joal9pqt7jvvhxrrredj6w4qi foreign key (processor_id)references processor;

alter table processor_metrics
	add constraint FK_gt7e64sl76bxwcn4sgx9r593d foreign key (id)references discoverable_entity;

alter table processor_metrics_health
	add constraint FK_4f02cuaqgs8iyi1n8xt9r0yb9 foreign key (processor_metrics_id)references processor_metrics;

alter table processor_on_package_memory
	add constraint FK_nl1sghf3mctnrveh4by8msg3v foreign key (processor_id)references processor;

alter table redundancy
	add constraint FK_p2xhovnbncw8tsr2mpdnnegim foreign key (redundancy_owner_id)references discoverable_entity;

alter table redundancy
	add constraint FK_g95xs7iwq1vwj17rreeamxlip foreign key (id)references discoverable_entity;

alter table redundancy_member
	add constraint FK_tf9dqivigi9clulp696ivvg86 foreign key (member_id)references redundancy;

alter table redundancy_member
	add constraint FK_br8sw85ec61iippj8x61lmi5o foreign key (redundancy_id)references discoverable_entity;

alter table replica_info
	add constraint FK_4fskd7byitrhe50112s92gyi9 foreign key (replica_id)references volume;

alter table replica_info
	add constraint FK_3w7vxhbx0vckkyi5v4ki9l980 foreign key (volume_id)references volume;

alter table replica_info
	add constraint FK_h7p9dm60cfupvug5agc3xummw foreign key (id)references discoverable_entity;

alter table serial_console_connect_type_supported
	add constraint FK_l2dih1yqlnjj3m7ugbdv72ewm foreign key (serial_console_id)references manager;

alter table simple_storage
	add constraint FK_3863hyymhay9xrguda7x76inw foreign key (computer_system_id)references computer_system;

alter table simple_storage
	add constraint FK_pcbos2cmcqofu5aii39b9713x foreign key (id)references discoverable_entity;

alter table simple_storage_device
	add constraint FK_3hw06r8lalse8hhqska4u25px foreign key (simple_storage_id)references simple_storage;

alter table storage
	add constraint FK_ea0hs48jmmlouy8ku36smxvoy foreign key (chassis_id)references chassis;

alter table storage
	add constraint FK_9d9s1f6ryg37k1u44w1elucb8 foreign key (computer_system_id)references computer_system;

alter table storage
	add constraint FK_303v22f6u27remio9g3wkjgs4 foreign key (id)references discoverable_entity;

alter table storage_controller
	add constraint FK_n69yhp10frad0v71617exy1ot foreign key (storage_id)references storage;

alter table storage_controller
	add constraint FK_h4ywc7mg92b5qxytlch0g6h9r foreign key (storage_adapter_id)references storage;

alter table storage_controller
	add constraint FK_d8ifocsns87v5ijv1mu1d6d2i foreign key (id)references discoverable_entity;

alter table storage_controller_identifier
	add constraint FK_3s98pxsdyy5ds2dh7fqogda78 foreign key (storage_controller_id)references storage_controller;

alter table storage_controller_pcie_device_function
	add constraint FK_68537nl7bje3wtpnr17kg1ubv foreign key (pcie_device_function_id)references pcie_device_function;

alter table storage_controller_pcie_device_function
	add constraint FK_5eambtahd9xjhx9sqa6wtv7y7 foreign key (storage_controller_id)references storage_controller;

alter table storage_controller_supported_controller_protocol
	add constraint FK_nsmsvtjba9gln1t5snr9bguyg foreign key (storage_controller_id)references storage_controller;

alter table storage_controller_supported_device_protocol
	add constraint FK_902q8gdwv8jpfvlrisvmg61vn foreign key (storage_controller_id)references storage_controller;

alter table storage_pool
	add constraint FK_lr31gfsnaejrj2l3s3mitfjb0 foreign key (storage_pool_id)references storage_pool;

alter table storage_pool
	add constraint FK_jlxmwreqatm6c9w4tapkacmkm foreign key (storage_service_id)references storage_service;

alter table storage_pool
	add constraint FK_qixnmckht3ylhe5q6brew0rvb foreign key (id)references discoverable_entity;

alter table storage_pool_composed_node
	add constraint FK_abqgvnb4vwa2l7aimajrca4ai foreign key (composed_node_id)references composed_node;

alter table storage_pool_composed_node
	add constraint FK_e9scdonhh1t1vof5wcc7bha3b foreign key (storage_pool_id)references storage_pool;

alter table storage_providing_pool_capacity_source
	add constraint FK_mh12abccrkpmwfl54g9v6g792 foreign key (capacity_source_id)references capacity_source;

alter table storage_providing_pool_capacity_source
	add constraint FK_h2wtq3r38qw3h9njxrcfg67rk foreign key (storage_pool_id)references storage_pool;

alter table storage_service
	add constraint FK_90sl33yp60n4dgpbjm9osntdi foreign key (computer_system_id)references computer_system;

alter table storage_service
	add constraint FK_jqmsra7bcla6voptd2dgvethj foreign key (id)references discoverable_entity;

alter table storage_service_manager
	add constraint FK_kl8lw8718nq4qamryjb7mw4cr foreign key (manager_id)references manager;

alter table storage_service_manager
	add constraint FK_a5pw738ejj33n0al7d0s3t6sa foreign key (storage_service_id)references storage_service;

alter table switch
	add constraint FK_hrjog9vjw78fsq9lf7e4ni4hv foreign key (fabric_id)references fabric;

alter table switch
	add constraint FK_6xxu8f9txm6ub9qrh1um7k44y foreign key (id)references discoverable_entity;

alter table switch_allowable_reset_type
	add constraint FK_jewchglyx24hsif1k1kqeb55b foreign key (switch_id)references switch;

alter table switch_manager
	add constraint FK_pa97j31fvr8pa3b5rvkekws4s foreign key (manager_id)references manager;

alter table switch_manager
	add constraint FK_l3kgrc0kls4fvm3bgmcwo4i5m foreign key (switch_id)references switch;

alter table switch_zone
	add constraint FK_l066wmax7qsjy9yhjchouylj6 foreign key (zone_id)references zone;

alter table switch_zone
	add constraint FK_npljxjloe4pmjy5out3nxufyb foreign key (switch_id)references switch;

alter table thermal
	add constraint FK_g9dk9xsm6vix174n6bx6d9smj foreign key (chassis_id)references chassis;

alter table thermal
	add constraint FK_hgrnb6ky9jlfbhacmq386s4fk foreign key (id)references discoverable_entity;

alter table thermal_fan
	add constraint FK_4pip60vb2oqar3wub3joioj8m foreign key (thermal_id)references thermal;

alter table thermal_fan
	add constraint FK_5lvlfhlxtary3auouk03jld2 foreign key (id)references discoverable_entity;

alter table thermal_temperature
	add constraint FK_ewo2wh7k7qd67u1uwn8o7t7sy foreign key (thermal_id)references thermal;

alter table thermal_temperature
	add constraint FK_49qdcbh7rro8iww9pxvukxqb3 foreign key (id)references discoverable_entity;

alter table unknown_oem
	add constraint FK_9meypuq1dtsy5ufl3qs5ywmdi foreign key (entity_id)references discoverable_entity;

alter table volume
	add constraint FK_exalqcebmxgj3iw837c144uoh foreign key (composed_node_id)references composed_node;

alter table volume
	add constraint FK_hrgd9v89f7rgxt94oefydf44p foreign key (volume_metadata_id)references volume_metadata;

alter table volume
	add constraint FK_2a4qbtt7xnk7jip0rv23lse52 foreign key (volume_metrics_id)references volume_metrics;

alter table volume
	add constraint FK_9uqlco3xv0w7i5o3nv22uigdh foreign key (storage_pool_id)references storage_pool;

alter table volume
	add constraint FK_oncgq0cahnas9itl0a19gqpt6 foreign key (storage_service_id)references storage_service;

alter table volume
	add constraint FK_f4fmido82kejr7g7ijcffs9js foreign key (id)references discoverable_entity;

alter table volume_access_capability
	add constraint FK_3w1d4dc804d4emtn7pmsdtcmi foreign key (volume_id)references volume;

alter table volume_allowable_initialize_type
	add constraint FK_2sf0fd6u6l9xcge0dnuwy4b1v foreign key (volume_id)references volume;

alter table volume_identifier
	add constraint FK_qq4xxk5acatfyti1m8xdpfw5i foreign key (volume_id)references volume;

alter table volume_metrics
	add constraint FK_4oa20ch9f9hru5cqw4qaorhw6 foreign key (volume_id)references volume;

alter table volume_metrics
	add constraint FK_15j33vvd2aq9r37chjyu8l5ep foreign key (id)references discoverable_entity;

alter table volume_operation
	add constraint FK_3tvkitl5axuxbddhf8latbina foreign key (volume_id)references volume;

alter table zone
	add constraint FK_1qx30aoet3jq3k8y2ffyoi5al foreign key (fabric_id)references fabric;

alter table zone
	add constraint FK_7jhic4whhk83teqpjvv0c0h8t foreign key (id)references discoverable_entity;

