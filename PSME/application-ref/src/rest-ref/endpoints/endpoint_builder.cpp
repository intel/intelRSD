/*!
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
 *
 * @copyright
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * @copyright
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * @copyright
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * */

#include "psme/rest-ref/endpoints/endpoint_builder.hpp"
#include "psme/rest-ref/endpoints/endpoints.hpp"
#include "psme/rest-ref/server/multiplexer.hpp"
#include "psme/rest-ref/server/utils.hpp"
#include "psme/rest-ref/endpoints/path_builder.hpp"

using namespace psme::rest;
using namespace psme::rest::endpoint;
using namespace psme::rest::server;

using namespace app::rest;

EndpointBuilder::~EndpointBuilder() {}

void EndpointBuilder::build_endpoints(::psme::rest::server::Multiplexer& mp) {
    mp.use_before([this](const Request&, Response& res) {
        res.set_header(ContentType::CONTENT_TYPE, ContentType::JSON);
    });

    // "/redfish
    mp.register_handler(Redfish::UPtr(new Redfish(
                        PathBuilder().append(constants::PathParam::REDFISH)
                                    .build())), AccessType::ALL);

    // "/redfish/v1"
    mp.register_handler(Root::UPtr(new Root(
                        PathBuilder(constants::PathParam::BASE_URL).build())),
                        AccessType::ALL);

    // "/redfish/v1/$metadata"
    mp.register_handler(MetadataRoot::UPtr(new MetadataRoot(
                        PathBuilder(constants::PathParam::BASE_URL)
                                    .append(constants::PathParam::METADATA_ROOT)
                                    .build())),
                        AccessType::ALL);

    // "/redfish/v1/metadata/{metadata_file:*}"
    mp.register_handler(Metadata::UPtr(new Metadata(
                        PathBuilder(constants::PathParam::BASE_URL)
                                    .append(constants::PathParam::METADATA)
                                    .append_regex(constants::PathParam::METADATA_FILE, constants::PathParam::XML_REGEX)
                                    .build())),
                        AccessType::ALL);

    // "/redfish/v1/EventService"
    mp.register_handler(EventService::UPtr(new EventService(
                        PathBuilder(constants::PathParam::BASE_URL)
                                    .append(constants::Root::EVENT_SERVICE)
                                    .build())));

    // "/redfish/v1/EventService/Subscriptions"
    mp.register_handler(SubscriptionCollection::UPtr(new SubscriptionCollection(
                        PathBuilder(constants::PathParam::BASE_URL)
                                    .append(constants::Root::EVENT_SERVICE)
                                    .append(constants::EventService::SUBSCRIPTIONS)
                                    .build())));

    // "/redfish/v1/EventService/Subscriptions/{subscriptionId:[0-9]+}"
    mp.register_handler(Subscription::UPtr(new Subscription(
                        PathBuilder(constants::PathParam::BASE_URL)
                                    .append(constants::Root::EVENT_SERVICE)
                                    .append(constants::EventService::SUBSCRIPTIONS)
                                    .append_regex(constants::PathParam::SUBSCRIPTION_ID, constants::PathParam::ID_REGEX)
                                    .build())));

#ifndef NDEBUG
    // "/redfish/v1/EventService/TestEventSubscription"
    mp.register_handler(TestEventSubscription::UPtr(new TestEventSubscription(
                        PathBuilder(constants::PathParam::BASE_URL)
                                    .append(constants::Root::EVENT_SERVICE)
                                    .append("TestEventSubscription")
                                    .build())));
#endif

    // "/redfish/v1/Chassis"
    mp.register_handler(ChassisCollection::UPtr(new ChassisCollection(
                        PathBuilder(constants::PathParam::BASE_URL)
                                    .append(constants::Root::CHASSIS)
                                    .build())));

    // "/redfish/v1/Chassis/{chassisId:[0-9]+}"
    mp.register_handler(Chassis::UPtr(new Chassis(
                        PathBuilder(constants::PathParam::BASE_URL)
                                    .append(constants::Root::CHASSIS)
                                    .append_regex(constants::PathParam::CHASSIS_ID, constants::PathParam::ID_REGEX)
                                    .build())));

    // "/redfish/v1/Managers"
    mp.register_handler(ManagerCollection::UPtr(new ManagerCollection(
                        PathBuilder(constants::PathParam::BASE_URL)
                                    .append(constants::Root::MANAGERS)
                                    .build())));

    // "/redfish/v1/Managers/{managerId:[0-9]+}"
    mp.register_handler(Manager::UPtr(new Manager(
                        PathBuilder(constants::PathParam::BASE_URL)
                                    .append(constants::Root::MANAGERS)
                                    .append_regex(constants::PathParam::MANAGER_ID, constants::PathParam::ID_REGEX)
                                    .build())));

    // "/redfish/v1/Managers/{managerId:[0-9]+}/EthernetInterfaces"
    mp.register_handler(ManagerNetworkInterfaceCollection::UPtr(new ManagerNetworkInterfaceCollection(
                        PathBuilder(constants::PathParam::BASE_URL)
                                    .append(constants::Root::MANAGERS)
                                    .append_regex(constants::PathParam::MANAGER_ID, constants::PathParam::ID_REGEX)
                                    .append(constants::Manager::ETHERNET_INTERFACES)
                                    .build())));

    // "/redfish/v1/Managers/{managerId:[0-9]+}/EthernetInterfaces/{nicId:[0-9]+}"
    mp.register_handler(ManagerNetworkInterface::UPtr(new ManagerNetworkInterface(
                        PathBuilder(constants::PathParam::BASE_URL)
                                    .append(constants::Root::MANAGERS)
                                    .append_regex(constants::PathParam::MANAGER_ID, constants::PathParam::ID_REGEX)
                                    .append(constants::Manager::ETHERNET_INTERFACES)
                                    .append_regex(constants::PathParam::NIC_ID, constants::PathParam::ID_REGEX)
                                    .build())));

    // "/redfish/v1/Managers/{managerId:[0-9]+}/NetworkProtocol"
    mp.register_handler(NetworkProtocol::UPtr(new NetworkProtocol(
                        PathBuilder(constants::PathParam::BASE_URL)
                                    .append(constants::Root::MANAGERS)
                                    .append_regex(constants::PathParam::MANAGER_ID, constants::PathParam::ID_REGEX)
                                    .append(constants::Manager::NETWORK_PROTOCOL)
                                    .build())));

    // "/redfish/v1/Systems"
    mp.register_handler(SystemsCollection::UPtr(new SystemsCollection(
                        PathBuilder(constants::PathParam::BASE_URL)
                                    .append(constants::Root::SYSTEMS)
                                    .build())));

    // "/redfish/v1/Systems/{systemId:[0-9]+}"
    mp.register_handler(System::UPtr(new System(
                        PathBuilder(constants::PathParam::BASE_URL)
                                    .append(constants::Root::SYSTEMS)
                                    .append_regex(constants::PathParam::SYSTEM_ID, constants::PathParam::ID_REGEX)
                                    .build())));

    // "/redfish/v1/Systems/{systemId:[0-9]+}/Actions/ComputerSystem.Reset"
    mp.register_handler(SystemReset::UPtr(new SystemReset(
                        PathBuilder(constants::PathParam::BASE_URL)
                                    .append(constants::Root::SYSTEMS)
                                    .append_regex(constants::PathParam::SYSTEM_ID, constants::PathParam::ID_REGEX)
                                    .append(constants::Common::ACTIONS)
                                    .append(constants::System::COMPUTER_SYSTEM_RESET)
                                    .build())));

    // "/redfish/v1/Systems/{systemId:[0-9]+}/EthernetInterfaces"
    mp.register_handler(SystemNetworkInterfaceCollection::UPtr(new SystemNetworkInterfaceCollection(
                        PathBuilder(constants::PathParam::BASE_URL)
                                    .append(constants::Root::SYSTEMS)
                                    .append_regex(constants::PathParam::SYSTEM_ID, constants::PathParam::ID_REGEX)
                                    .append(constants::System::ETHERNET_INTERFACES)
                                    .build())));

    // "/redfish/v1/Systems/{systemId:[0-9]+}/EthernetInterfaces/{nicId:[0-9]+}"
    mp.register_handler(SystemNetworkInterface::UPtr(new SystemNetworkInterface(
                        PathBuilder(constants::PathParam::BASE_URL)
                                    .append(constants::Root::SYSTEMS)
                                    .append_regex(constants::PathParam::SYSTEM_ID, constants::PathParam::ID_REGEX)
                                    .append(constants::System::ETHERNET_INTERFACES)
                                    .append_regex(constants::PathParam::NIC_ID, constants::PathParam::ID_REGEX)
                                    .build())));

    // "/redfish/v1/Systems/{systemId:[0-9]+}/DimmConfig"
    mp.register_handler(DimmConfigCollection::UPtr(new DimmConfigCollection(
                        PathBuilder(constants::PathParam::BASE_URL)
                                    .append(constants::Root::SYSTEMS)
                                    .append_regex(constants::PathParam::SYSTEM_ID, constants::PathParam::ID_REGEX)
                                    .append(constants::System::DIMM_CONFIG)
                                    .build())));

    // "/redfish/v1/Systems/{systemId:[0-9]+}/DimmConfig/{dimmConfigId:[0-9]+}"
    mp.register_handler(DimmConfig::UPtr(new DimmConfig(
                        PathBuilder(constants::PathParam::BASE_URL)
                                    .append(constants::Root::SYSTEMS)
                                    .append_regex(constants::PathParam::SYSTEM_ID, constants::PathParam::ID_REGEX)
                                    .append(constants::System::DIMM_CONFIG)
                                    .append_regex(constants::PathParam::DIMM_CONFIG_ID, constants::PathParam::ID_REGEX)
                                    .build())));

    // "/redfish/v1/Systems/{systemId:[0-9]+}/MemoryChunks"
    mp.register_handler(MemoryChunksCollection::UPtr(new MemoryChunksCollection(
                        PathBuilder(constants::PathParam::BASE_URL)
                                    .append(constants::Root::SYSTEMS)
                                    .append_regex(constants::PathParam::SYSTEM_ID, constants::PathParam::ID_REGEX)
                                    .append(constants::System::MEMORY_CHUNKS)
                                    .build())));

    //  "/redfish/v1/Systems/{systemId:[0-9]+}/MemoryChunks/{chunkId:[0-9]+}"
    mp.register_handler(MemoryChunk::UPtr(new MemoryChunk(
                        PathBuilder(constants::PathParam::BASE_URL)
                                    .append(constants::Root::SYSTEMS)
                                    .append_regex(constants::PathParam::SYSTEM_ID, constants::PathParam::ID_REGEX)
                                    .append(constants::System::MEMORY_CHUNKS)
                                    .append_regex(constants::PathParam::MEMORY_CHUNK_ID, constants::PathParam::ID_REGEX)
                                    .build())));

    //  "/redfish/v1/Systems/{systemId:[0-9]+}/Processors"
    mp.register_handler(ProcessorsCollection::UPtr(new ProcessorsCollection(
                        PathBuilder(constants::PathParam::BASE_URL)
                                    .append(constants::Root::SYSTEMS)
                                    .append_regex(constants::PathParam::SYSTEM_ID, constants::PathParam::ID_REGEX)
                                    .append(constants::System::PROCESSORS)
                                    .build())));

    // "/redfish/v1/Systems/{systemId:[0-9]+}/Processors/{processorId:[0-9]+}"
    mp.register_handler(Processor::UPtr(new Processor(
                        PathBuilder(constants::PathParam::BASE_URL)
                                    .append(constants::Root::SYSTEMS)
                                    .append_regex(constants::PathParam::SYSTEM_ID, constants::PathParam::ID_REGEX)
                                    .append(constants::System::PROCESSORS)
                                    .append_regex(constants::PathParam::PROCESSOR_ID, constants::PathParam::ID_REGEX)
                                    .build())));

    // "/redfish/v1/Systems/{systemId:[0-9]+}/Adapters"
    mp.register_handler(StorageAdaptersCollection::UPtr(new StorageAdaptersCollection(
                        PathBuilder(constants::PathParam::BASE_URL)
                                    .append(constants::Root::SYSTEMS)
                                    .append_regex(constants::PathParam::SYSTEM_ID, constants::PathParam::ID_REGEX)
                                    .append(constants::System::STORAGE_ADAPTERS)
                                    .build())));

    // "/redfish/v1/Systems/{systemId:[0-9]+}/Adapters/{adapterId:[0-9]+}"
    mp.register_handler(StorageAdapter::UPtr(new StorageAdapter(
                        PathBuilder(constants::PathParam::BASE_URL)
                                    .append(constants::Root::SYSTEMS)
                                    .append_regex(constants::PathParam::SYSTEM_ID, constants::PathParam::ID_REGEX)
                                    .append(constants::System::STORAGE_ADAPTERS)
                                    .append_regex(constants::PathParam::ADAPTER_ID, constants::PathParam::ID_REGEX)
                                    .build())));

    // "/redfish/v1/Systems/{systemId:[0-9]+}/Adapters/{adapterId:[0-9]+}/Devices"
    mp.register_handler(DevicesCollection::UPtr(new DevicesCollection(
                        PathBuilder(constants::PathParam::BASE_URL)
                                    .append(constants::Root::SYSTEMS)
                                    .append_regex(constants::PathParam::SYSTEM_ID, constants::PathParam::ID_REGEX)
                                    .append(constants::System::STORAGE_ADAPTERS)
                                    .append_regex(constants::PathParam::ADAPTER_ID, constants::PathParam::ID_REGEX)
                                    .append(constants::StorageAdapter::DEVICES)
                                    .build())));

    // "/redfish/v1/Systems/{systemId:[0-9]+}/Adapters/{adapterId:[0-9]+}/Devices/{deviceId:[0-9]+}"
    mp.register_handler(Device::UPtr(new Device(
                        PathBuilder(constants::PathParam::BASE_URL)
                                    .append(constants::Root::SYSTEMS)
                                    .append_regex(constants::PathParam::SYSTEM_ID, constants::PathParam::ID_REGEX)
                                    .append(constants::System::STORAGE_ADAPTERS)
                                    .append_regex(constants::PathParam::ADAPTER_ID, constants::PathParam::ID_REGEX)
                                    .append(constants::StorageAdapter::DEVICES)
                                    .append_regex(constants::PathParam::DEVICE_ID, constants::PathParam::ID_REGEX)
                                    .build())));

    // "/redfish/v1/EthernetSwitches"
    mp.register_handler(EthernetSwitchCollection::UPtr(new EthernetSwitchCollection(
                            PathBuilder(constants::PathParam::BASE_URL)
                                    .append(constants::Root::ETHERNET_SWITCHES)
                                    .build())));

    // "/redfish/v1/EthernetSwitches/{ethernetSwitchId:[0-9]+}"
    mp.register_handler(EthernetSwitch::UPtr(new EthernetSwitch(
                            PathBuilder(constants::PathParam::BASE_URL)
                                    .append(constants::Root::ETHERNET_SWITCHES)
                                    .append_regex(constants::PathParam::ETHERNET_SWITCH_ID, constants::PathParam::ID_REGEX)
                                    .build())));

    // "/redfish/v1/EthernetSwitches/{ethernetSwitchId:[0-9]+}/ACLs"
    mp.register_handler(AclCollection::UPtr(new AclCollection(
        PathBuilder(constants::PathParam::BASE_URL)
            .append(constants::Root::ETHERNET_SWITCHES)
            .append_regex(constants::PathParam::ETHERNET_SWITCH_ID,
                          constants::PathParam::ID_REGEX)
            .append(constants::Switch::ACLS)
            .build())));

    // "/redfish/v1/EthernetSwitches/{ethernetSwitchId:[0-9]+}/ACLs/{aclId:[0-9]+}"
    mp.register_handler(Acl::UPtr(new Acl(
        PathBuilder(constants::PathParam::BASE_URL)
            .append(constants::Root::ETHERNET_SWITCHES)
            .append_regex(constants::PathParam::ETHERNET_SWITCH_ID,
                          constants::PathParam::ID_REGEX)
            .append(constants::Switch::ACLS)
            .append_regex(constants::PathParam::ACL_ID,
                          constants::PathParam::ID_REGEX)
            .build())));

    // "/redfish/v1/EthernetSwitches/{ethernetSwitchId:[0-9]+}/ACLs/{aclId:[0-9]+}/Actions/EthernetSwitchACL.Bind"
    mp.register_handler(AclBind<true>::UPtr(new AclBind<true>(
        PathBuilder(constants::PathParam::BASE_URL)
            .append(constants::Root::ETHERNET_SWITCHES)
            .append_regex(constants::PathParam::ETHERNET_SWITCH_ID,
                          constants::PathParam::ID_REGEX)
            .append(constants::Switch::ACLS)
            .append_regex(constants::PathParam::ACL_ID,
                          constants::PathParam::ID_REGEX)
            .append(constants::Common::ACTIONS)
            .append(constants::Acl::BIND_ACL)
            .build())));

    // "/redfish/v1/EthernetSwitches/{ethernetSwitchId:[0-9]+}/ACLs/{aclId:[0-9]+}/Actions/EthernetSwitchACL.Unbind"
    mp.register_handler(AclBind<false>::UPtr(new AclBind<false>(
        PathBuilder(constants::PathParam::BASE_URL)
            .append(constants::Root::ETHERNET_SWITCHES)
            .append_regex(constants::PathParam::ETHERNET_SWITCH_ID,
                          constants::PathParam::ID_REGEX)
            .append(constants::Switch::ACLS)
            .append_regex(constants::PathParam::ACL_ID,
                          constants::PathParam::ID_REGEX)
            .append(constants::Common::ACTIONS)
            .append(constants::Acl::UNBIND_ACL)
            .build())));

    // "/redfish/v1/EthernetSwitches/{ethernetSwitchId:[0-9]+}/ACLs/{aclId:[0-9]+}/Rules"
    mp.register_handler(RuleCollection::UPtr(new RuleCollection(
        PathBuilder(constants::PathParam::BASE_URL)
            .append(constants::Root::ETHERNET_SWITCHES)
            .append_regex(constants::PathParam::ETHERNET_SWITCH_ID,
                          constants::PathParam::ID_REGEX)
            .append(constants::Switch::ACLS)
            .append_regex(constants::PathParam::ACL_ID,
                          constants::PathParam::ID_REGEX)
            .append(constants::Acl::RULES)
            .build())));

    // "/redfish/v1/EthernetSwitches/{ethernetSwitchId:[0-9]+}/ACLs/{aclId:[0-9]+}/Rules/{ruleId:[0-9]+}"
    mp.register_handler(Rule::UPtr(new Rule(
        PathBuilder(constants::PathParam::BASE_URL)
            .append(constants::Root::ETHERNET_SWITCHES)
            .append_regex(constants::PathParam::ETHERNET_SWITCH_ID,
                          constants::PathParam::ID_REGEX)
            .append(constants::Switch::ACLS)
            .append_regex(constants::PathParam::ACL_ID,
                          constants::PathParam::ID_REGEX)
            .append(constants::Acl::RULES)
            .append_regex(constants::PathParam::RULE_ID,
                          constants::PathParam::ID_REGEX)
            .build())));

    // "/redfish/v1/EthernetSwitches/{switchId:[0-9]+}/Ports"
    mp.register_handler(EthernetSwitchPortCollection::UPtr(new EthernetSwitchPortCollection(
                            PathBuilder(constants::PathParam::BASE_URL)
                                    .append(constants::Root::ETHERNET_SWITCHES)
                                    .append_regex(constants::PathParam::ETHERNET_SWITCH_ID, constants::PathParam::ID_REGEX)
                                    .append(constants::Switch::PORTS)
                                    .build())));

    // "/redfish/v1/EthernetSwitches/{switchId:[0-9]+}/Ports/{switchPortId:[0-9]+}"
    mp.register_handler(EthernetSwitchPort::UPtr(new EthernetSwitchPort(
                            PathBuilder(constants::PathParam::BASE_URL)
                                    .append(constants::Root::ETHERNET_SWITCHES)
                                    .append_regex(constants::PathParam::ETHERNET_SWITCH_ID, constants::PathParam::ID_REGEX)
                                    .append(constants::Switch::PORTS)
                                    .append_regex(constants::PathParam::SWITCH_PORT_ID, constants::PathParam::ID_REGEX)
                                    .build())));

    // "/redfish/v1/EthernetSwitches/{ethernetSwitchId:[0-9]+}/Ports/{portId:[0-9]+}/staticMACs"
    mp.register_handler(StaticMacCollection::UPtr(new StaticMacCollection(
            PathBuilder(constants::PathParam::BASE_URL)
                    .append(constants::Root::ETHERNET_SWITCHES)
                    .append_regex(constants::PathParam::ETHERNET_SWITCH_ID,
                                  constants::PathParam::ID_REGEX)
                    .append(constants::Switch::PORTS)
                    .append_regex(constants::PathParam::SWITCH_PORT_ID,
                                  constants::PathParam::ID_REGEX)
                    .append(constants::SwitchPort::STATIC_MACS)
                    .build())));

    // "/redfish/v1/EthernetSwitches/{ethernetSwitchId:[0-9]+}/Ports/{portId:[0-9]+}/staticMACs/{staticMacId:[0-9]+}"
    mp.register_handler(StaticMac::UPtr(new StaticMac(
            PathBuilder(constants::PathParam::BASE_URL)
                    .append(constants::Root::ETHERNET_SWITCHES)
                    .append_regex(constants::PathParam::ETHERNET_SWITCH_ID,
                                  constants::PathParam::ID_REGEX)
                    .append(constants::Switch::PORTS)
                    .append_regex(constants::PathParam::SWITCH_PORT_ID,
                                  constants::PathParam::ID_REGEX)
                    .append(constants::SwitchPort::STATIC_MACS)
                    .append_regex(constants::PathParam::STATIC_MAC_ID,
                                  constants::PathParam::ID_REGEX)
                    .build())));

    // "/redfish/v1/EthernetSwitches/{switchId:[0-9]+}/Ports/{switchPortId:[0-9]+}/VLANs"
    mp.register_handler(VlanNetworkInterfaceCollection::UPtr(new VlanNetworkInterfaceCollection(
                            PathBuilder(constants::PathParam::BASE_URL)
                                    .append(constants::Root::ETHERNET_SWITCHES)
                                    .append_regex(constants::PathParam::ETHERNET_SWITCH_ID, constants::PathParam::ID_REGEX)
                                    .append(constants::Switch::PORTS)
                                    .append_regex(constants::PathParam::SWITCH_PORT_ID, constants::PathParam::ID_REGEX)
                                    .append(constants::SwitchPort::VLANS)
                                    .build())));

    // "/redfish/v1/EthernetSwitches/{switchId:[0-9]+}/Ports/{switchPortId:[0-9]+}/VLANs/{vlanId:[0-9]+}"
    mp.register_handler(VlanNetworkInterface::UPtr(new VlanNetworkInterface(
                            PathBuilder(constants::PathParam::BASE_URL)
                                    .append(constants::Root::ETHERNET_SWITCHES)
                                    .append_regex(constants::PathParam::ETHERNET_SWITCH_ID, constants::PathParam::ID_REGEX)
                                    .append(constants::Switch::PORTS)
                                    .append_regex(constants::PathParam::SWITCH_PORT_ID, constants::PathParam::ID_REGEX)
                                    .append(constants::SwitchPort::VLANS)
                                    .append_regex(constants::PathParam::VLAN_ID, constants::PathParam::ID_REGEX)
                                    .build())));


    // "/redfish/v1/Services"
    mp.register_handler(StorageServiceCollection::UPtr(new StorageServiceCollection(
                            PathBuilder(constants::PathParam::BASE_URL)
                                    .append(constants::Root::SERVICES)
                                    .build())));

    // "/redfish/v1/Services/{serviceId:[0-9]+}"
    mp.register_handler(StorageService::UPtr(new StorageService(
                            PathBuilder(constants::PathParam::BASE_URL)
                                    .append(constants::Root::SERVICES)
                                    .append_regex(constants::PathParam::SERVICE_ID, constants::PathParam::ID_REGEX)
                                    .build())));

    // "/redfish/v1/Services/{serviceId:[0-9]+}/Drives"
    mp.register_handler(PhysicalDrivesCollection::UPtr(new PhysicalDrivesCollection(
                            PathBuilder(constants::PathParam::BASE_URL)
                                    .append(constants::Root::SERVICES)
                                    .append_regex(constants::PathParam::SERVICE_ID, constants::PathParam::ID_REGEX)
                                    .append(constants::StorageService::DRIVES)
                                    .build())));

    // "/redfish/v1/Services/{serviceId:[0-9]+}/Drives/{driveId:[0-9]+}"
    mp.register_handler(PhysicalDrive::UPtr(new PhysicalDrive(
                            PathBuilder(constants::PathParam::BASE_URL)
                                    .append(constants::Root::SERVICES)
                                    .append_regex(constants::PathParam::SERVICE_ID, constants::PathParam::ID_REGEX)
                                    .append(constants::StorageService::DRIVES)
                                    .append_regex(constants::PathParam::DRIVE_ID, constants::PathParam::ID_REGEX)
                                    .build())));

    // "/redfish/v1/Services/{serviceId:[0-9]+}/LogicalDrives"
    mp.register_handler(LogicalDriveCollection::UPtr(new LogicalDriveCollection(
                            PathBuilder(constants::PathParam::BASE_URL)
                                    .append(constants::Root::SERVICES)
                                    .append_regex(constants::PathParam::SERVICE_ID, constants::PathParam::ID_REGEX)
                                    .append(constants::StorageService::LOGICAL_DRIVES)
                                    .build())));

    // "/redfish/v1/Services/{serviceId:[0-9]+}/LogicalDrives/{logicalDriveId:[0-9]+}"
    mp.register_handler(LogicalDrive::UPtr(new LogicalDrive(
                            PathBuilder(constants::PathParam::BASE_URL)
                                    .append(constants::Root::SERVICES)
                                    .append_regex(constants::PathParam::SERVICE_ID, constants::PathParam::ID_REGEX)
                                    .append(constants::StorageService::LOGICAL_DRIVES)
                                    .append_regex(constants::PathParam::LOGICAL_DRIVE_ID, constants::PathParam::ID_REGEX)
                                    .build())));

    // "/redfish/v1/Services/{serviceId:[0-9]+}/Targets"
    mp.register_handler(RemoteTargetCollection::UPtr(new RemoteTargetCollection(
                            PathBuilder(constants::PathParam::BASE_URL)
                                    .append(constants::Root::SERVICES)
                                    .append_regex(constants::PathParam::SERVICE_ID, constants::PathParam::ID_REGEX)
                                    .append(constants::StorageService::TARGETS)
                                    .build())));

    // "/redfish/v1/Services/{serviceId:[0-9]+}/Targets/{remoteTargetId:[0-9]+}"
    mp.register_handler(RemoteTarget::UPtr(new RemoteTarget(
                            PathBuilder(constants::PathParam::BASE_URL)
                                    .append(constants::Root::SERVICES)
                                    .append_regex(constants::PathParam::SERVICE_ID, constants::PathParam::ID_REGEX)
                                    .append(constants::StorageService::TARGETS)
                                    .append_regex(constants::PathParam::REMOTE_TARGET_ID, constants::PathParam::ID_REGEX)
                                    .build())));
}
