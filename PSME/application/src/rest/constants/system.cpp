/*!
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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

#include "psme/rest/constants/system.hpp"



namespace psme {
namespace rest {
namespace constants {

namespace System {
const char SYSTEM_TYPE[] = "SystemType";
const char SKU[] = "SKU";
const char HOST_NAME[] = "HostName";
const char INDICATOR_LED[] = "IndicatorLED";
const char POWER_STATE[] = "PowerState";
const char BOOT[] = "Boot";
const char BOOT_MODE[] = "BootMode";
const char BOOT_SOURCE_OVERRIDE_ENABLED[] = "BootSourceOverrideEnabled";
const char BOOT_SOURCE_OVERRIDE_TARGET[] = "BootSourceOverrideTarget";
const char BOOT_SOURCE_OVERRIDE_TARGET_ALLOWABLE_VALUES[] = "BootSourceOverrideTarget@Redfish.AllowableValues";
const char BOOT_SOURCE_OVERRIDE_MODE[] = "BootSourceOverrideMode";
const char BOOT_SOURCE_OVERRIDE_MODE_ALLOWABLE_VALUES[] = "BootSourceOverrideMode@Redfish.AllowableValues";
const char PXE_PARAMETERS[] = "PXEParameters";
const char ETHERNET_INTERFACE[] = "EthernetInterface";
const char ETHERNET_INTERFACES[] = "EthernetInterfaces";
const char NETWORK_INTERFACES[] = "NetworkInterfaces";
const char UEFI_TARGET[] = "UefiTargetBootSourceOverride";
const char ISCSI_PARAMETERS[] = "iSCSIParameters";
const char AUTH_METHOD[] = "AuthMethod";
const char BOOT_VLAN[] = "BootVLAN";
const char CHAP_MUTUAL_SECRET[] = "CHAPMutualSecret";
const char CHAP_MUTUAL_USERNAME[] = "CHAPMutualUsername";
const char CHAP_SECRET[] = "CHAPSecret";
const char CHAP_USERNAME[] = "CHAPUsername";
const char IP_VERSION[] = "IPVersion";
const char INITIATOR_IP[] = "InitiatorIP";
const char INITIATOR_IQN[] = "INITIATOR_IQN";
const char TARGET_IQN[] = "TargetIQN";
const char TARGET_LUN[] = "TargetLUN";
const char TARGET_PORTAL_IP[] = "TargetPortalIP";
const char TARGET_PORTAL_PORT[] = "TargetPortalPort";
const char BIOS_VERSION[] = "BiosVersion";
const char PROCESSOR_SUMMARY[] = "ProcessorSummary";
const char COUNT[] = "Count";
const char MEMORY_SUMMARY[] = "MemorySummary";
const char TOTAL_SYSTEM_MEMORY_GIB[] = "TotalSystemMemoryGiB";
const char PROCESSORS[] = "Processors";
const char SIMPLE_STORAGE[] = "SimpleStorage";
const char MEMORY[] = "Memory";
const char ENDPOINTS[] = "Endpoints";
const char COMPUTER_SYSTEM_RESET[] = "ComputerSystem.Reset";
const char TARGET[] = "target";
const char ALLOWABLE_RESET_TYPES[] = "ResetType@Redfish.AllowableValues";
const char COMPUTER_SYSTEM_START_DEEP_DISCOVERY[] = "#ComputerSystem.StartDeepDiscovery";
const char DISCOVERY_STATE[] = "DiscoveryState";
const char BASIC[] = "Basic";
const char MEMORY_SOCKETS[] = "MemorySockets";
const char PCI_DEVICES[] = "PciDevices";
const char PCIE_CONNECTION_ID[] = "PCIeConnectionId";
const char PROCESSOR_SOCKETS[] = "ProcessorSockets";
const char STORAGE_ADAPTERS[] = "Adapters";
const char STORAGE[] = "Storage";
const char ON[] = "On";
const char FORCE_OFF[] = "ForceOff";
const char GRACEFUL_SHUTDOWN[] = "GracefulShutdown";
const char FORCE_RESTART[] = "ForceRestart";
const char NMI[] = "Nmi";
const char GRACEFUL_RESTART[] = "GracefulRestart";
const char FORCE_ON[] = "ForceOn";
const char PUSH_POWER_BUTTON[] = "PushPowerButton";

}

namespace Memory {
const char MEMORY_TYPE[] = "MemoryType";
const char MEMORY_DEVICE_TYPE[] = "MemoryDeviceType";
const char BASE_MODULE_TYPE[] = "BaseModuleType";
const char MEMORY_MEDIA[] = "MemoryMedia";
const char CAPACITY_MIB[] = "CapacityMiB";
const char DATA_WIDTH_BITS[] = "DataWidthBits";
const char BUS_WIDTH_BITS[] = "BusWidthBits";
const char ALLOWED_SPEEDS_MHZ[] = "AllowedSpeedsMHz";
const char FIRMWARE_REVISION[] = "FirmwareRevision";
const char FIRMWARE_API_VERSION[] = "FirmwareApiVersion";
const char FUNCTION_CLASSES[] = "FunctionClasses";
const char SUBSYSTEM_VENDOR_ID[] = "SubsystemVendorId";
const char SUBSYSTEM_DEVICE_ID[] = "SubsystemDeviceId";
const char RANK_COUNT[] = "RankCount";
const char DEVICE_LOCATOR[] = "DeviceLocator";
const char MEMORY_LOCATION[] = "MemoryLocation";
const char SOCKET[] = "Socket";
const char MEMORY_CONTROLLER[] = "MemoryController";
const char CHANNEL[] = "Channel";
const char SLOT[] = "Slot";
const char ERROR_CORRECTION[] = "ErrorCorrection";
const char OPERATING_SPEED_MHZ[] = "OperatingSpeedMhz";
const char REGIONS[] = "Regions";
const char REGION_ID[] = "RegionId";
const char MEMORY_CLASSIFICATION[] = "MemoryClassification";
const char OFFSET_MIB[] = "OffsetMiB";
const char SIZE_MIB[] = "SizeMiB";
const char OPERATING_MEMORY_MODES[] = "OperatingMemoryModes";
const char METRICS[] = "Metrics";
const char VOLTAGE_VOLT[] = "VoltageVolt";
const char VENDOR_ID[] = "VendorID";
const char DEVICE_ID[] = "DeviceID";
}

namespace Processor {
const char SOCKET[] = "Socket";
const char PROC_TYPE[] = "ProcessorType";
const char BRAND[] = "Brand";
const char CAPABILITIES[] = "Capabilities";
const char PROC_ARCHITECTURE[] = "ProcessorArchitecture";
const char INSTRUCTION_SET[] = "InstructionSet";
const char PROC_ID[] = "ProcessorId";
const char IDENTIFICATION_REGISTERS[] = "IdentificationRegisters";
const char EFFECTIVE_FAMILY[] = "EffectiveFamily";
const char EFFECTIVE_MODEL[] = "EffectiveModel";
const char STEP[] = "Step";
const char MICROCODE_INFO[] = "MicrocodeInfo";
const char MAX_SPEED[] = "MaxSpeedMHz";
const char TOTAL_CORES[] = "TotalCores";
const char TOTAL_THREADS[] = "TotalThreads";
}

namespace StorageAdapter {
const char INTERFACE[] = "Interface";
const char BUS_INFO[] = "BusInfo";
const char DEVICES[] = "Devices";
}

namespace StorageSubsystem {
const char STORAGE_CONTROLLERS[] = "StorageControllers";
const char STORAGE_CONTROLLERS_POINTER[] = "#/StorageControllers";
const char SKU[] = "SKU";
const char SPEED_GBPS[] = "Speed";
const char FIRMWARE_VERSION[] = "FirmwareVersion";
const char SUPPORTED_CONTROLLER_PROTOCOLS[] = "SupportedControllerProtocols";
const char SUPPORTED_DEVICE_PROTOCOLS[] = "SupportedDeviceProtocols";
const char IDENTIFIERS[] = "Identifiers";
const char DURABLE_NAME[] = "DurableName";
const char DURABLE_NAME_FORMAT[] = "DurableNameFormat";
const char DRIVES[] = "Drives";
const char VOLUMES[] = "Volumes";
const char ENCLOSURES[] = "Enclosures";
const char ASSET_TAG[] = "AssetTag";
}

namespace Drive {
const char INTERFACE[] = "Interface";
const char CAPACITY[] = "CapacityGiB";
const char TYPE[] = "Type";
const char RPM[] = "RPM";
const char BUS_INFO[] = "BusInfo";
const char USED_BY[] = "UsedBy";
}

namespace NetworkInterface {
const char NETWORK_DEVICE_FUNCTIONS[] = "NetworkDeviceFunctions";
}

namespace NetworkDeviceFunction {
const char DEVICE_ENABLED[] = "DeviceEnabled";
const char ETHERNET[] = "Ethernet";
const char ISCSI_BOOT[] = "iSCSIBoot";
const char IP_ADDRESS_TYPE[] = "IPAddressType";
const char INITIATOR_IP_ADDRESS[] = "InitiatorIPAddress";
const char INITIATOR_NAME[] = "InitiatorName";
const char INITIATOR_DEFAULT_GATEWAY[] = "InitiatorDefaultGateway";
const char INITIATOR_NETMASK[] = "InitiatorNetmask";
const char TARGET_INFO_VIA_DHCP[] = "TargetInfoViaDHCP";
const char PRIMARY_TARGET_NAME[] = "PrimaryTargetName";
const char PRIMARY_TARGET_IP_ADDRESS[] = "PrimaryTargetIPAddress";
const char PRIMARY_TARGET_TCP_PORT[] = "PrimaryTargetTCPPort";
const char PRIMARY_LUN[] = "PrimaryLUN";
const char PRIMARY_VLAN_ENABLE[] = "PrimaryVLANEnable";
const char PRIMARY_VLAN_ID[] = "PrimaryVLANId";
const char PRIMARY_DNS[] = "PrimaryDNS";
const char SECONDARY_TARGET_NAME[] = "SecondaryTargetName";
const char SECONDARY_TARGET_IP_ADDRESS[] = "SecondaryTargetIPAddress";
const char SECONDARY_TARGET_TCP_PORT[] = "SecondaryTargetTCPPort";
const char SECONDARY_LUN[] = "SecondaryLUN";
const char SECONDARY_VLAN_ENABLE[] = "SecondaryVLANEnable";
const char SECONDARY_VLAN_ID[] = "SecondaryVLANId";
const char SECONDARY_DNS[] = "SecondaryDNS";
const char IP_MASK_DNS_VIA_DHCP[] = "IPMaskDNSViaDHCP";
const char ROUTER_ADVERTISEMENT_ENABLED[] = "RouterAdvertisementEnabled";
const char AUTHENTICATION_METHOD[] = "AuthenticationMethod";
const char CHAP_USERNAME[] = "CHAPUsername";
const char CHAP_SECRET[] = "CHAPSecret";
const char MUTUAL_CHAP_USERNAME[] = "MutualCHAPUsername";
const char MUTUAL_CHAP_SECRET[] = "MutualCHAPSecret";
}

}
}
}
