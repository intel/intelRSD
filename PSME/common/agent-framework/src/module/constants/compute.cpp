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
 *
 *
 * @file compute.cpp
 * @brief Contains constants used by compute agents
 * */

#include "agent-framework/module/constants/compute.hpp"



namespace agent_framework {
namespace model {
namespace literals {

constexpr const char System::SYSTEM[];
constexpr const char System::TYPE[];
constexpr const char System::STATUS[];
constexpr const char System::BIOS_VERSION[];
constexpr const char System::BOOT_OVERRIDE[];
constexpr const char System::BOOT_OVERRIDE_MODE[];
constexpr const char System::BOOT_OVERRIDE_TARGET[];
constexpr const char System::BOOT_OVERRIDE_SUPPORTED[];
constexpr const char System::UEFI_TARGET[];
constexpr const char System::POWER_STATE[];
constexpr const char System::USB_DEVICES[];
constexpr const char System::PCI_DEVICES[];
constexpr const char System::FRU_INFO[];
constexpr const char System::SKU[];
constexpr const char System::ASSET_TAG[];
constexpr const char System::INDICATOR_LED[];
constexpr const char System::COLLECTIONS[];
constexpr const char System::CHASSIS[];
constexpr const char System::OEM[];
constexpr const char System::GUID[];
constexpr const char System::CABLE_IDS[];

constexpr const char CpuId::VENDOR_ID[];
constexpr const char CpuId::NUMERIC_ID[];
constexpr const char CpuId::FAMILY[];
constexpr const char CpuId::MODEL[];
constexpr const char CpuId::STEP[];
constexpr const char CpuId::MICROCODE_INFO[];

constexpr const char Processor::PROCESSOR[];
constexpr const char Processor::STATUS[];
constexpr const char Processor::SOCKET[];
constexpr const char Processor::TYPE[];
constexpr const char Processor::ARCHITECTURE[];
constexpr const char Processor::INSTRUCTION_SET[];
constexpr const char Processor::MANUFACTURER[];
constexpr const char Processor::MODEL[];
constexpr const char Processor::MODEL_NAME[];
constexpr const char Processor::CPU_ID[];
constexpr const char Processor::MAX_SPEED[];
constexpr const char Processor::TOTAL_CORES[];
constexpr const char Processor::ENABLED_CORES[];
constexpr const char Processor::TOTAL_THREADS[];
constexpr const char Processor::ENABLED_THREADS[];
constexpr const char Processor::CAPABILITIES[];
constexpr const char Processor::OEM[];

constexpr const char StorageController::CONTROLLER[];
constexpr const char StorageController::STATUS[];
constexpr const char StorageController::FRU_INFO[];
constexpr const char StorageController::COLLECTIONS[];
constexpr const char StorageController::PHYSICAL_ID[];
constexpr const char StorageController::OEM[];
constexpr const char StorageController::ASSET_TAG[];
constexpr const char StorageController::SKU[];
constexpr const char StorageController::FIRMWARE_VERSION[];
constexpr const char StorageController::SPEED_GBPS[];
constexpr const char StorageController::SUPPORTED_CONTROLLER_PROTOCOLS[];
constexpr const char StorageController::SUPPORTED_DEVICE_PROTOCOLS[];
 constexpr const char StorageController::IDENTIFIERS[];
// @TO DO  Local storage: to delete.
constexpr const char StorageController::INTERFACE[];

constexpr const char Memory::MEMORY[];
constexpr const char Memory::STATUS[];
constexpr const char Memory::MEMORY_TYPE[];
constexpr const char Memory::DEVICE_TYPE[];
constexpr const char Memory::MODULE_TYPE[];
constexpr const char Memory::MEDIA[];
constexpr const char Memory::MEMORY_MODES[];
constexpr const char Memory::CAPACITY_MB[];
constexpr const char Memory::DATA_WIDTH_BITS[];
constexpr const char Memory::BUS_WIDTH_BITS[];
constexpr const char Memory::FRU_INFO[];
constexpr const char Memory::FIRMWARE_REVISION[];
constexpr const char Memory::FIRMWARE_API_VERSION[];
constexpr const char Memory::MEMORY_CLASSES[];
constexpr const char Memory::VENDOR_ID[];
constexpr const char Memory::DEVICE_ID[];
constexpr const char Memory::OPERATING_SPEED_MHZ[];
constexpr const char Memory::ALLOWED_SPEEDS_MHZ[];
constexpr const char Memory::VOLTAGE[];
constexpr const char Memory::MIN_VOLTAGE[];
constexpr const char Memory::MAX_VOLTAGE[];
constexpr const char Memory::DEVICE_LOCATOR[];
constexpr const char Memory::LOCATION[];
constexpr const char Memory::RANK_COUNT[];
constexpr const char Memory::ERROR_CORRECTION[];
constexpr const char Memory::REGIONS[];
constexpr const char Memory::OPERATING_MODES[];
constexpr const char Memory::OEM[];

constexpr const char NetworkInterface::INTERFACE[];
constexpr const char NetworkInterface::STATUS[];
constexpr const char NetworkInterface::FRAME_SIZE[];
constexpr const char NetworkInterface::SPEED[];
constexpr const char NetworkInterface::AUTO_SENSE[];
constexpr const char NetworkInterface::MAC_ADDRESS[];
constexpr const char NetworkInterface::VLAN_ENABLE[];
constexpr const char NetworkInterface::VLAN_COUNT[];
constexpr const char NetworkInterface::FULL_DUPLEX[];
constexpr const char NetworkInterface::IPv4_ADDRESS[];
constexpr const char NetworkInterface::IPv6_ADDRESS[];
constexpr const char NetworkInterface::IPv4_ADDRESSES[];
constexpr const char NetworkInterface::IPv6_ADDRESSES[];
constexpr const char NetworkInterface::IPv6_DEFAULT_GATEWAY[];
constexpr const char NetworkInterface::MAX_IPv6_STATIC_ADDRESSES[];
constexpr const char NetworkInterface::DEFAULT_VLAN[];
constexpr const char NetworkInterface::FACTORY_MAC_ADDRESS[];
constexpr const char NetworkInterface::COLLECTIONS[];
constexpr const char NetworkInterface::OEM[];

constexpr const char NetworkDevice::DEVICE[];
constexpr const char NetworkDevice::STATUS[];
constexpr const char NetworkDevice::COLLECTIONS[];
constexpr const char NetworkDevice::OEM[];

constexpr const char PciDevice::DEVICE[];
constexpr const char PciDevice::VENDOR_ID[];
constexpr const char PciDevice::DEVICE_ID[];

constexpr const char UsbDevice::DEVICE[];
constexpr const char UsbDevice::VENDOR_ID[];
constexpr const char UsbDevice::DEVICE_ID[];

constexpr const char Region::REGION_ID[];
constexpr const char Region::MEMORY_TYPE[];
constexpr const char Region::OFFSET_MB[];
constexpr const char Region::SIZE_MB[];

constexpr const char MemoryLocation::SOCKET[];
constexpr const char MemoryLocation::CONTROLLER[];
constexpr const char MemoryLocation::CHANNEL[];
constexpr const char MemoryLocation::SLOT[];

constexpr const char StorageSubsystem::STORAGE[];
constexpr const char StorageSubsystem::STATUS[];
constexpr const char StorageSubsystem::COLLECTIONS[];
constexpr const char StorageSubsystem::OEM[];
constexpr const char StorageSubsystem::ASSET_TAG[];
constexpr const char StorageSubsystem::ENCRYPTION_KEY[];

constexpr const char NetworkDeviceFunction::FUNCTION[];
constexpr const char NetworkDeviceFunction::STATUS[];
constexpr const char NetworkDeviceFunction::DEVICE_ENABLED[];
constexpr const char NetworkDeviceFunction::ETHERNET[];
constexpr const char NetworkDeviceFunction::MAC_ADDRESS[];
constexpr const char NetworkDeviceFunction::ISCSI_BOOT[];
constexpr const char NetworkDeviceFunction::OEM[];

constexpr const char IscsiBoot::IP_ADDRESS_TYPE[];
constexpr const char IscsiBoot::INITIATOR_IP_ADDRESS[];
constexpr const char IscsiBoot::INITIATOR_NAME[];
constexpr const char IscsiBoot::INITIATOR_DEFAULT_GATEWAY[];
constexpr const char IscsiBoot::INITIATOR_NETMASK[];
constexpr const char IscsiBoot::TARGET_INFO_VIA_DHCP[];
constexpr const char IscsiBoot::PRIMARY_TARGET_NAME[];
constexpr const char IscsiBoot::PRIMARY_TARGET_IP_ADDRESS[];
constexpr const char IscsiBoot::PRIMARY_TARGET_TCP_PORT[];
constexpr const char IscsiBoot::PRIMARY_LUN[];
constexpr const char IscsiBoot::PRIMARY_VLAN_ENABLE[];
constexpr const char IscsiBoot::PRIMARY_VLAN_ID[];
constexpr const char IscsiBoot::PRIMARY_DNS[];
constexpr const char IscsiBoot::SECONDARY_TARGET_NAME[];
constexpr const char IscsiBoot::SECONDARY_TARGET_IP_ADDRESS[];
constexpr const char IscsiBoot::SECONDARY_TARGET_TCP_PORT[];
constexpr const char IscsiBoot::SECONDARY_LUN[];
constexpr const char IscsiBoot::SECONDARY_VLAN_ENABLE[];
constexpr const char IscsiBoot::SECONDARY_VLAN_ID[];
constexpr const char IscsiBoot::SECONDARY_DNS[];
constexpr const char IscsiBoot::IP_MASK_DNS_VIA_DHCP[];
constexpr const char IscsiBoot::ROUTER_ADVERTISEMENT_ENABLED[];
constexpr const char IscsiBoot::AUTHENTICATION_METHOD[];
constexpr const char IscsiBoot::CHAP_USERNAME[];
constexpr const char IscsiBoot::CHAP_SECRET[];
constexpr const char IscsiBoot::MUTUAL_CHAP_USERNAME[];
constexpr const char IscsiBoot::MUTUAL_CHAP_SECRET[];

}
}
}
