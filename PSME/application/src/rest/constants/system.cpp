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
const char HASH_COMPUTER_SYSTEM_RESET[] = "#ComputerSystem.Reset";
const char COMPUTER_SYSTEM_RESET[] = "ComputerSystem.Reset";
const char HASH_INTEL_OEM_CHANGE_TPM_STATE[] = "#Intel.Oem.ChangeTPMState";
const char INTEL_OEM_CHANGE_TPM_STATE[] = "Intel.Oem.ChangeTPMState";
const char DISCOVERY_STATE[] = "DiscoveryState";
const char BASIC[] = "Basic";
const char MEMORY_SOCKETS[] = "MemorySockets";
const char PCI_DEVICES[] = "PciDevices";
const char PCIE_CONNECTION_ID[] = "PCIeConnectionId";
const char PROCESSOR_SOCKETS[] = "ProcessorSockets";
const char TRUSTED_MODULES[] = "TrustedModules";
const char INTERFACE_TYPE_SELECTION[] = "InterfaceTypeSelection";
const char FIRMWARE_VERSION_2[] = "FirmwareVersion2";
const char OEM_METHOD[] = "OemMethod";
const char USER_MODE_ENABLED[] = "UserModeEnabled";
const char TRUSTED_EXECUTION_TECHNOLOGY_ENABLED[] = "TrustedExecutionTechnologyEnabled";
const char STORAGE_ADAPTERS[] = "Adapters";
const char STORAGE[] = "Storage";
const char INTERFACE_TYPE[] = "InterfaceType";
const char DEVICE_ENABLED[] = "DeviceEnabled";
const char CLEAR_OWNERSHIP[] = "ClearOwnership";
const char ALLOWABLE_INTERFACE_TYPE[] = "InterfaceType@Redfish.AllowableValues";

}

namespace SystemMetrics {
const char PROCESSOR_BANDWIDTH_PERCENT[] = "ProcessorBandwidthPercent";
const char IO_BANDWIDTH_GBPS[] = "IOBandwidthGBps";
const char MEMORY_BANDWIDTH_PERCENT[] = "MemoryBandwidthPercent";
const char MEMORY_THROTTLED_CYCLES_PERCENT[] = "MemoryThrottledCyclesPercent";
const char MEMORY_POWER_WATT[] = "MemoryPowerWatt";
const char PROCESSOR_POWER_WATT[] = "ProcessorPowerWatt";
const char HEALTH[] = "Health";
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

namespace MemoryMetrics {
const char BLOCK_SIZE_BYTES[] = "BlockSizeBytes";
const char CURRENT_PERIOD[] = "CurrentPeriod";
const char LIFE_TIME[] = "LifeTime";
const char HEALTH_DATA[] = "HealthData";
const char BLOCKS_READ[] = "BlocksRead";
const char BLOCKS_WRITTEN[] = "BlocksWritten";
const char REMAINING_SPARE_BLOCK_PERCENTAGE[] = "RemainingSpareBlockPercentage";
const char LAST_SHUTDOWN_SUCCESS[] = "LastShutdownSuccess";
const char DATA_LOSS_DETECTED[] = "DataLossDetected";
const char PERFORMANCE_DEGRADED[] = "PerformanceDegraded";
const char PREDICTED_MEDIA_LIFE_LEFT_PERCENT[] = "PredictedMediaLifeLeftPercent";
const char ALARM_TRIPS[] = "AlarmTrips";
const char TEMPERATURE[] = "Temperature";
const char SPARE_BLOCK[] = "SpareBlock";
const char UNCORRECTABLE_ECC_ERROR[] = "UncorrectableECCError";
const char CORRECTABLE_ECC_ERROR[] = "CorrectableECCError";
const char ADDRESS_PARITY_ERROR[] = "AddressParityError";
const char TEMPERATURE_CELSIUS[] = "TemperatureCelsius";
const char BANDWIDTH_PERCENT[] = "BandwidthPercent";
const char THROTTLED_CYCLES_PERCENT[] = "ThrottledCyclesPercent";
const char CONSUMED_POWER_WATT[] = "ConsumedPowerWatt";
const char THERMAL_MARGIN_CELSIUS[] = "ThermalMarginCelsius";
const char ECC_ERRORS_COUNT[] = "ECCErrorsCount";
const char HEALTH[] = "Health";
}

namespace Processor {
const char SOCKET[] = "Socket";
const char PROCESSOR_TYPE[] = "ProcessorType";
const char BRAND[] = "Brand";
const char CAPABILITIES[] = "Capabilities";
const char ON_PACKAGE_MEMORY[] = "OnPackageMemory";
const char THERMAL_DESIGN_POWER_WATT[] = "ThermalDesignPowerWatt";
const char FPGA[] = "FPGA";
const char PROCESSOR_ARCHITECTURE[] = "ProcessorArchitecture";
const char INSTRUCTION_SET[] = "InstructionSet";
const char PROCESSOR_ID[] = "ProcessorId";
const char IDENTIFICATION_REGISTERS[] = "IdentificationRegisters";
const char EFFECTIVE_FAMILY[] = "EffectiveFamily";
const char EFFECTIVE_MODEL[] = "EffectiveModel";
const char STEP[] = "Step";
const char MICROCODE_INFO[] = "MicrocodeInfo";
const char MAX_SPEED[] = "MaxSpeedMHz";
const char TOTAL_CORES[] = "TotalCores";
const char TOTAL_THREADS[] = "TotalThreads";
const char ENDPOINTS[] = "Endpoints";
const char MEMORY_TYPE[] = "Type";
const char MEMORY_CAPACITY_MB[] = "CapacityMB";
const char SPEED_MHZ[] = "SpeedMHz";
const char FPGA_TYPE[] = "Type";
const char BIT_STREAM_VERSION[] = "BitStreamVersion";
const char HSSI_CONFIGURATION[] = "HSSIConfiguration";
const char HSSI_SIDEBAND[] = "HSSISideband";
const char RECONFIGURATION_SLOTS[] = "ReconfigurationSlots";
const char EXTENDED_IDENTIFICATION_REGISTERS[] = "ExtendedIdentificationRegisters";

}

namespace ExtendedCpuId {
const char EAX_00H[] = "EAX_00h";
const char EAX_01H[] = "EAX_01h";
const char EAX_02H[] = "EAX_02h";
const char EAX_03H[] = "EAX_03h";
const char EAX_04H[] = "EAX_04h";
const char EAX_05H[] = "EAX_05h";
const char EAX_06H[] = "EAX_06h";
const char EAX_07H[] = "EAX_07h";
const char EAX_09H[] = "EAX_09h";
const char EAX_0AH[] = "EAX_0ah";
const char EAX_0BH[] = "EAX_0bh";
const char EAX_0DH[] = "EAX_0dh";
const char EAX_0FH[] = "EAX_0fh";
const char EAX_10H[] = "EAX_10h";
const char EAX_14H[] = "EAX_14h";
const char EAX_15H[] = "EAX_15h";
const char EAX_16H[] = "EAX_16h";
const char EAX_17H_ECX_00H[] = "EAX_17h_00h";
const char EAX_17H_ECX_01H[] = "EAX_17h_01h";
const char EAX_17H_ECX_02H[] = "EAX_17h_02h";
const char EAX_17H_ECX_03H[] = "EAX_17h_03h";
const char EAX_80000000H[] = "EAX_80000000h";
const char EAX_80000001H[] = "EAX_80000001h";
const char EAX_80000002H[] = "EAX_80000002h";
const char EAX_80000003H[] = "EAX_80000003h";
const char EAX_80000004H[] = "EAX_80000004h";
const char EAX_80000006H[] = "EAX_80000006h";
const char EAX_80000007H[] = "EAX_80000007h";
const char EAX_80000008H[] = "EAX_80000008h";
}

namespace ProcessorMetrics {
const char BANDWIDTH_PERCENT[] = "BandwidthPercent";
const char AVERAGE_FREQUENCY_MHZ[] = "AverageFrequencyMHz";
const char THROTTLING_CELSIUS[] = "ThrottlingCelsius";
const char TEMPERATURE_CELSIUS[] = "TemperatureCelsius";
const char CONSUMED_POWER_WATT[] = "ConsumedPowerWatt";
const char HEALTH[] = "Health";
}

namespace StorageAdapter {
const char INTERFACE[] = "Interface";
const char BUS_INFO[] = "BusInfo";
const char DEVICES[] = "Devices";
}

namespace StorageSubsystem {
const char STORAGE_CONTROLLERS[] = "StorageControllers";
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
