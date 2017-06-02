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
 *    http_//www.apache.org/licenses/LICENSE-2.0
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
 * @brief string enums for model classes, see enum_builder.hpp for more info
 * */

#pragma once



#include "enum_builder.hpp"
#include "processor_instruction_set.hpp"
#include "memory_module_type.hpp"



namespace agent_framework {
namespace model {
namespace enums {

/*!
 * @brief ENUM SystemType for System class member
 *
 */
ENUM(SystemType, uint32_t, Physical, Virtual, OS, PhysicallyPartitioned, VirtuallyPartitioned);

/*!
 * @brief ENUM BootOverride for System class member
 *
 */
ENUM(BootOverride, uint32_t, Disabled, Once, Continuous);

/*!
 * @brief ENUM BootOverrideMode for System class member
 *
 */
ENUM(BootOverrideMode, uint32_t, Legacy, UEFI);

/*!
 * @brief ENUM BootOverrideTarget for System class member
 *
 */
ENUM(BootOverrideTarget, uint32_t, None, Pxe, Floppy, Cd, Usb, Hdd, BiosSetup,
     Utilities, Diags, UefiTarget, SDCard, UefiHttp, RemoteDrive);

/*!
 * @brief ENUM PowerState for System class member
 *
 */
ENUM(PowerState, uint32_t, On, Off);

/*!
 * @brief ENUM ProcessorType for Processor class member
 *
 */
ENUM(ProcessorType, uint32_t, CPU, GPU, FPGA, DSP, Accelerator, OEM);

/*!
 * @brief ENUM ProcessorModel for Processor class member
 *
 */
ENUM(ProcessorModel, uint32_t, E3, E5, E7, X3, X5, X7, I3, I5, I7, Unknown);

/*!
 * @brief ENUM ProcessorArchitecture for Processor class member
 */
ENUM(ProcessorArchitecture, uint32_t, x86, ARM, MIPS);

/*!
 * @brief ENUM MemoryClass for Memory class member
 */
ENUM(MemoryClass, uint32_t, Volatile, Block, Persistent, Unknown);

/*!
 * @brief ENUM ErrorCorrection for Memory class member
 */
ENUM(ErrorCorrection, uint32_t, NoECC, SingleBitECC, MultiBitECC, AddressParity, Unknown);

/*!
 * @brief ENUM MemoryType for Memory class member
 */
ENUM(MemoryType, uint32_t,
     DRAM,
     NVDIMM_N,
     NVDIMM_F,
     NVDIMM_P,
     Unknown,
);

/*!
 * @brief ENUM DeviceType for Memory class member
 */
ENUM(DeviceType, uint32_t,
     DDR,
     DDR2,
     DDR3,
     DDR4,
     DDR4_SDRAM,
     DDR4E_SDRAM,
     LPDDR4_SDRAM,
     DDR3_SDRAM,
     LPDDR3_SDRAM,
     DDR2_SDRAM,
     DDR2_SDRAM_FB_DIMM,
     DDR2_SDRAM_FB_DIMM_PROBE,
     DDR_SGRAM,
     DDR_SDRAM,
     ROM,
     SDRAM,
     EDO,
     Fast_Page_Mode,
     PipelinedNibble,
     Unknown
);

/*!
 * @brief ENUM Media for Memory class member
 */
ENUM(Media, uint32_t,
     DRAM,
     NAND,
     Proprietary,
     Unknown
);

/*!
 * @brief ENUM MemoryMode for Memory class member
 */
ENUM(MemoryMode, uint32_t,
     Volatile,
     PMEM,
     Block,
     Unknown
);

/*!
 * @brief ENUM IPAddressType for NetworkDeviceFunction class member
 */
ENUM(IPAddressType, uint32_t, IPv4, IPv6);


/*!
 * @brief ENUM FunctionAuthenticationMethod for NetworkDeviceFunction class member
 */
ENUM(FunctionAuthenticationMethod, uint32_t, None, CHAP, MutualCHAP);

}
}
}

