/*!
 * @copyright
 * Copyright (c) 2015-2019 Intel Corporation
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

#pragma once
namespace psme {
namespace rest {
namespace constants {

/*!
 * @brief Constant literals for System endpoint.
 */
namespace System {
extern const char SYSTEM_TYPE[];
extern const char HOST_NAME[];
extern const char INDICATOR_LED[];
extern const char POWER_STATE[];
extern const char BOOT[];
extern const char BOOT_MODE[];
extern const char BOOT_SOURCE_OVERRIDE_ENABLED[];
extern const char BOOT_SOURCE_OVERRIDE_TARGET[];
extern const char BOOT_SOURCE_OVERRIDE_TARGET_ALLOWABLE_VALUES[];
extern const char BOOT_SOURCE_OVERRIDE_MODE[];
extern const char BOOT_SOURCE_OVERRIDE_MODE_ALLOWABLE_VALUES[];
extern const char PXE_PARAMETERS[];
extern const char ETHERNET_INTERFACE[];
extern const char ETHERNET_INTERFACES[];
extern const char NETWORK_INTERFACES[];
extern const char UEFI_TARGET[];
extern const char ISCSI_PARAMETERS[];
extern const char AUTH_METHOD[];
extern const char BOOT_VLAN[];
extern const char CHAP_MUTUAL_SECRET[];
extern const char CHAP_MUTUAL_USERNAME[];
extern const char CHAP_SECRET[];
extern const char CHAP_USERNAME[];
extern const char IP_VERSION[];
extern const char INITIATOR_IP[];
extern const char INITIATOR_IQN[];
extern const char TARGET_IQN[];
extern const char TARGET_LUN[];
extern const char TARGET_PORTAL_IP[];
extern const char TARGET_PORTAL_PORT[];
extern const char BIOS_VERSION[];
extern const char PROCESSOR_SUMMARY[];
extern const char COUNT[];
extern const char MEMORY_SUMMARY[];
extern const char TOTAL_SYSTEM_MEMORY_GIB[];
extern const char PROCESSORS[];
extern const char SIMPLE_STORAGE[];
extern const char MEMORY[];
extern const char ENDPOINTS[];
extern const char HASH_COMPUTER_SYSTEM_RESET[];
extern const char COMPUTER_SYSTEM_RESET[];
extern const char HASH_INTEL_OEM_CHANGE_TPM_STATE[];
extern const char INTEL_OEM_CHANGE_TPM_STATE[];
extern const char HASH_INTEL_OEM_ERASE_OPTANE_DC_PERSISTENT_MEMORY[];
extern const char INTEL_OEM_ERASE_OPTANE_DC_PERSISTENT_MEMORY[];
extern const char ALLOWABLE_DCPMEM_RESET_CONFIGURATION_VALUES[];
extern const char ALLOWABLE_DCPMEM_ERASE_CONFIGURATION_KEY_VALUES[];
extern const char RESET_CONFIGURATION[];
extern const char ERASE_CONFIGURATION_KEYS[];
extern const char MEMORY_SOCKETS[];
extern const char PCI_DEVICES[];
extern const char PCIE_CONNECTION_ID[];
extern const char PROCESSOR_SOCKETS[];
extern const char TRUSTED_MODULES[];
extern const char FIRMWARE_VERSION_2[];
extern const char INTERFACE_TYPE_SELECTION[];
extern const char OEM_METHOD[];
extern const char USER_MODE_ENABLED[];
extern const char TRUSTED_EXECUTION_TECHNOLOGY_ENABLED[];
extern const char STORAGE_ADAPTERS[];
extern const char STORAGE[];
extern const char INTERFACE_TYPE[];
extern const char DEVICE_ENABLED[];
extern const char CLEAR_OWNERSHIP[];
extern const char ALLOWABLE_INTERFACE_TYPE[];
extern const char HOSTED_SERVICES[];
extern const char STORAGE_SERVICES[];
extern const char HOSTING_ROLES[];

}

/*!
 * @brief Constant literals for System Metrics endpoint.
 */
namespace SystemMetrics {
extern const char PROCESSOR_BANDWIDTH_PERCENT[];
extern const char IO_BANDWIDTH_GBPS[];
extern const char MEMORY_BANDWIDTH_PERCENT[];
extern const char MEMORY_THROTTLED_CYCLES_PERCENT[];
extern const char MEMORY_POWER_WATT[];
extern const char PROCESSOR_POWER_WATT[];
extern const char HEALTH[];
}

/*!
 * @brief Constant literals for Memory endpoint.
 */
namespace Memory {
extern const char MEMORY_TYPE[];
extern const char MEMORY_DEVICE_TYPE[];
extern const char BASE_MODULE_TYPE[];
extern const char MEMORY_MEDIA[];
extern const char CAPACITY_MIB[];
extern const char DATA_WIDTH_BITS[];
extern const char BUS_WIDTH_BITS[];
extern const char ALLOWED_SPEEDS_MHZ[];
extern const char FIRMWARE_REVISION[];
extern const char FIRMWARE_API_VERSION[];
extern const char RANK_COUNT[];
extern const char DEVICE_LOCATOR[];
extern const char MEMORY_LOCATION[];
extern const char SOCKET[];
extern const char MEMORY_CONTROLLER[];
extern const char CHANNEL[];
extern const char SLOT[];
extern const char ERROR_CORRECTION[];
extern const char OPERATING_SPEED_MHZ[];
extern const char REGIONS[];
extern const char REGION_ID[];
extern const char MEMORY_CLASSIFICATION[];
extern const char OFFSET_MIB[];
extern const char SIZE_MIB[];
extern const char OPERATING_MEMORY_MODES[];
extern const char VOLTAGE_VOLT[];
extern const char MODULE_MANUFACTURER_ID[];
extern const char MODULE_PRODUCT_ID[];
extern const char MEMORY_SUBSYSTEM_CONTROLLER_MANUFACTURER_ID[];
extern const char MEMORY_SUBSYSTEM_CONTROLLER_PRODUCT_ID[];
extern const char SECURITY_CAPABILITIES[];
extern const char PASSPHRASE_CAPABLE[];
extern const char MAX_PASSPHRASE_COUNT[];
extern const char SECURITY_STATES[];
extern const char SPARE_DEVICE_COUNT[];
extern const char LOGICAL_SIZE_MIB[];
extern const char VOLATILE_SIZE_MIB[];
extern const char NON_VOLATILE_SIZE_MIB[];
extern const char VOLATILE_REGION_SIZE_LIMIT_MIB[];
extern const char PERSISTENT_REGION_SIZE_LIMIT_MIB[];
extern const char POWER_MANAGEMENT_POLICY[];
extern const char POLICY_ENABLED[];
extern const char MAX_TDP_MILLIWATTS[];
extern const char AVERAGE_POWER_BUDGET_MILLIWATTS[];
extern const char PEAK_POWER_BUDGET_MILLIWATTS[];
}

/*!
 * @brief Constant literals for Memory Metrics endpoint.
 */
namespace MemoryMetrics {
extern const char BLOCK_SIZE_BYTES[];
extern const char CURRENT_PERIOD[];
extern const char LIFE_TIME[];
extern const char HEALTH_DATA[];
extern const char BLOCKS_READ[];
extern const char BLOCKS_WRITTEN[];
extern const char REMAINING_SPARE_BLOCK_PERCENTAGE[];
extern const char LAST_SHUTDOWN_SUCCESS[];
extern const char DATA_LOSS_DETECTED[];
extern const char PERFORMANCE_DEGRADED[];
extern const char PREDICTED_MEDIA_LIFE_LEFT_PERCENT[];
extern const char ALARM_TRIPS[];
extern const char TEMPERATURE[];
extern const char SPARE_BLOCK[];
extern const char UNCORRECTABLE_ECC_ERROR[];
extern const char CORRECTABLE_ECC_ERROR[];
extern const char ADDRESS_PARITY_ERROR[];
extern const char TEMPERATURE_CELSIUS[];
extern const char CONTROLLER_TEMPERATURE_CELSIUS[];
extern const char BANDWIDTH_PERCENT[];
extern const char THROTTLED_CYCLES_PERCENT[];
extern const char CONSUMED_POWER_WATT[];
extern const char THERMAL_MARGIN_CELSIUS[];
extern const char HEALTH[];
extern const char UPTIME_SECONDS[];
extern const char HOST_READ_REQUESTS[];
extern const char HOST_WRITE_REQUESTS[];
extern const char ECC_CORRECTED_ERRORS[];
extern const char UNSAFE_SHUTDOWN_COUNT[];
extern const char POWER_CYCLES[];
extern const char POWER_ON_TIME_SECONDS[];
extern const char WRITE_COUNT_MAX[];
extern const char WRITE_COUNT_AVG[];
extern const char MEDIA_ECC_CORRECTED_ERRORS[];
extern const char MEDIA_ECC_UNCORRECTABLE_ERRORS[];
extern const char ECC_UNCORRECTABLE_ERRORS[];
}

/*!
 * @brief Constant literals for Processor endpoint.
 */
namespace Processor {
extern const char SOCKET[];
extern const char PROCESSOR_TYPE[];
extern const char BRAND[];
extern const char CAPABILITIES[];
extern const char THERMAL_DESIGN_POWER_WATT[];
extern const char FPGA[];
extern const char PROCESSOR_ARCHITECTURE[];
extern const char INSTRUCTION_SET[];
extern const char PROCESSOR_ID[];
extern const char IDENTIFICATION_REGISTERS[];
extern const char EFFECTIVE_FAMILY[];
extern const char EFFECTIVE_MODEL[];
extern const char STEP[];
extern const char MICROCODE_INFO[];
extern const char MAX_SPEED[];
extern const char TOTAL_CORES[];
extern const char TOTAL_THREADS[];
extern const char ENDPOINTS[];
extern const char INTEGRATED_MEMORY[];
extern const char MEMORY_TYPE[];
extern const char MEMORY_CAPACITY_MB[];
extern const char SPEED_MHZ[];
extern const char EXTENDED_IDENTIFICATION_REGISTERS[];

}

/*!
 * @brief Constant literals for Processor endpoint, OEM/FPGA property.
 */
namespace Fpga {
extern const char FPGA_TYPE[];
extern const char BIT_STREAM_VERSION[];
extern const char HSSI_CONFIGURATION[];
extern const char HSSI_SIDEBAND[];
extern const char MODEL[];
extern const char FW_ID[];
extern const char FW_MANUFACTURER[];
extern const char FW_VERSION[];
extern const char HOST_INTERFACE[];
extern const char EXTERNAL_INTERFACES[];
extern const char SIDEBAND_INTERFACE[];
extern const char PCIE_VIRTUAL_FUNCTIONS[];
extern const char PROGRAMMABLE_FROM_HOST[];
extern const char RECONFIGURATION_SLOTS[];
extern const char RECONFIGURATION_SLOTS_DETAILS[];
extern const char ERASED[];
extern const char SECURE_ERASE[];
extern const char HASH_SECURE_ERASE[];

}

/*!
 * @brief Constant literals for Processor Extended CPU ID property.
 */
namespace ExtendedCpuId {
extern const char EAX_00H[];
extern const char EAX_01H[];
extern const char EAX_02H[];
extern const char EAX_03H[];
extern const char EAX_04H[];
extern const char EAX_05H[];
extern const char EAX_06H[];
extern const char EAX_07H[];
extern const char EAX_09H[];
extern const char EAX_0AH[];
extern const char EAX_0BH[];
extern const char EAX_0DH[];
extern const char EAX_0FH[];
extern const char EAX_10H[];
extern const char EAX_14H[];
extern const char EAX_15H[];
extern const char EAX_16H[];
extern const char EAX_17H_ECX_00H[];
extern const char EAX_17H_ECX_01H[];
extern const char EAX_17H_ECX_02H[];
extern const char EAX_17H_ECX_03H[];
extern const char EAX_80000000H[];
extern const char EAX_80000001H[];
extern const char EAX_80000002H[];
extern const char EAX_80000003H[];
extern const char EAX_80000004H[];
extern const char EAX_80000006H[];
extern const char EAX_80000007H[];
extern const char EAX_80000008H[];

}

/*!
 * @brief Constant literals for Processor Metrics endpoint.
 */
namespace ProcessorMetrics {
extern const char BANDWIDTH_PERCENT[];
extern const char AVERAGE_FREQUENCY_MHZ[];
extern const char THROTTLING_CELSIUS[];
extern const char TEMPERATURE_CELSIUS[];
extern const char CONSUMED_POWER_WATT[];
extern const char HEALTH[];
}

/*!
 * @brief Constant literals for StorageAdapter endpoint.
 */
namespace StorageAdapter {
extern const char INTERFACE[];
extern const char BUS_INFO[];
extern const char DEVICES[];
}

/*!
 * @brief Constant literals for StorageSubsystem endpoint.
 */
namespace StorageSubsystem {
extern const char STORAGE_CONTROLLERS[];
extern const char SPEED_GBPS[];
extern const char FIRMWARE_VERSION[];
extern const char SUPPORTED_CONTROLLER_PROTOCOLS[];
extern const char SUPPORTED_DEVICE_PROTOCOLS[];
extern const char IDENTIFIERS[];
extern const char DURABLE_NAME[];
extern const char DURABLE_NAME_FORMAT[];
extern const char DRIVES[];
extern const char VOLUMES[];
extern const char ENCLOSURES[];
extern const char ASSET_TAG[];
}

/*!
 * @brief Constant literals for NetworkInterface endpoint.
 */
namespace NetworkInterface {
extern const char NETWORK_DEVICE_FUNCTIONS[];
}

/*!
 * @brief Constant literals for NetworkDeviceFunction endpoint.
 */
namespace NetworkDeviceFunction {
extern const char DEVICE_ENABLED[];
extern const char ETHERNET[];
extern const char ISCSI_BOOT[];
extern const char IP_ADDRESS_TYPE[];
extern const char INITIATOR_IP_ADDRESS[];
extern const char INITIATOR_NAME[];
extern const char INITIATOR_DEFAULT_GATEWAY[];
extern const char INITIATOR_NETMASK[];
extern const char TARGET_INFO_VIA_DHCP[];
extern const char PRIMARY_TARGET_NAME[];
extern const char PRIMARY_TARGET_IP_ADDRESS[];
extern const char PRIMARY_TARGET_TCP_PORT[];
extern const char PRIMARY_LUN[];
extern const char PRIMARY_VLAN_ENABLE[];
extern const char PRIMARY_VLAN_ID[];
extern const char PRIMARY_DNS[];
extern const char SECONDARY_TARGET_NAME[];
extern const char SECONDARY_TARGET_IP_ADDRESS[];
extern const char SECONDARY_TARGET_TCP_PORT[];
extern const char SECONDARY_LUN[];
extern const char SECONDARY_VLAN_ENABLE[];
extern const char SECONDARY_VLAN_ID[];
extern const char SECONDARY_DNS[];
extern const char IP_MASK_DNS_VIA_DHCP[];
extern const char ROUTER_ADVERTISEMENT_ENABLED[];
extern const char AUTHENTICATION_METHOD[];
extern const char CHAP_USERNAME[];
extern const char CHAP_SECRET[];
extern const char MUTUAL_CHAP_USERNAME[];
extern const char MUTUAL_CHAP_SECRET[];
}

/*!
 * @brief Constant literals for System endpoint, PerformanceConfiguration property.
 */
namespace PerformanceConfiguration {
extern const char CONFIGURATION_ID[];
extern const char TYPE[];
extern const char HIGH_PRIORITY_CORE_COUNT_PER_CPU[];
extern const char HIGH_PRIORITY_BASE__CORE_FREQUENCY[];
extern const char LOW_PRIORITY_CORE_COUNT_PER_CPU[];
extern const char LOW_PRIORITY_BASE_CORE_FREQUENCY[];
extern const char ACTIVE_CORES_PER_CPU[];
extern const char BASE_CORE_FREQUENCY[];
extern const char TDP_PER_CPU[];
extern const char MAX_CPU_JUNCTION_TEMP[];
}

}
}
}
