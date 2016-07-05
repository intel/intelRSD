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
 *
 *
 * @file compute.cpp
 * @brief Contains constants used by compute agents
 * */

#include "agent-framework/module-ref/constants/compute.hpp"

namespace agent_framework {
namespace model {
namespace literals {

constexpr const char System::SYSTEM[];
constexpr const char System::STATUS[];
constexpr const char System::BIOS_VERSION[];
constexpr const char System::BOOT_OVERRIDE[];
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
constexpr const char StorageController::INTERFACE[];
constexpr const char StorageController::FRU_INFO[];
constexpr const char StorageController::COLLECTIONS[];
constexpr const char StorageController::PHYSICAL_ID[];
constexpr const char StorageController::OEM[];


constexpr const char Dimm::DIMM[];
constexpr const char Dimm::STATUS[];
constexpr const char Dimm::MEMORY_TYPE[];
constexpr const char Dimm::DIMM_TYPE[];
constexpr const char Dimm::MODULE_TYPE[];
constexpr const char Dimm::DIMM_MEDIA[];
constexpr const char Dimm::MEMORY_MODES[];
constexpr const char Dimm::CAPACITY_MB[];
constexpr const char Dimm::DATA_WIDTH_BITS[];
constexpr const char Dimm::BUS_WIDTH_BITS[];
constexpr const char Dimm::FRU_INFO[];
constexpr const char Dimm::FIRMWARE_REVISION[];
constexpr const char Dimm::FIRMWARE_API_VERSION[];
constexpr const char Dimm::MEMORY_CLASSES[];
constexpr const char Dimm::VENDOR_ID[];
constexpr const char Dimm::DEVICE_ID[];
constexpr const char Dimm::OPERATING_SPEED_MHZ[];
constexpr const char Dimm::ALLOWED_SPEEDS_MHZ[];
constexpr const char Dimm::VOLTAGE[];
constexpr const char Dimm::MIN_VOLTAGE[];
constexpr const char Dimm::MAX_VOLTAGE[];
constexpr const char Dimm::DEVICE_LOCATOR[];
constexpr const char Dimm::DIMM_LOCATION[];
constexpr const char Dimm::RANK_COUNT[];
constexpr const char Dimm::ERROR_CORRECTION[];
constexpr const char Dimm::REGIONS[];
constexpr const char Dimm::OPERATING_MODES[];
constexpr const char Dimm::OEM[];


constexpr const char Drive::DRIVE[];
constexpr const char Drive::STATUS[];
constexpr const char Drive::PHYSICAL_ID[];
constexpr const char Drive::INTERFACE[];
constexpr const char Drive::TYPE[];
constexpr const char Drive::RPM[];
constexpr const char Drive::FIRMWARE_VERSION[];
constexpr const char Drive::CAPACITY[];
constexpr const char Drive::FRU_INFO[];
constexpr const char Drive::OEM[];

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

constexpr const char DimmLocation::SOCKET[];
constexpr const char DimmLocation::CONTROLLER[];
constexpr const char DimmLocation::CHANNEL[];
constexpr const char DimmLocation::SLOT[];

constexpr const char InterleaveSet::DIMM[];
constexpr const char InterleaveSet::REGION_ID[];

constexpr const char MemoryChunk::CHUNK[];
constexpr const char MemoryChunk::STATUS[];
constexpr const char MemoryChunk::CHUNK_NAME[];
constexpr const char MemoryChunk::CHUNK_ID[];
constexpr const char MemoryChunk::CHUNK_SIZE[];
constexpr const char MemoryChunk::TYPE[];
constexpr const char MemoryChunk::MIRRORED[];
constexpr const char MemoryChunk::SPARE[];
constexpr const char MemoryChunk::INTERLEAVE_SETS[];
constexpr const char MemoryChunk::OEM[];

}
}
}
