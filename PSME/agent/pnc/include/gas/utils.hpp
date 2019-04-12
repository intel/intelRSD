/*!
 * @copyright
 * Copyright (c) 2016-2019 Intel Corporation
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
 * @file utils.hpp
 * @brief PMC GAS utils
 * */

#pragma once



#include "agent-framework/module/enum/enum_builder.hpp"

/*! Agent namespace */
namespace agent {
/*! PNC namespace */
namespace pnc {
/*! GAS namespace */
namespace gas {

/*! Number of physical ports in PM85X6 switch */
static constexpr std::uint8_t PM85X6_PHY_PORTS_NUMBER = 48;
/*! Max Logical Bridge Count per Partition in PM85X6 switch */
static constexpr std::uint8_t PM85X6_MAX_LOGICAL_BRIDGE_COUNT_PER_PARTITION = 48;
/*! Max PCIe functions in PM85X6 switch */
static constexpr std::uint16_t PM85X6_MAX_PCIE_FUNCTIONS_NUMBER = 308;
/*! Max partitions in PM85X6 switch */
static constexpr std::uint16_t PM85X6_MAX_PARTITIONS_NUMBER = 48;
/*! Seeprom data size in bytes */
static constexpr std::uint8_t PM85X6_SEEPROM_DATA_SIZE_BYTES = 79;
/*! Host cable eeprom size */
static constexpr std::uint16_t HOST_CABLE_EEPROM_DATA_SIZE_BYTES = 256;
/*! I2C MF3 slave address */
static constexpr std::uint8_t I2C_MF3_BMC_SLAVE_ADDRESS = 0x10;
/*! PM85X6 TWI Ports */
enum PM85X6TwiPort : std::uint8_t {
    PORT0 = 0,
    PORT1 = 1,
    PORT2 = 2,
    PORT3 = 3,
    PORT4 = 4,
    PORT8 = 8,
    PORT9 = 9
};

/*! PCA9548 Expander channels */
enum PCA9548TwiExpanderChannel : std::uint8_t {
    CHANNEL0 = 1,
    CHANNEL1 = 2,
    CHANNEL2 = 4,
    CHANNEL3 = 8,
    CHANNEL4 = 16,
    CHANNEL5 = 32,
    CHANNEL6 = 64,
    CHANNEL7 = 128
};

/*! PM85X6 TWI Devices Addresses */
enum PM85X6TwiDeviceAddress : std::uint16_t {
    SEEPROM = 0x00a0,
    EDK_TWI_EXPANDER = 0x00ec,
    MF3_TWI_EXPANDER = 0x00e0,
    HOST_CABLE = 0x00a0,
    NVME_VPD = 0x00a6,
    SMART = 0x00d4,
    DRIVE_PRESENCE = 0x0040,
    MF3_FRU_EEPROM = 0x00a2,
    FPGA_RUSHCREEK_SLAVE_ADDRESS = 0x00ce
};

/*! PM85X6 TWI register with NVMe drives presence*/
enum NVMeDrivePresence : std::uint8_t {
    PORTS_0_14 = 0x2,
    PORTS_32_46= 0x1,
};

/* MEMORY MAP */
/*! MRPC input data region offset */
static constexpr std::uint32_t MRPC_INPUT_DATA_REG_OFFSET = 0x00000000;
/*! MRPC output data region offset */
static constexpr std::uint32_t MRPC_OUTPUT_DATA_REG_OFFSET = 0x00000400;
/*! MRPC command region offset */
static constexpr std::uint32_t MRPC_COMMAND_REG_OFFSET = 0x00000800;
/*! MRPC status region offset */
static constexpr std::uint32_t MRPC_STATUS_REG_OFFSET = 0x00000804;
/*! MRPC command return value region offset */
static constexpr std::uint32_t MRPC_COMMAND_RETURN_VALUE_REG_OFFSET = 0x00000808;
/*! Top Level Registers offset */
static constexpr std::uint32_t TOP_SETTING_REG_OFFSET = 0x00001000;
/*! Switch Event Region offset */
static constexpr std::uint32_t SWITCH_EVENT_REG_OFFSET = 0x00001800;
/*! Reserved Region offset */
static constexpr std::uint32_t RSERVED_REG_OFFSET = 0x00002000;
/*! Partition Region offset */
static constexpr std::uint32_t PARTITION_REG_OFFSET = 0x00004000;
/*! NTB Mem Mapped Region offset */
static constexpr std::uint32_t NTB_MEM_MAPPED_REG_OFFSET = 0x00010000;
/*! PCIe Configuration Space Registers */
static constexpr std::uint32_t CSR_REG_OFFSET = 0x00134000;

/*! MRPC region size */
static constexpr std::uint32_t MRPC_REG_SIZE = TOP_SETTING_REG_OFFSET - MRPC_INPUT_DATA_REG_OFFSET;
/*! MRPC command region size */
static constexpr std::uint32_t MRPC_COMMAND_REG_SIZE = MRPC_STATUS_REG_OFFSET - MRPC_COMMAND_REG_OFFSET;
/*! MRPC status region size */
static constexpr std::uint32_t MRPC_STATUS_REG_SIZE = MRPC_COMMAND_RETURN_VALUE_REG_OFFSET - MRPC_STATUS_REG_OFFSET;
/*! MRPC command return value region size */
static constexpr std::uint32_t MRPC_COMMAND_RETURN_VALUE_REG_SIZE = 4;
/*! Top Level Registers size */
static constexpr std::uint32_t TOP_SETTING_REG_SIZE = SWITCH_EVENT_REG_OFFSET - TOP_SETTING_REG_OFFSET;
/*! Partition region size */
static constexpr std::uint32_t PARTITION_REG_SIZE = NTB_MEM_MAPPED_REG_OFFSET - PARTITION_REG_OFFSET;
/*! Partition configuration max number of ports */
static constexpr std::uint32_t PARTITION_MAX_DSP_PORTS_NUM = 47;
/*! Partition used region size */
static constexpr std::uint32_t PARTITION_USED_REG_SIZE = 24 + PARTITION_MAX_DSP_PORTS_NUM * 4;
/*! Single PCIe Configuration space size */
static constexpr std::uint32_t SINGLE_PCIE_FUN_CSR_REG_SIZE = 4 * 1024;

/*! MRPC Command code */
enum class CommandCode : std::uint32_t {
    NONE = 0x00000000,
    TWI_ACCESS = 0x00000001,
    PORT_PARTITION_P2P_BINDING = 0x0000000C,
    LTSSM_MONITOR = 0x00000011,
    LINK_STATUS_RETRIEVE = 0x0000001C
};

/*! MRPC Command status */
enum class CommandStatus : std::uint8_t {
    IDLE = 0,
    IN_PROGRESS = 1,
    DONE = 2,
    FAILED = 0xff
};

/*
 * TWI Access Command
 * */

/*! TWI Slave Address size */
enum class TwiSlaveAddressSize : std::uint8_t {
    ADDRESS_7_BIT = 0,
    ADDRESS_10_BIT = 1
};

/*! TWI Offset size */
enum class TwiOffsetSize : std::uint8_t {
    OFFSET_8_BIT = 1,
    OFFSET_16_BIT = 2,
    OFFSET_32_BIT = 4
};

/*! TWI Access SubCommand code */
enum class TwiAccessSubCommandCode : std::uint8_t {
    READ = 0,
    WRITE = 1
};

/*! TWI Access Return Value */
enum class TwiAccessReturnValue : std::uint32_t {
    SUCCESS = 0
};

/*
* Port Partition P2P Binding Command
* */

/*! Port Partition P2P Bind SubCommand code */
enum class PortPartitionP2PBindSubCommandCode : std::uint8_t {
    BIND_PORT = 0,
    UNBIND_PORT = 1,
    PORT_BINDING_INFO = 2,
    PARTITION_BINDING_INFO = 3
};

/*! Unbind Port Option */
enum class UnbindPortOption {
    UNBIND_IF_LINK_IS_DOWN = 0,
    IF_PHY_PORT_IS_L0_OR_L1_SIM_MANAGED_HOT_REMOVE = 1,
    IF_PHY_PORT_IS_L0_OR_L1_SIM_SURPRISE_HOT_REMOVE = 2,
    IF_PHY_PORT_IS_L0_OR_L1_SIM_LINK_DOWN = 3
};

/*! PortPartitionP2PBinding Return Value */
enum class PortPartitionP2PBindingReturnValue : std::uint32_t {
    COMMAND_SUCCEED = 0x00000000,
    PHY_PORT_ALREADY_BOUND = 0x00000001,
    LOGICAL_BRIDGE_BINDING_INSTANCE_ALREADY_BOUND = 0x00000002,
    PARTITION_DOES_NOT_EXIST = 0x00000003,
    PHY_PORT_DOES_NOT_EXIST = 0x00000004,
    PHY_PORT_DISABLED = 0x00000005,
    NO_LOGICAL_BRIDGE_BINDING_INSTANCE = 0x00000006,
    BIND_UNBIND_IN_PROGRESS = 0x00000007,
    BIND_UNBIND_TARGET_IS_USP = 0x00000008,
    SUB_COMMAND_DOES_NOT_EXIST = 0x00000009,
    PHY_PORT_LINK_ACTIVE = 0x0000000A,
    LOGICAL_BRIDGE_BINDING_INSTANCE_NOT_BOUND_TO_PHY_PORT = 0x0000000B,
    INVALID_UNBIND_OPTION = 0x0000000C
};

/*
 * LTSSM Monitor Command
 * */

/*! LTSSMMonitor SubCommand code */
enum class LtssmMonitorSubCommandCode : std::uint8_t {
    RESERVED = 0,
    LTMON_BASIC_CONFIGURATION = 1,
    LTMON_RESET = 2,
    LTMON_TRIGGER_CONDITION_CONFIGURATION = 3,
    LTMON_MODE_CONFIGURATION = 4,
    LTMON_STATUS_GET = 5,
    LTMON_FREEZE_UNFREEZE = 6,
    LTMON_LOG_DUMP = 7,
    LTMON_ACCESS_PERMITION_FREE = 8
};

/*! LTSSMMonitor Return Value */
enum class LtssmMonitorReturnValue : std::uint32_t {
    COMMAND_SUCCEED = 0x00000000,
    PHY_PORT_INACTIVE = 0x00110001,
    TRIGGER_CONDITION_NUMBER_INVALID = 0x00110002,
    TRIGGER_MODE_INVALID = 0x00110003,
    SRC_STATE_FOR_TRIGGER_MODE_INVALID = 0x00110004,
    DST_STATE_FOR_TRIGGER_MODE_INVALID = 0x00110005,
    LTMON_ACCESS_REFUSED = 0xffff0001
};

/*! LTSSMMonitor Log Wrapping */
enum class LtssmMonitorWrap : uint8_t {
    NO_WRAP = 0x00,
    WRAP = 0x01
};

/*! LTSSMMonitor port major state ignore mask */
enum class LtssmMonitorMajorStateIgnore : std::uint8_t {
    NOT_IGNORE = 0x00,
    IGNORE = 0x01
};

/*! LTSSMMonitor port minor state ignore mask */
enum class LtssmMonitorMinorStateIgnore : std::uint16_t {
    IGNORE_ALL = 0xffff,
    NOT_IGNORE = 0x0000
};

/*! LTSSMMonitor mode */
enum class LtssmMonitorMode : std::uint8_t {
    CAPTURE = 0,
    TRIGGER = 1
};

/*! LTSSMMonitor rollover */
enum class LtssmMonitorRollover : std::uint8_t {
    NO = 0,
    YES = 1
};

/*! Freeze/Unfreeze action */
enum class LtssmMonitorFreezeAction : std::uint8_t {
    UNFREEZE = 0,
    FREEZE = 1
};

/*! PCIe Link State */
enum class PCIeLinkState : std::uint8_t {
    DETECT = 0,
    POLL = 1,
    CFG = 2,
    L0 = 3,
    RECOV = 4,
    DISABLE = 5,
    LOOPBK = 6,
    HOTRST = 7,
    TXLOS = 8,
    L1 = 9,
    L2 = 10
};

/*! PM85X6 Max Link Speed GT/s */
double constexpr PM85X6_MAX_LINK_SPEED = 8.0;

/*! PCIe Generation */
enum class PCIeGen : std::uint8_t {
    GEN1 = 0x01,
    GEN2 = 0x02,
    GEN3 = 0x03
};

/*! PCIe Generation to transfer map */
static std::map<PCIeGen, double> pcie_gen_to_transfer = {
        { PCIeGen::GEN1, 2.5 },
        { PCIeGen::GEN2, 5.0 },
        { PCIeGen::GEN3, 8.0 }
};

/*! LTSSM PCIe Port Major State mask */
static constexpr uint32_t LTMON_LOG_MAJOR_STATE_MASK = 0x000000780;
/*! LTSSM PCIe Port Major State shift */
static constexpr uint32_t LTMON_LOG_MAJOR_STATE_SHIFT = 7;

/*
 * Link Status Retrieve Command
 * */

/*! Link Status Retrieve Input */
enum class LinkStatusRetrieveInput : std::uint64_t {
    ALL_PORTS = 0xffffffffffffffff,
    ALL_BOUND_PORTS = 0
};

/*! PortPartitionP2PBinding Return Value */
enum class LinkStatusRetrieveReturnValue : std::uint32_t {
    COMMAND_SUCCEED = 0x00000000
};

/*! Partition status */
enum class PartitionStatus : std::uint32_t {
    INACTIVE = 0x00000000,
    ACTIVE = 0x00000001
};

/*! Partition state */
enum class PartitionState : std::uint32_t {
    UNINITIALIZED = 0x00000000,
    ACTIVE = 0x00000001,
    FUNDAMENTAL_RESET_PSC = 0x00000002,
    FUNDAMENTAL_RESET_PTD = 0x00000003,
    FUNDAMENTAL_RESET_CLEAN_TLP_DONE = 0x00000004,
    HOT_RESET_PSC = 0x00000005,
    HOT_RESET_PTD = 0x00000006,
    HOT_RESET_CLEANUP_TLP_DONE = 0x00000007,
    USP_SECOND_BUS_RESET = 0x00000008
};

/*! USP Port Mode */
enum class UspPortMode : std::uint32_t {
    ONE_P2P_BRIDGE = 0x00000000,
    ONE_P2P_BRIDGE_AND_ONE_NT_EP = 0x00000001,
    ONE_P2P_BRIDGE_AND_ONE_MGMT_EP = 0x00000002,
    ONE_NT_EP = 0x00000003,
    VRC = 0x00000004
};

/*! PortBindingInfo and PartitionBindingInfo operation result mask */
static constexpr uint8_t BINDING_INFO_OPERATION_RESULT_MASK = 0x0f;

/*! PortBindingInfo and PartitionBindingInfo state mask */
static constexpr uint8_t BINDING_INFO_BINDING_STATE_MASK = 0xf0;

/*! Port and Partition Binding Info operation result */
enum class OperationResult : std::uint8_t {
    SUCCESS = 0x00,
    FAILED = 0x01,
    IN_PROGRESS = 0x02,
    N_A = 0x0f
};

/*! Port Binding state */
enum class BindingState : std::uint8_t {
    UNBOUND = 0x00,
    BOUND = 0x01,
    N_A = 0x0f
};

/*! Physical id value when no port assigned */
static constexpr uint8_t NO_PORT_ASSIGNED = 0xff;


/*!
 * @brief Method used to obtain the human readable result value from PortPartitionP2PBinding command.
 * @param value Value of the PortPartitionP2PBinding command return code.
 */
const std::string get_p2p_binding_command_result(std::uint32_t value);


/*!
 * @brief Method used to obtain the human readable result value from PortPartitionP2PBinding command.
 * @param value Value of the PortPartitionP2PBinding command enum return code.
 */
const std::string get_p2p_binding_command_result(PortPartitionP2PBindingReturnValue value);


/*!
 * @brief Method used to obtain the human readable unbind option value.
 * @param value Unbind option used for the unbind command.
 */
const std::string get_unbind_option(std::uint32_t value);


/*!
 * @brief Method used to obtain the numerical operation result from PortBindinginfo and PartitionBindingInfo command.
 * @param operation_state Operation state returned from GAS.
 */
OperationResult get_operation_result_enum(uint8_t operation_state);


/*!
 * @brief Method used to obtain the numerical binding state enum from PortBindinginfo and PartitionBindingInfo command.
 * @param operation_state Operation state returned from GAS.
 */
BindingState get_binding_state_enum(uint8_t operation_state);


/*!
 * @brief Method used to obtain the human readable result from PortBindinginfo and PartitionBindingInfo command.
 * @param operation_state Operation state returned from GAS.
 */
const std::string get_operation_result(uint8_t operation_state);


/*!
 * @brief Method used to obtain the human readable binding state from PortBindinginfo and PartitionBindingInfo command.
 * @param operation_state Operation state returned from gas.
 */
const std::string get_binding_state(uint8_t operation_state);


/*!
 * @brief Method used to obtain the human readable name of the mrpc command to be executed.
 * @param value Numeric value of the command name enum.
 */
const std::string get_command_name(uint32_t value);


/*!
 * @brief Method used to obtain human readable ltssm monitor command return value
 * @param value Return value obtained from GAS.
 */
const std::string get_ltssm_monitor_return_value(uint32_t value);
}
}
}
