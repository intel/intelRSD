/*!
 * @copyright
 * Copyright (c) 2015-2018 Intel Corporation
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
 *
 *
 * @file compute.hpp
 * @brief Contains constants used by compute agents
 * */

#pragma once

namespace agent_framework {
namespace model {
namespace literals {

/*!
 * @brief Class consisting of literals for System model objects
 */
class System {
public:
    static constexpr const char SYSTEM[] = "system";
    static constexpr const char STATUS[] = "status";
    static constexpr const char TYPE[] = "type";
    static constexpr const char BIOS_VERSION[] = "biosVersion";
    static constexpr const char BOOT_OVERRIDE[] = "bootOverride";
    static constexpr const char BOOT_OVERRIDE_MODE[] = "bootOverrideMode";
    static constexpr const char BOOT_OVERRIDE_TARGET[] = "bootOverrideTarget";
    static constexpr const char BOOT_OVERRIDE_SUPPORTED[] = "bootOverrideSupported";
    static constexpr const char UEFI_TARGET[] = "uefiTarget";
    static constexpr const char POWER_STATE[] = "powerState";
    static constexpr const char USB_DEVICES[] = "usbDevices";
    static constexpr const char PCI_DEVICES[] = "pciDevices";
    static constexpr const char FRU_INFO[] = "fruInfo";
    static constexpr const char SKU[] = "sku";
    static constexpr const char ASSET_TAG[] = "assetTag";
    static constexpr const char INDICATOR_LED[] = "indicatorLED";
    static constexpr const char COLLECTIONS[] = "collections";
    static constexpr const char CHASSIS[] = "chassis";
    static constexpr const char OEM[] = "oem";
    static constexpr const char GUID[] = "systemGuid";
    static constexpr const char CABLE_IDS[] = "cableIds";
    static constexpr const char TXT_ENABLED[] = "txtEnabled";
    static constexpr const char USER_MODE_ENABLED[] = "userModeEnabled";
};

/*!
 * @brief Class consisting of literals for Fpga model objects
 */
class Fpga {
public:
    static constexpr char TYPE[] = "type";
    static constexpr char BIT_STREAM_VERSION[] = "bitStreamVersion";
    static constexpr char HSSI_CONFIGURATION[] = "hssiConfiguration";
    static constexpr char HSSI_SIDEBAND[] = "hssiSideband";
    static constexpr char RECONFIGURATION_SLOTS[] = "reconfigurationSlots";
};

/*!
 * @brief Class consisting of literals for CpuId model objects
 */
class CpuId {
public:
    static constexpr const char VENDOR_ID[] = "vendorId";
    static constexpr const char NUMERIC_ID[] = "numericId";
    static constexpr const char FAMILY[] = "family";
    static constexpr const char MODEL[] = "model";
    static constexpr const char STEP[] = "step";
    static constexpr const char MICROCODE_INFO[] = "microcodeInfo";
};

/*!
 * @brief Class consisting of literals for ExtendedCpuId attribute objects
 */
class ExtendedCpuId {
public:
    static constexpr const char EAX_00H[] = "eax00h";
    static constexpr const char EAX_01H[] = "eax01h";
    static constexpr const char EAX_02H[] = "eax02h";
    static constexpr const char EAX_03H[] = "eax03h";
    static constexpr const char EAX_04H[] = "eax04h";
    static constexpr const char EAX_05H[] = "eax05h";
    static constexpr const char EAX_06H[] = "eax06h";
    static constexpr const char EAX_07H[] = "eax07h";
    static constexpr const char EAX_09H[] = "eax09h";
    static constexpr const char EAX_0aH[] = "eax0ah";
    static constexpr const char EAX_0bH[] = "eax0bh";
    static constexpr const char EAX_0dH[] = "eax0dh";
    static constexpr const char EAX_0fH[] = "eax0fh";
    static constexpr const char EAX_10H[] = "eax10h";
    static constexpr const char EAX_14H[] = "eax14h";
    static constexpr const char EAX_15H[] = "eax15h";
    static constexpr const char EAX_16H[] = "eax16h";
    static constexpr const char EAX_17H_ECX_00H[] = "eax17hecx00h";
    static constexpr const char EAX_17H_ECX_01H[] = "eax17hecx01h";
    static constexpr const char EAX_17H_ECX_02H[] = "eax17hecx02h";
    static constexpr const char EAX_17H_ECX_03H[] = "eax17hecx03h";
    static constexpr const char EAX_80000000H[] = "eax80000000h";
    static constexpr const char EAX_80000001H[] = "eax80000001h";
    static constexpr const char EAX_80000002H[] = "eax80000002h";
    static constexpr const char EAX_80000003H[] = "eax80000003h";
    static constexpr const char EAX_80000004H[] = "eax80000004h";
    static constexpr const char EAX_80000006H[] = "eax80000006h";
    static constexpr const char EAX_80000007H[] = "eax80000007h";
    static constexpr const char EAX_80000008H[] = "eax80000008h";
};

/*!
 * @brief Class consisting of literals for Processor model objects
 */
class Processor {
public:
    static constexpr const char PROCESSOR[] = "processor";
    static constexpr const char STATUS[] = "status";
    static constexpr const char SOCKET[] = "socket";
    static constexpr const char TYPE[] = "processorType";
    static constexpr const char ARCHITECTURE[] = "processorArchitecture";
    static constexpr const char INSTRUCTION_SET[] = "instructionSet";
    static constexpr const char MANUFACTURER[] = "manufacturer";
    static constexpr const char MODEL[] = "model";
    static constexpr const char MODEL_NAME[] = "modelName";
    static constexpr const char CPU_ID[] = "cpuId";
    static constexpr const char EXTENDED_CPU_ID[] = "extendedCpuId";
    static constexpr const char MAX_SPEED[] = "maxSpeedMHz";
    static constexpr const char TOTAL_CORES[] = "totalCores";
    static constexpr const char ENABLED_CORES[] = "enabledCores";
    static constexpr const char TOTAL_THREADS[] = "totalThreads";
    static constexpr const char ENABLED_THREADS[] = "enabledThreads";
    static constexpr const char CAPABILITIES[] = "capabilities";
    static constexpr const char THERMAL_DESIGN_POWER_WATT[] = "thermalDesignPowerWatt";
    static constexpr const char FPGA[] = "fpga";
    static constexpr const char ON_PACKAGE_MEMORY[] = "onPackageMemory";
    static constexpr const char OEM[] = "oem";
};

/*!
 * @brief Class consisting of literals for StorageController model objects
 */
class StorageController {
public:
    static constexpr const char CONTROLLER[] = "controller";
    static constexpr const char STATUS[] = "status";
    static constexpr const char FRU_INFO[] = "fruInfo";
    static constexpr const char PHYSICAL_ID[] = "physicalId";
    static constexpr const char COLLECTIONS[] = "collections";
    static constexpr const char OEM[] = "oem";
    static constexpr const char ASSET_TAG[] = "assetTag";
    static constexpr const char SKU[] = "sku";
    static constexpr const char FIRMWARE_VERSION[] = "firmwareVersion";
    static constexpr const char SPEED_GBPS[] = "speedGbps";
    static constexpr const char SUPPORTED_CONTROLLER_PROTOCOLS[] = "supportedControllerProtocols";
    static constexpr const char SUPPORTED_DEVICE_PROTOCOLS[] = "supportedDeviceProtocols";
    static constexpr const char IDENTIFIERS[] = "identifiers";

    // @TO DO  Local storage: to delete.
    static constexpr const char INTERFACE[] = "interface";
};

/*!
 * @brief Class consisting of literals for Memory model objects
 */
class Memory {
public:
    static constexpr const char MEMORY[] = "memory";
    static constexpr const char STATUS[] = "status";
    static constexpr const char MEMORY_TYPE[] = "memoryType";
    static constexpr const char DEVICE_TYPE[] = "deviceType";
    static constexpr const char MODULE_TYPE[] = "moduleType";
    static constexpr const char MEDIA[] = "media";
    static constexpr const char OPERATING_MEMORY_MODES[] = "operatingMemoryModes";
    static constexpr const char CAPACITY_MB[] = "capacityMB";
    static constexpr const char DATA_WIDTH_BITS[] = "dataWidthBits";
    static constexpr const char BUS_WIDTH_BITS[] = "busWidthBits";
    static constexpr const char FRU_INFO[] = "fruInfo";
    static constexpr const char FIRMWARE_REVISION[] = "firmwareRevision";
    static constexpr const char FIRMWARE_API_VERSION[] = "firmwareApiVersion";
    static constexpr const char MEMORY_CLASSES[] = "functionClasses";
    static constexpr const char VENDOR_ID[] = "vendorID";
    static constexpr const char DEVICE_ID[] = "deviceID";
    static constexpr const char OPERATING_SPEED_MHZ[] = "operatingSpeedMHz";
    static constexpr const char ALLOWED_SPEEDS_MHZ[] = "allowedSpeedsMHz";
    static constexpr const char VOLTAGE[] = "voltageVolt";
    static constexpr const char MIN_VOLTAGE[] = "minimumVoltageVolt";
    static constexpr const char MAX_VOLTAGE[] = "maximumVoltageVolt";
    static constexpr const char DEVICE_LOCATOR[] = "deviceLocator";
    static constexpr const char LOCATION[] = "location";
    static constexpr const char RANK_COUNT[] = "rankCount";
    static constexpr const char ERROR_CORRECTION[] = "errorCorrection";
    static constexpr const char REGIONS[] = "regions";
    static constexpr const char OPERATING_MODES[] = "operatingModes";
    static constexpr const char OEM[] = "oem";
};

/*!
 * @brief Class consisting of literals for NetworkDevice model objects
 */
class NetworkDevice {
public:
    static constexpr const char DEVICE[] = "device";
    static constexpr const char STATUS[] = "status";
    static constexpr const char COLLECTIONS[] = "collections";
    static constexpr const char OEM[] = "oem";
};

/*!
 * @brief Class consisting of literals for PciDevice model objects
 */
class PciDevice {
public:
    static constexpr const char DEVICE[] = "pciDevice";
    static constexpr const char VENDOR_ID[] = "vendorId";
    static constexpr const char DEVICE_ID[] = "deviceId";
};

/*!
 * @brief Class consisting of literals for UsbDevice model objects
 */
class UsbDevice {
public:
    static constexpr const char DEVICE[] = "usbDevice";
    static constexpr const char VENDOR_ID[] = "vendorId";
    static constexpr const char DEVICE_ID[] = "deviceId";
};

/*!
 * @brief Class consisting of literals for Regions in Dimms model objects
 */
class Region {
public:
    static constexpr const char REGION_ID[] = "regionId";
    static constexpr const char MEMORY_TYPE[] = "memoryType";
    static constexpr const char OFFSET_MB[] = "offsetMB";
    static constexpr const char SIZE_MB[] = "sizeMB";
};

/*!
 * @brief Class consisting of literals for MemoryLocation in Memory model objects
 */
class MemoryLocation {
public:
    static constexpr const char SOCKET[] = "socket";
    static constexpr const char CONTROLLER[] = "controller";
    static constexpr const char CHANNEL[] = "channel";
    static constexpr const char SLOT[] = "slot";
};

/*!
 * @brief Class consisting of literals for StorageSubsystem model objects
 */
class StorageSubsystem {
public:
    static constexpr const char STORAGE[] = "storage";
    static constexpr const char STATUS[] = "status";
    static constexpr const char COLLECTIONS[] = "collections";
    static constexpr const char OEM[] = "oem";
    static constexpr const char ASSET_TAG[] = "assetTag";
    static constexpr const char ENCRYPTION_KEY[] = "encryptionKey";
};

/*!
 * @brief Class consisting of literals for TrustedModule attribute in System objects
 */
class TrustedModule {
public:
    static constexpr const char INTERFACE_TYPE[] = "interfaceType";
    static constexpr const char FIRMWARE_VERSION[] = "firmwareVersion";
    static constexpr const char TRUSTED_MODULE[] = "trustedModule";
    static constexpr const char CLEAR_OWNERSHIP[] = "clearOwnership";
    static constexpr const char DEVICE_ENABLED[] = "deviceEnabled";
    static constexpr const char OEM[] = "oem";
};

/*!
 * @brief Class consisting of literals for NetworkDeviceFunction model objects
 */
class NetworkDeviceFunction {
public:
    static constexpr const char FUNCTION[] = "function";
    static constexpr const char STATUS[] = "status";
    static constexpr const char DEVICE_ENABLED[] = "deviceEnabled";
    static constexpr const char ETHERNET[] = "ethernet";
    static constexpr const char MAC_ADDRESS[] = "macAddress";
    static constexpr const char ISCSI_BOOT[] = "iSCSIBoot";
    static constexpr const char OEM[] = "oem";
};

/*!
 * @brief Class consisting of literals for IscsiBoot model attribute
 */
class IscsiBoot {
public:
    static constexpr const char IP_ADDRESS_TYPE[] = "ipAddressType";
    static constexpr const char INITIATOR_IP_ADDRESS[] = "initiatorIpAddress";
    static constexpr const char INITIATOR_NAME[] = "initiatorName";
    static constexpr const char INITIATOR_DEFAULT_GATEWAY[] = "initiatorDefaultGateway";
    static constexpr const char INITIATOR_NETMASK[] = "initiatorNetmask";
    static constexpr const char TARGET_INFO_VIA_DHCP[] = "targetInfoViaDhcp";
    static constexpr const char PRIMARY_TARGET_NAME[] = "primaryTargetName";
    static constexpr const char PRIMARY_TARGET_IP_ADDRESS[] = "primaryTargetIpAddress";
    static constexpr const char PRIMARY_TARGET_TCP_PORT[] = "primaryTargetTcpPort";
    static constexpr const char PRIMARY_LUN[] = "primaryLun";
    static constexpr const char PRIMARY_VLAN_ENABLE[] = "primaryVlanEnable";
    static constexpr const char PRIMARY_VLAN_ID[] = "primaryVlanId";
    static constexpr const char PRIMARY_DNS[] = "primaryDns";
    static constexpr const char SECONDARY_TARGET_NAME[] = "secondaryTargetName";
    static constexpr const char SECONDARY_TARGET_IP_ADDRESS[] = "secondaryTargetIpAddress";
    static constexpr const char SECONDARY_TARGET_TCP_PORT[] = "secondaryTargetTcpPort";
    static constexpr const char SECONDARY_LUN[] = "secondaryLun";
    static constexpr const char SECONDARY_VLAN_ENABLE[] = "secondaryVlanEnable";
    static constexpr const char SECONDARY_VLAN_ID[] = "secondaryVlanId";
    static constexpr const char SECONDARY_DNS[] = "secondaryDns";
    static constexpr const char IP_MASK_DNS_VIA_DHCP[] = "ipMaskDnsViaDhcp";
    static constexpr const char ROUTER_ADVERTISEMENT_ENABLED[] = "routerAdvertisementEnabled";
    static constexpr const char AUTHENTICATION_METHOD[] = "authenticationMethod";
    static constexpr const char CHAP_USERNAME[] = "chapUsername";
    static constexpr const char CHAP_SECRET[] = "chapSecret";
    static constexpr const char MUTUAL_CHAP_USERNAME[] = "mutualChapUsername";
    static constexpr const char MUTUAL_CHAP_SECRET[] = "mutualChapSecret";
};

}
}
}
