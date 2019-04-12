/*!
 * @brief Provides class for stabilizing chassis agent resource tree
 *
 * @copyright
 * Copyright (c) 2015-2019 Intel Corporation
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
 * @file chassis_tree_stabilizer.hpp
 * */



#pragma once



#include "agent-framework/tree_stability/tree_stabilizer.hpp"

#include <string>



namespace agent {
namespace chassis {

/*!
 * @brief Tree stability managing class for chassis agent.
 * */
class ChassisTreeStabilizer : public agent_framework::TreeStabilizer {
public:
    /*! Destructor */
    virtual ~ChassisTreeStabilizer();


    /*!
     * Stabilizes compute agent resource tree starting from the module.
     *
     * @param[in] module_uuid Temporary UUID of module to stabilize
     *
     * @return Persistent module UUID
     * */
    virtual const std::string stabilize(const std::string& module_uuid) override;

};

}
}
