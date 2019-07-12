/*!
 * @brief Provides class for stabilizing fpga-of agent resources
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file fpgaof_key_generator.cpp
 * */



#include "agent-framework/module/managers/utils/manager_utils.hpp"
#include "agent-framework/module/model/model_fpgaof.hpp"
#include "agent-framework/module/model/fabric.hpp"
#include "agent-framework/module/model/system.hpp"
#include "agent-framework/module/model/chassis.hpp"
#include "fpgaof_key_generator.hpp"
#include "agent-framework/module/model/manager.hpp"
#include "agent-framework/tree_stability/stabilizer.hpp"



using namespace agent_framework::model;
using namespace agent_framework::module;

namespace agent {
namespace fpgaof {

const FpgaofKeyGenerator::KeysBaseMap FpgaofKeyGenerator::m_keys_base_map{
    {Manager::get_component(),          "_Fpgaof_Manager_"},
    {Chassis::get_component(),          "_Fpgaof_Chassis"},
    {Fabric::get_component(),           "_Fpgaof_Fabric"},
    {System::get_component(),           "_Fpgaof_System"},
    {NetworkInterface::get_component(), "_Fpgaof_Interface"},
    {Zone::get_component(),             "_Fpgaof_Zone"},
    {Endpoint::get_component(),         "_Fpgaof_Endpoint"},
    {Processor::get_component(),        "_Fpgaof_Processor"}
};


FpgaofKeyGenerator::KeyType FpgaofKeyGenerator::generate_key_base(const enums::Component& component) {
    return m_keys_base_map.at(component);
}


FpgaofKeyGenerator::KeyType FpgaofKeyGenerator::generate_key(const agent_framework::model::Processor& processor) {
    std::stringstream ss{};
    ss << generate_key_base(processor.get_component());
    ss << processor.get_fpga().get_bus();
    ss << processor.get_fpga().get_device();
    ss << processor.get_fpga().get_function();
    ss << processor.get_fpga().get_firmware_id();
    ss << processor.get_fpga().get_firmware_version();

    return ss.str();
}

}
}