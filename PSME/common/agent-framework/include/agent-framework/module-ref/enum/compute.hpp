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

#ifndef AGENT_FRAMEWORK_MODEL_ENUM_COMPUTE_HPP
#define AGENT_FRAMEWORK_MODEL_ENUM_COMPUTE_HPP

#include "enum_builder.hpp"
#include "processor_instruction_set.hpp"
#include "dimm_module_type.hpp"

namespace agent_framework {
namespace model {
namespace enums {

/*!
* @brief ENUM BootOverride for System class member
*
*/
ENUM(BootOverride, uint32_t, Disabled, Once, Continuous);

/*!
 * @brief ENUM BootOverrideTarget for System class member
 *
 */
ENUM(BootOverrideTarget, uint32_t, None, Pxe, Floppy, Cd, Usb, Hdd, BiosSetup,
        Utilities, Diags, UefiTarget);

/*!
 * @brief ENUM BootOverrideSupported for System class member
 *
 */
ENUM(BootOverrideSupported, uint32_t, Pxe, Floppy, Cd, Usb, Hdd, BiosSetup,
        Utilities, Diags, UefiTarget);


/*!
 * @brief ENUM PowerState for System class member
 *
 */
ENUM(PowerState, uint32_t, On, Off);

/*!
 * @brief ENUM ResetType. Action which can be triggered on System.
 *
 */
ENUM(ResetType, uint32_t, None, On, ForceOff, GracefulShutdown, GracefulRestart,
     ForceRestart, Nmi, ForceOn, PushPowerButton);

/*!
 * @brief ENUM ProcessorType for Processor class member
 *
 */
ENUM(ProcessorType, uint32_t, CPU, GPU, FPGA, DSP, ACL, OEM);

/*!
 * @brief ENUM ProcessorModel for Processor class member
 *
 */
ENUM(ProcessorModel, uint32_t, E3, E5, E7, X3, X5, X7, I3, I5, I7, Unknown);

/*!
 * @brief ENUM ProcessorArchitecture for Processor class member
 */
ENUM(ProcessorArchitecture, uint32_t, x86, x86_64);

/*!
 * @brief ENUM MemoryClass for Dimm class member
 */
ENUM(MemoryClass, uint32_t, Volatile, Block, Persistent, Unknown);

/*!
 * @brief ENUM ErrorCorrection for Dimm class member
 */
ENUM(ErrorCorrection, uint32_t, NoECC, SingleBitECC, MultiBitECC, AddressParity, Unknown);

/*!
 * @brief ENUM MemoryType for Dimm class member
 */
ENUM(MemoryType, uint32_t,
        DRAM,
        NVDIMM_N,
        NVDIMM_F,
        NVDIMM_P,
        Unknown,
    );

/*!
 * @brief ENUM DimmType for Dimm class member
 */
ENUM(DimmType, uint32_t,
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
 * @brief ENUM DimmMedia for Dimm class member
 */
ENUM(DimmMedia, uint32_t,
        DRAM,
        NAND,
        Proprietary,
        Unknown
    );

/*!
 * @brief ENUM MemoryMode for Dimm class member
 */
ENUM(MemoryMode, uint32_t,
        Volatile,
        PMEM,
        Block,
        Unknown
    );

/*!
 * @brief ENUM MemoryChunkType for for MemoryChunk class
 */
ENUM(MemoryChunkType, uint32_t, Volatile, Block, Persistent);

}
}
}

#endif //AGENT_FRAMEWORK_MODEL_ENUM_COMPUTE_HPP
