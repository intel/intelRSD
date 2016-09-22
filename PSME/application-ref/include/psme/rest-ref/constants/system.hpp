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

#pragma once
namespace app {
namespace rest {
namespace constants {

namespace System {
    extern const char SYSTEM_TYPE[];
    extern const char PHYSICAL[];
    extern const char SKU[];
    extern const char HOST_NAME[];
    extern const char INDICATOR_LED[];
    extern const char POWER_STATE[];
    extern const char BOOT[];
    extern const char BOOT_MODE[];
    extern const char BOOT_SOURCE_OVERRIDE_ENABLED[];
    extern const char BOOT_SOURCE_OVERRIDE_TARGET[];
    extern const char BOOT_SOURCE_OVERRIDE_TARGET_ALLOWABLE_VALUES[];
    extern const char PXE_PARAMETERS[];
    extern const char ETHERNET_INTERFACE[];
    extern const char ETHERNET_INTERFACES[];
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
    extern const char DIMM_CONFIG[];
    extern const char MEMORY_CHUNKS[];
    extern const char CHASSIS[];
    extern const char ACTIONS[];
    extern const char COMPUTER_SYSTEM_RESET[];
    extern const char TARGET[];
    extern const char ALLOWABLE_RESET_TYPES[];
    extern const char COMPUTER_SYSTEM_START_DEEP_DISCOVERY[];
    extern const char DISCOVERY_STATE[];
    extern const char BASIC[];
    extern const char MEMORY_SOCKETS[];
    extern const char PCI_DEVICES[];
    extern const char PROCESSOR_SOCKETS[];
    extern const char STORAGE_ADAPTERS[];
    extern const char RESET_TYPE[];
    extern const char ON[];
    extern const char FORCE_OFF[];
    extern const char GRACEFUL_SHUTDOWN[];
    extern const char FORCE_RESTART[];
    extern const char NMI[];
    extern const char GRACEFUL_RESTART[];
    extern const char FORCE_ON[];
    extern const char PUSH_POWER_BUTTON[];
    extern const char VENDOR_ID[];
    extern const char DEVICE_ID[];
}

namespace DimmConfig {
    extern const char DIMM_TYPE[];
    extern const char DIMM_DEVICE_TYPE[];
    extern const char BASE_MODULE_TYPE[];
    extern const char DIMM_MEDIA[];
    extern const char CAPACITY_MIB[];
    extern const char DATA_WIDTH_BITS[];
    extern const char BUS_WIDTH_BITS[];
    extern const char ALLOWED_SPEEDS_MHZ[];
    extern const char FIRMWARE_REVISION[];
    extern const char FIRMWARE_API_VERSION[];
    extern const char FUNCTION_CLASSES[];
    extern const char SUBSYSTEM_VENDOR_ID[];
    extern const char SUBSYSTEM_DEVICE_ID[];
    extern const char RANK_COUNT[];
    extern const char DEVICE_LOCATOR[];
    extern const char DIMM_LOCATION[];
    extern const char SOCKET[];
    extern const char MEMORY_CONTROLLER[];
    extern const char CHANNEL[];
    extern const char SLOT[];
    extern const char ERROR_CORRECTION[];
    extern const char OPERATING_SPEED_MHZ[];
    extern const char REGIONS[];
    extern const char REGION_ID[];
    extern const char MEMORY_TYPE[];
    extern const char OFFSET_MIB[];
    extern const char SIZE_MIB[];
    extern const char OPERATING_MEMORY_MODES[];
    extern const char METRICS[];
    extern const char VOLTAGE_VOLT[];
    extern const char VENDOR_ID[];
    extern const char DEVICE_ID[];
}

namespace Processor {
    extern const char SOCKET[];
    extern const char PROC_TYPE[];
    extern const char BRAND[];
    extern const char CAPABILITIES[];
    extern const char PROC_ARCHITECTURE[];
    extern const char INSTRUCTION_SET[];
    extern const char PROC_ID[];
    extern const char IDENTIFICATION_REGISTERS[];
    extern const char EFFECTIVE_FAMILY[];
    extern const char EFFECTIVE_MODEL[];
    extern const char STEP[];
    extern const char MICROCODE_INFO[];
    extern const char MAX_SPEED[];
    extern const char TOTAL_CORES[];
    extern const char TOTAL_THREADS[];
}

namespace MemoryChunk {
    extern const char MEMORY_CHUNK_NAME[];
    extern const char MEMORY_CHUNK_UID[];
    extern const char MEMORY_CHUNK_SIZE_MIB[];
    extern const char ADDRESS_RANGE_TYPE[];
    extern const char IS_MIRROR_ENABLED[];
    extern const char IS_SPARE[];
    extern const char INTERLEAVE_SETS[];
    extern const char REGION_ID[];
}

namespace StorageAdapter {
    extern const char INTERFACE[];
    extern const char BUS_INFO[];
    extern const char DEVICES[];
}

namespace Drive {
    extern const char INTERFACE[];
    extern const char CAPACITY[];
    extern const char TYPE[];
    extern const char RPM[];
    extern const char BUS_INFO[];
    extern const char USED_BY[];
}

}
}
}
