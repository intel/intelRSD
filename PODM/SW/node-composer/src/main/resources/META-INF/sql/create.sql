create table associated_drive_uri (
	associated_drive_uri_id int8 not null,
	associated_drives_uris text
);

create table associated_endpoint_uri (
	associated_endpoint_uri_id int8 not null,
	associated_endpoints_uris text
);

create table associated_processor_uri (
	associated_processor_uri_id int8 not null,
	associated_processor_uris text
);

create table associated_volume_uri (
	associated_volume_uri_id int8 not null,
	associated_volumes_uris text
);

create table boot_boot_source_override_mode_allowable_value (
	boot_id int8 not null,
	boot_source_override_mode_allowable_value text,
	boot_source_override_mode_allowable_value_order int4 not null,
	primary key (boot_id,boot_source_override_mode_allowable_value_order)
);

create table boot_boot_source_override_target_allowable_value (
	boot_id int8 not null,
	boot_source_override_target_allowable_value text,
	boot_source_override_target_allowable_value_order int4 not null,
	primary key (boot_id,boot_source_override_target_allowable_value_order)
);

create table capacity_source (
	id int8 not null,
	volume_id int8,
	primary key (id)
);

create table chassis (
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

create table chassis_pcie_device (
	chassis_id int8 not null,
	pcie_device_id int8 not null,
	primary key (chassis_id,pcie_device_id)
);

create table composed_node (
	id  bigserial not null,
	version integer DEFAULT 0 not null,
	associated_computer_system_uuid text,
	associated_remote_target_iqn text,
	clear_optane_memory_on_delete boolean,
	clear_tpm_on_delete boolean,
	composed_node_id bigserial not null,
	composed_node_state text,
	description text,
	eligible_for_recovery boolean,
	name text,
	prior_untagged_vlan_id int4,
	remote_drive_capacity_gib numeric,
	status text,
	tagged_values text,
	uri text,
	computer_system_id int8,
	primary key (id)
);

create table computer_system (
	achievable boolean,
	asset_tag text,
	boot_boot_source_override_enabled text,
	boot_boot_source_override_mode text,
	boot_boot_source_override_target text,
	memory_summary_total_system_memory_gib numeric,
	power_state text,
	system_type text,
	trusted_execution_technology_enabled boolean,
	user_mode_enabled boolean,
	uuid text,
	id int8 not null,
	computer_system_metadata_id int8,
	primary key (id)
);

create table computer_system_allowable_reset_type (
	computer_system_id int8 not null,
	allowable_reset_type text,
	allowable_reset_type_order int4 not null,
	primary key (computer_system_id,allowable_reset_type_order)
);

create table computer_system_manager (
	computer_system_id int8 not null,
	manager_id int8 not null,
	primary key (computer_system_id,manager_id)
);

create table computer_system_metadata (
	id  bigserial not null,
	version integer DEFAULT 0 not null,
	allocated boolean,
	primary key (id)
);

create table computer_system_pcie_connection_id (
	computer_system_id int8 not null,
	pcie_connection_id text,
	pcie_connection_id_order int4 not null,
	primary key (computer_system_id,pcie_connection_id_order)
);

create table computer_system_performance_configuration_type (
	computer_system_id int8 not null,
	performance_configuration_type text,
	performance_configuration_type_order int4 not null,
	primary key (computer_system_id,performance_configuration_type_order)
);

create table computer_system_trusted_module (
	computer_system_id int8 not null,
	trusted_module_firmware_version text,
	trusted_module_interface_type text,
	trusted_module_status text,
	trusted_module_order int4 not null,
	primary key (computer_system_id,trusted_module_order)
);

create table connected_entity (
	entity_role text,
	lun int4,
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

create table discoverable_entity (
	discriminator_class text not null,
	id  bigserial not null,
	version integer DEFAULT 0 not null,
	description text,
	name text,
	status text,
	uri text,
	primary key (id)
);

create table drive (
	achievable boolean,
	capacity_bytes int8,
	drive_erased boolean,
	media_type text,
	protocol text,
	rotation_speed_rpm numeric,
	serial_number text,
	id int8 not null,
	chassis_id int8,
	composed_node_id int8,
	drive_metadata_id int8,
	pcie_device_function_id int8,
	storage_id int8,
	storage_service_id int8,
	volume_id int8,
	primary key (id)
);

create table drive_metadata (
	id  bigserial not null,
	version integer DEFAULT 0 not null,
	allocated boolean,
	primary key (id)
);

create table endpoint (
	achievable boolean,
	endpoint_authentication_password text,
	endpoint_authentication_username text,
	endpoint_protocol text,
	id int8 not null,
	composed_node_id int8,
	computer_system_id int8,
	fabric_id int8,
	endpoint_metadata_id int8,
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
	version integer DEFAULT 0 not null,
	allocated boolean,
	primary key (id)
);

create table ethernet_interface (
	mac_address text,
	speed_mbps int4,
	id int8 not null,
	computer_system_id int8,
	primary key (id)
);

create table ethernet_interface_endpoint (
	ethernet_interface_id int8 not null,
	endpoint_id int8 not null,
	primary key (ethernet_interface_id,endpoint_id)
);

create table ethernet_interface_supported_protocol (
	ethernet_interface_id int8 not null,
	supported_protocol text,
	supported_protocol_order int4 not null,
	primary key (ethernet_interface_id,supported_protocol_order)
);

create table ethernet_switch (
	id int8 not null,
	chassis_id int8,
	primary key (id)
);

create table ethernet_switch_port (
	neighbor_mac text,
	id int8 not null,
	ethernet_switch_id int8,
	primary_vlan_id int8,
	primary key (id)
);

create table ethernet_switch_port_vlan (
	tagged boolean,
	vlan_id int4,
	id int8 not null,
	ethernet_interface_id int8,
	ethernet_switch_port_id int8,
	primary key (id)
);

create table fabric (
	fabric_type text,
	id int8 not null,
	primary key (id)
);

create table fabric_manager (
	fabric_id int8 not null,
	manager_id int8 not null,
	primary key (fabric_id,manager_id)
);

create table ip_transport_details (
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
	manager_type text,
	remote_redfish_service_uri text,
	service_entry_point_uuid text,
	id int8 not null,
	primary key (id)
);

create table memory (
	capacity_mib int4,
	data_width_bits int4,
	manufacturer text,
	memory_device_type text,
	memory_type text,
	operating_speed_mhz int4,
	id int8 not null,
	computer_system_id int8,
	primary key (id)
);

create table network_adapter (
	id int8 not null,
	chassis_id int8,
	primary key (id)
);

create table network_device_function (
	chap_username text,
	id int8 not null,
	network_adapter_id int8,
	network_interface_id int8,
	primary key (id)
);

create table network_interface (
	id int8 not null,
	computer_system_id int8,
	primary key (id)
);

create table pcie_device (
	id int8 not null,
	primary key (id)
);

create table pcie_device_function (
	id int8 not null,
	pcie_device_id int8,
	primary key (id)
);

create table port (
	port_type text,
	id int8 not null,
	fabric_switch_id int8,
	primary key (id)
);

create table port_endpoint (
	port_id int8 not null,
	endpoint_id int8 not null,
	primary key (port_id,endpoint_id)
);

create table port_pcie_connection_id (
	port_id int8 not null,
	pcie_connection_id text,
	pcie_connection_id_order int4 not null,
	primary key (port_id,pcie_connection_id_order)
);

create table processor (
	achievable boolean,
	brand text,
	instruction_set text,
	max_speed_mhz int4,
	model text,
	processor_erased boolean,
	processor_type text,
	total_cores int4,
	id int8 not null,
	composed_node_id int8,
	computer_system_id int8,
	processor_metadata_id int8,
	primary key (id)
);

create table processor_capability (
	processor_id int8 not null,
	capability text,
	capability_order int4 not null,
	primary key (processor_id,capability_order)
);

create table processor_metadata (
	id  bigserial not null,
	version integer DEFAULT 0 not null,
	allocated boolean,
	primary key (id)
);

create table simple_storage (
	id int8 not null,
	computer_system_id int8,
	primary key (id)
);

create table simple_storage_device (
	id  bigserial not null,
	version integer DEFAULT 0 not null,
	capacity_bytes numeric,
	status text,
	simple_storage_id int8,
	primary key (id)
);

create table storage (
	id int8 not null,
	chassis_id int8,
	computer_system_id int8,
	primary key (id)
);

create table storage_pool (
	data_allocated_bytes numeric,
	data_consumed_bytes numeric,
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
	id int8 not null,
	computer_system_id int8,
	fabric_id int8,
	primary key (id)
);

create table storage_service_manager (
	storage_service_id int8 not null,
	manager_id int8 not null,
	primary key (storage_service_id,manager_id)
);

create table switch (
	switch_type text,
	id int8 not null,
	fabric_id int8,
	primary key (id)
);

create table volume (
	achievable boolean,
	bootable boolean,
	capacity_bytes numeric,
	initialize_action_supported boolean,
	id int8 not null,
	composed_node_id int8,
	volume_metadata_id int8,
	storage_pool_id int8,
	storage_service_id int8,
	primary key (id)
);

create table volume_identifier (
	volume_id int8 not null,
	durable_name text,
	durable_name_format text
);

create table volume_metadata (
	id  bigserial not null,
	version integer DEFAULT 0 not null,
	allocated boolean,
	primary key (id)
);

create table zone (
	id int8 not null,
	fabric_id int8,
	primary key (id)
);

alter table composed_node
	add constraint idx_composed_node_composed_node_id unique (composed_node_id);

alter table composed_node
	add constraint idx_composed_node_uri unique (uri);

alter table discoverable_entity
	add constraint UK_b9xwua9qor92bb4mxcc37a50y unique (uri);

alter table associated_drive_uri
	add constraint FK_1xk99xoj02hb4q1l6q79hsqad foreign key (associated_drive_uri_id)references composed_node;

alter table associated_endpoint_uri
	add constraint FK_tj8mj4lgdbbbq7rfh6xh2x25o foreign key (associated_endpoint_uri_id)references composed_node;

alter table associated_processor_uri
	add constraint FK_i4qhvg3dq9siit6ymmclmgwaw foreign key (associated_processor_uri_id)references composed_node;

alter table associated_volume_uri
	add constraint FK_4ehsffma68kaqxhtxdmuvbi1x foreign key (associated_volume_uri_id)references composed_node;

alter table boot_boot_source_override_mode_allowable_value
	add constraint FK_6fn7re809c41t0wj13le353lp foreign key (boot_id)references computer_system;

alter table boot_boot_source_override_target_allowable_value
	add constraint FK_qf4nieyuxc7ck34wx3xow0e0u foreign key (boot_id)references computer_system;

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

alter table chassis_pcie_device
	add constraint FK_bwbmyt4g5627wfilvxygxsiqr foreign key (pcie_device_id)references pcie_device;

alter table chassis_pcie_device
	add constraint FK_e71b8tftjlqno48so21oibbwt foreign key (chassis_id)references chassis;

alter table composed_node
	add constraint FK_6dvurerir9p7quf3b4bq6vq0t foreign key (computer_system_id)references computer_system;

alter table computer_system
	add constraint FK_md6awm92wh5cq8dmsgwkdbpy1 foreign key (computer_system_metadata_id)references computer_system_metadata;

alter table computer_system
	add constraint FK_cwca10t6xao4h4cdqea0dxita foreign key (id)references discoverable_entity;

alter table computer_system_allowable_reset_type
	add constraint FK_q1iv5xwi4hqrxlwwy374amwth foreign key (computer_system_id)references computer_system;

alter table computer_system_manager
	add constraint FK_5qjqbrmviflp7dgyh3imnvkaw foreign key (manager_id)references manager;

alter table computer_system_manager
	add constraint FK_9gbeankygenoq464djmovjy0c foreign key (computer_system_id)references computer_system;

alter table computer_system_pcie_connection_id
	add constraint FK_csh08v7q5vjxvgt2csgwb9fp6 foreign key (computer_system_id)references computer_system;

alter table computer_system_performance_configuration_type
	add constraint FK_b3ds3xiaccbmf3rko7kxqebeu foreign key (computer_system_id)references computer_system;

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

alter table drive
	add constraint FK_jnpr25lir5pn6sct8eu2sdhjf foreign key (chassis_id)references chassis;

alter table drive
	add constraint FK_hg2i3dcoyr30041ams83q6k1c foreign key (composed_node_id)references composed_node;

alter table drive
	add constraint FK_rnn8cx4x4xma4clftuqi3boy4 foreign key (drive_metadata_id)references drive_metadata;

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

alter table endpoint
	add constraint FK_gt4bk9ytuwb0lyt44d78jj0s7 foreign key (composed_node_id)references composed_node;

alter table endpoint
	add constraint FK_6p15jxa7qa2u2iylyig3n4sx4 foreign key (computer_system_id)references computer_system;

alter table endpoint
	add constraint FK_ofmdqutkvp6sw3g905brjxuhv foreign key (fabric_id)references fabric;

alter table endpoint
	add constraint FK_p7teicxqruuhp6jj6e6mfuv17 foreign key (endpoint_metadata_id)references endpoint_metadata;

alter table endpoint
	add constraint FK_abj470s1wq95lixg45iro1lyh foreign key (storage_service_id)references storage_service;

alter table endpoint
	add constraint FK_s3uwsvug4plk1dpprwdln4smr foreign key (zone_id)references zone;

alter table endpoint
	add constraint FK_7j9v08qwbrpl4wxrjog3cy1u7 foreign key (id)references discoverable_entity;

alter table endpoint_identifier
	add constraint FK_bkav1mey33701095ndikid2tb foreign key (endpoint_id)references endpoint;

alter table ethernet_interface
	add constraint FK_bm8wr6mkdvjpxuvqqpr0iobq2 foreign key (computer_system_id)references computer_system;

alter table ethernet_interface
	add constraint FK_36pt3bdrw0ew81twg7n0kltg7 foreign key (id)references discoverable_entity;

alter table ethernet_interface_endpoint
	add constraint FK_a79h1utxwn29r7oljhatav94s foreign key (endpoint_id)references endpoint;

alter table ethernet_interface_endpoint
	add constraint FK_wb9wkddiltnfev1phs8q44rj foreign key (ethernet_interface_id)references ethernet_interface;

alter table ethernet_interface_supported_protocol
	add constraint FK_ce9it6x1r3qaxvdilh4x3e32q foreign key (ethernet_interface_id)references ethernet_interface;

alter table ethernet_switch
	add constraint FK_n5jyrct0lcdxo4irimihnymjv foreign key (chassis_id)references chassis;

alter table ethernet_switch
	add constraint FK_6rkxinq1qe60ks1g5sninouf7 foreign key (id)references discoverable_entity;

alter table ethernet_switch_port
	add constraint FK_14oc3d5wv6m3e9ptni5kolwj7 foreign key (ethernet_switch_id)references ethernet_switch;

alter table ethernet_switch_port
	add constraint FK_ex12fvee2h96igx56fnookgb foreign key (primary_vlan_id)references ethernet_switch_port_vlan;

alter table ethernet_switch_port
	add constraint FK_gv0tspm65f7sfuc8pgipel0s8 foreign key (id)references discoverable_entity;

alter table ethernet_switch_port_vlan
	add constraint FK_qummguchfcuwyfj0yt95hf3by foreign key (ethernet_interface_id)references ethernet_interface;

alter table ethernet_switch_port_vlan
	add constraint FK_llscecvwe3wcu265sxbwjfko0 foreign key (ethernet_switch_port_id)references ethernet_switch_port;

alter table ethernet_switch_port_vlan
	add constraint FK_8ejp883if3pjm176b5cic3o2o foreign key (id)references discoverable_entity;

alter table fabric
	add constraint FK_4uvir8mle70pabjldo72m198f foreign key (id)references discoverable_entity;

alter table fabric_manager
	add constraint FK_ldl6hww7lha75i7dlqm34umk5 foreign key (manager_id)references manager;

alter table fabric_manager
	add constraint FK_lmw8yr5tfe2gbbs83cwr581cb foreign key (fabric_id)references fabric;

alter table ip_transport_details
	add constraint FK_qa9v1pip5eqpwwkkk497yvhi1 foreign key (endpoint_id)references endpoint;

alter table ip_transport_details
	add constraint FK_lv4ye0wvy8v0aopppbd8ueb1o foreign key (id)references discoverable_entity;

alter table manager
	add constraint FK_dk9y35huoo6qkm48fj2n3s1kb foreign key (id)references discoverable_entity;

alter table memory
	add constraint FK_61vlwhnkrir23pfwe7y6wd50s foreign key (computer_system_id)references computer_system;

alter table memory
	add constraint FK_imr6buoy6xv7iok8k16u7auxi foreign key (id)references discoverable_entity;

alter table network_adapter
	add constraint FK_e2yq31uq7849j3erdytoyoynu foreign key (chassis_id)references chassis;

alter table network_adapter
	add constraint FK_ael091qr0akma3s7ceulcru6h foreign key (id)references discoverable_entity;

alter table network_device_function
	add constraint FK_o1ur5jntl84knvcudja7v3w99 foreign key (network_adapter_id)references network_adapter;

alter table network_device_function
	add constraint FK_jchhhhbmfa9rly6mf7nrw5tf9 foreign key (network_interface_id)references network_interface;

alter table network_device_function
	add constraint FK_dmc4vvphx2kk2t9riycu1vtq4 foreign key (id)references discoverable_entity;

alter table network_interface
	add constraint FK_5dft6yhrjq4d3c7gegqmwiqkb foreign key (computer_system_id)references computer_system;

alter table network_interface
	add constraint FK_6crdctyjvdt7tx5iuy74uwdp1 foreign key (id)references discoverable_entity;

alter table pcie_device
	add constraint FK_btgk9gcjgrc308q7af4pu237d foreign key (id)references discoverable_entity;

alter table pcie_device_function
	add constraint FK_qeyudqcco39drbfdsr53j1s6u foreign key (pcie_device_id)references pcie_device;

alter table pcie_device_function
	add constraint FK_ogvc8drh5vmc13gp8pwo2fab8 foreign key (id)references discoverable_entity;

alter table port
	add constraint FK_il9vsjj7uakouf5c7driq2v9m foreign key (fabric_switch_id)references switch;

alter table port
	add constraint FK_dvgc32pm231nt5p6hnjjpbo9s foreign key (id)references discoverable_entity;

alter table port_endpoint
	add constraint FK_cqdrxblk501fansv2o90havj6 foreign key (endpoint_id)references endpoint;

alter table port_endpoint
	add constraint FK_hlt8ahyofrovcajtvw4sf2c8n foreign key (port_id)references port;

alter table port_pcie_connection_id
	add constraint FK_ol24bw8ag6yr0x3hi6a468nbk foreign key (port_id)references port;

alter table processor
	add constraint FK_42x49ygafsbiy09ev7upihfi0 foreign key (composed_node_id)references composed_node;

alter table processor
	add constraint FK_c0ryxm4p5bj9737gvef5kglfw foreign key (computer_system_id)references computer_system;

alter table processor
	add constraint FK_be30xw58waxh6vwt7fe9lyrq5 foreign key (processor_metadata_id)references processor_metadata;

alter table processor
	add constraint FK_2tcqjwetvafjrncwxdgfxqmg0 foreign key (id)references discoverable_entity;

alter table processor_capability
	add constraint FK_kseg4fqdmhk3w58imcvikarn1 foreign key (processor_id)references processor;

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
	add constraint FK_pwsftnxjode4yi414w5an0okh foreign key (fabric_id)references fabric;

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

alter table volume
	add constraint FK_exalqcebmxgj3iw837c144uoh foreign key (composed_node_id)references composed_node;

alter table volume
	add constraint FK_hrgd9v89f7rgxt94oefydf44p foreign key (volume_metadata_id)references volume_metadata;

alter table volume
	add constraint FK_9uqlco3xv0w7i5o3nv22uigdh foreign key (storage_pool_id)references storage_pool;

alter table volume
	add constraint FK_oncgq0cahnas9itl0a19gqpt6 foreign key (storage_service_id)references storage_service;

alter table volume
	add constraint FK_f4fmido82kejr7g7ijcffs9js foreign key (id)references discoverable_entity;

alter table volume_identifier
	add constraint FK_qq4xxk5acatfyti1m8xdpfw5i foreign key (volume_id)references volume;

alter table zone
	add constraint FK_1qx30aoet3jq3k8y2ffyoi5al foreign key (fabric_id)references fabric;

alter table zone
	add constraint FK_7jhic4whhk83teqpjvv0c0h8t foreign key (id)references discoverable_entity;
