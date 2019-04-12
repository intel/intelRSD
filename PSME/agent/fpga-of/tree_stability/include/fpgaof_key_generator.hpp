/*!
 * @brief Definition of FpgaofKeyGenerator class.
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
 * @file fpgaof_key_generator.hpp
 */
#pragma once



#include "agent-framework/tree_stability/key_generator.hpp"
#include "agent-framework/tree_stability/key_value_missing_error.hpp"
#include "agent-framework/tree_stability/stabilizer.hpp"
#include "agent-framework/tree_stability/key_value_missing_error.hpp"



namespace agent {
namespace fpgaof {

/*!
 * @brief Key generating class for tree stabilization in FPGA-of agent.
 *
 * This class is responsible for generating unique keys for all resources in FPGA-oF agent,
 * regardless of the stabilization type (e.g. dry running or 'real' tree stabilization).
 * If it happens that the key generation method needs to be changed, this is the only
 * place to do so.
 * */
class FpgaofKeyGenerator : public agent_framework::KeyGenerator {
public:

    using KeysBaseMap = std::map<agent_framework::model::enums::Component, KeyType>;
    using agent_framework::KeyGenerator::generate_key;


    KeyType generate_key_base(const agent_framework::model::enums::Component& component) override;


    /*!
     * @brief Generate unique key for Processor.
     * @param[in] processor Processor resource.
     * @return Processor unique key.
     * */
    virtual KeyType generate_key(const agent_framework::model::Processor& processor);


private:
    static const KeysBaseMap m_keys_base_map;

};
}
}







