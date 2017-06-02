/*!
 * @brief Provides class for dry run tree stabilization in pnc agent
 *
 * @copyright
 * Copyright (c) 2016-2017 Intel Corporation
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
 * @file pnc_dry_stabilizer.hpp
 * */

#pragma once



#include "tree_stability/pnc_key_generator.hpp"
#include "agent-framework/tree_stability/tree_stabilizer.hpp"
#include "agent-framework/module/model/model_pnc.hpp"

#include <string>
#include <utility>



namespace agent {
namespace pnc {

/*!
 * @brief Dry run tree stability managing class for pnc agent.
 * */
class PncDryStabilizer : public agent_framework::TreeStabilizer {
public:
    virtual ~PncDryStabilizer();


    /*!
     * Stabilize a resource object.
     *
     * @return Resource persistent UUID.
     */
    template<class T, class ... Args>
    const std::string stabilize(T& resource, const Args& ... args) const {
        const std::string& resource_unique_key{PncKeyGenerator::generate_key(resource, args...)};
        return generate_persistent_uuid(resource, resource_unique_key);
    }


    /*!
     * Temporary placeholder function, needed so that the PncDryStabilizer class
     * is not abstract. Will be removed in a couple of days during tree stability refactorization.
     *
     * @return Module UUID.
     * */
    virtual const std::string stabilize(const std::string& module_uuid) {
        return module_uuid;
    }


protected:
    // @TODO: clean up this mess
    const std::string generate_persistent_uuid(agent_framework::model::Resource& resource,
                                               const std::string& unique_key) const {
        const std::string resource_uuid = resource.get_uuid();
        resource.set_unique_key(unique_key);
        resource.make_persistent_uuid();
        const std::string resource_persistent_uuid = resource.get_uuid();
        resource.make_random_uuid();
        resource.set_uuid(resource_uuid);
        return resource_persistent_uuid;
    }

};

}
}
