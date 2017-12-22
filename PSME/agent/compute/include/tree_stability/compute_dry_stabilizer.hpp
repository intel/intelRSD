/*!
 * @brief Provides class for dry run tree stabilization in compute agent.
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
 * @file compute_dry_stabilizer.hpp
 * */

#pragma once

#include "tree_stability/compute_key_generator.hpp"
#include "agent-framework/tree_stability/tree_stabilizer.hpp"

#include <string>
#include <utility>

namespace agent {
namespace compute {

/*!
 * @brief Dry run tree stability managing class for compute agent.
 * */
class ComputeDryStabilizer : public agent_framework::TreeStabilizer {
public:
    virtual ~ComputeDryStabilizer();


    /*!
     * Stabilize a resource object.
     *
     * @return Resource persistent UUID.
     */
    template<class T, class ... Args>
    const std::string stabilize(T& resource, const Args& ... args) const {
        std::string resource_temporary_uuid = resource.get_uuid();

        try {
            const std::string& resource_unique_key{ComputeKeyGenerator::generate_key(resource, args...)};
            resource.set_unique_key(resource_unique_key);
            resource.make_persistent_uuid();
            const std::string& resource_persistent_uuid = resource.get_uuid();

            log_persistent_uuid_generated(T::get_component().to_string(), resource_temporary_uuid,
                                          resource_persistent_uuid);

            return resource_persistent_uuid;
        }
        catch (const agent_framework::KeyValueMissingError&) {
            log_key_value_missing(T::get_component().to_string(), resource_temporary_uuid);
            return resource_temporary_uuid;
        }
    }


    /*!
     * Placeholder function, needed so that the ComputeDryStabilizer class
     * is not abstract.
     *
     * @return Module UUID.
     * */
    virtual const std::string stabilize(const std::string& module_uuid) {
        return module_uuid;
    }
};

}
}
