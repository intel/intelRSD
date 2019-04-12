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
 * @file utils.cpp
 * @brief PMC GAS utils
 * */

#include "gas/utils.hpp"



namespace {
using namespace agent::pnc::gas;

const std::map<PortPartitionP2PBindingReturnValue, std::string> p2p_binding_result{
    {PortPartitionP2PBindingReturnValue::COMMAND_SUCCEED,                    "Command succeeded"},
    {PortPartitionP2PBindingReturnValue::PHY_PORT_ALREADY_BOUND,             "Physical port already bound"},
    {PortPartitionP2PBindingReturnValue::LOGICAL_BRIDGE_BINDING_INSTANCE_ALREADY_BOUND,
                                                                             "Logical bridge binding instance already bound"},
    {PortPartitionP2PBindingReturnValue::PARTITION_DOES_NOT_EXIST,           "Partition does not exist"},
    {PortPartitionP2PBindingReturnValue::PHY_PORT_DOES_NOT_EXIST,            "Physical port does not exist"},
    {PortPartitionP2PBindingReturnValue::PHY_PORT_DISABLED,                  "Physical port disabled"},
    {PortPartitionP2PBindingReturnValue::NO_LOGICAL_BRIDGE_BINDING_INSTANCE, "No logical bridge binding instance"},
    {PortPartitionP2PBindingReturnValue::BIND_UNBIND_IN_PROGRESS,            "Bind/unbind in progress"},
    {PortPartitionP2PBindingReturnValue::BIND_UNBIND_TARGET_IS_USP,          "Bind/unbind target is USP"},
    {PortPartitionP2PBindingReturnValue::SUB_COMMAND_DOES_NOT_EXIST,         "Sub command does not exist"},
    {PortPartitionP2PBindingReturnValue::PHY_PORT_LINK_ACTIVE,               "Physical port link active"},
    {PortPartitionP2PBindingReturnValue::LOGICAL_BRIDGE_BINDING_INSTANCE_NOT_BOUND_TO_PHY_PORT,
                                                                             "Logical bridge binding instance not bound to physical port"},
    {PortPartitionP2PBindingReturnValue::INVALID_UNBIND_OPTION,              "Invalid unbind option"}
};

const std::map<UnbindPortOption, std::string> unbind_option{
    {UnbindPortOption::UNBIND_IF_LINK_IS_DOWN,                          "Link down"},
    {UnbindPortOption::IF_PHY_PORT_IS_L0_OR_L1_SIM_MANAGED_HOT_REMOVE,  "Managed hot remove"},
    {UnbindPortOption::IF_PHY_PORT_IS_L0_OR_L1_SIM_SURPRISE_HOT_REMOVE, "Surprise hot remove"},
    {UnbindPortOption::IF_PHY_PORT_IS_L0_OR_L1_SIM_LINK_DOWN,           "Simulated link down"}
};

const std::map<OperationResult, std::string> operation_result{
    {OperationResult::SUCCESS,     "Success"},
    {OperationResult::FAILED,      "Failed"},
    {OperationResult::IN_PROGRESS, "In progress"},
    {OperationResult::N_A,         "N/A"}
};

const std::map<BindingState, std::string> bind_state{
    {BindingState::UNBOUND, "Unbound"},
    {BindingState::BOUND,   "Bound"},
    {BindingState::N_A,     "N/A"}
};

const std::map<CommandCode, std::string> command{
    {CommandCode::NONE,                       "None"},
    {CommandCode::PORT_PARTITION_P2P_BINDING, "Port/partition P2P binding"},
    {CommandCode::TWI_ACCESS,                 "TWI access"},
    {CommandCode::LTSSM_MONITOR,              "LTSSM monitor"},
    {CommandCode::LINK_STATUS_RETRIEVE,       "Link status retrieve"}
};

const std::map<LtssmMonitorReturnValue, std::string> ltssm_result{
    {LtssmMonitorReturnValue::COMMAND_SUCCEED,                    "Command succeeded"},
    {LtssmMonitorReturnValue::PHY_PORT_INACTIVE,                  "Physical port inactive"},
    {LtssmMonitorReturnValue::TRIGGER_CONDITION_NUMBER_INVALID,   "Trigger condition number invalid"},
    {LtssmMonitorReturnValue::TRIGGER_MODE_INVALID,               "Trigger mode invalid"},
    {LtssmMonitorReturnValue::SRC_STATE_FOR_TRIGGER_MODE_INVALID, "Src state for trigger mode invalid"},
    {LtssmMonitorReturnValue::DST_STATE_FOR_TRIGGER_MODE_INVALID, "Dst state for trigger mode invalid"},
    {LtssmMonitorReturnValue::LTMON_ACCESS_REFUSED,               "LTMON access refused"}
};
}

/*! Agent namespace */
namespace agent {
/*! PNC namespace */
namespace pnc {
/*! GAS namespace */
namespace gas {

const std::string get_p2p_binding_command_result(std::uint32_t value) {
    return get_p2p_binding_command_result(static_cast<PortPartitionP2PBindingReturnValue>(value));
}

const std::string get_p2p_binding_command_result(PortPartitionP2PBindingReturnValue value) {
    try {
        return p2p_binding_result.at(value);
    }
    catch (std::out_of_range&) {
        return std::string("Unknown value: " + std::to_string(static_cast<int>(value)));
    }
}



const std::string get_unbind_option(std::uint32_t value) {
    try {
        return unbind_option.at(static_cast<UnbindPortOption>(value));
    }
    catch (std::out_of_range&) {
        return std::string("Unknown value: " + std::to_string(static_cast<int>(value)));
    }
}


const std::string get_command_name(std::uint32_t value) {
    try {
        return command.at(static_cast<CommandCode>(value));
    }
    catch (std::out_of_range&) {
        return std::string("Unknown value: " + std::to_string(static_cast<int>(value)));
    }
}


const std::string get_operation_result(uint8_t operation_state) {
    try {
        return operation_result.at(static_cast<OperationResult>(BINDING_INFO_OPERATION_RESULT_MASK & operation_state));
    }
    catch (std::out_of_range&) {
        return std::string("Unknown value: " + std::to_string(static_cast<int>(operation_state)));
    }
}


const std::string get_binding_state(uint8_t operation_state) {
    try {
        return bind_state.at(static_cast<BindingState>((BINDING_INFO_BINDING_STATE_MASK & operation_state) >> 4));
    }
    catch (std::out_of_range&) {
        return std::string("Unknown value: " + std::to_string(static_cast<int>(operation_state)));
    }
}


OperationResult get_operation_result_enum(uint8_t operation_state) {
    return static_cast<OperationResult>(BINDING_INFO_OPERATION_RESULT_MASK & operation_state);
}


BindingState get_binding_state_enum(uint8_t operation_state) {
    return static_cast<BindingState>((BINDING_INFO_BINDING_STATE_MASK & operation_state) >> 4);
}


const std::string get_ltssm_monitor_return_value(uint32_t value) {
    try {
        return ltssm_result.at(static_cast<LtssmMonitorReturnValue>(value));
    }
    catch (std::out_of_range&) {
        return std::string("Unknown value: " + std::to_string(static_cast<int>(value)));
    }
}

}
}
}
