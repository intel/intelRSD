/*!
 * @brief Definition of NvmeKeyGenerator class.
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation
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
 * @file nvme_key_generator.hpp
 */

#pragma once

#include "agent-framework/tree_stability/key_value_missing_error.hpp"
#include "agent-framework/module/model/model_common.hpp"
#include "agent-framework/module/model/model_storage.hpp"

#include <map>
#include <string>

namespace agent {
namespace nvme {

class NvmeKeyGenerator {
public:
    /*!
     * @brief Generate unique key for resource of type T.
     *
     * This method accepts a particular resource and generates the
     * unique key basing on this resource and data already present
     * in the model. WARNING: if information
     * necessary for key generation is not present, this method throws
     * an exception of type KeyValueMissingError.
     *
     * @param resource Resource for which unique key should be generated.
     * @return Resource unique key.
     */
    template<class T, class ... Args>
    static std::string generate_key(const T& resource, const Args& ... args) {
        std::string unique_key{get_unique_key(resource)};

        if (unique_key.empty()) {
            throw agent_framework::KeyValueMissingError(std::string("Unique key is empty for ") + T::get_component().to_string());
        }

        return generate_key_base(resource) + unique_key;
    }

    /*!
     * @brief Set agent id for the key generator.
     * @param agent_id Agent id.
     */
    static void set_agent_id(const std::string& agent_id) {
        m_agent_id = agent_id;
    }

protected:
    /*!
     * @brief Generate unique key base.
     *
     * The purpose of this method is to generate a consistent
     * key base upon which the rest of the key is generated.
     *
     * @param resource Resource for which the key base should be generated.
     * @return Key base for the resource.
     */
    template<class T>
    static std::string generate_key_base(const T& resource) {
        return m_agent_id + m_keys_base_map.at(resource.get_component().to_string());
    }
    static std::string get_unique_key(const agent_framework::model::Manager& manager);
    static std::string get_unique_key(const agent_framework::model::Chassis&);
    static std::string get_unique_key(const agent_framework::model::Fabric&);
    static std::string get_unique_key(const agent_framework::model::System&);
    static std::string get_unique_key(const agent_framework::model::StorageService&);
    static std::string get_unique_key(const agent_framework::model::NetworkInterface& interface);
    static std::string get_unique_key(const agent_framework::model::Drive& drive);
    static std::string get_unique_key(const agent_framework::model::Volume& volume);
    static std::string get_unique_key(const agent_framework::model::StoragePool& storage_pool);
    static std::string get_unique_key(const agent_framework::model::Zone& zone);
    static std::string get_unique_key(const agent_framework::model::Endpoint& endpoint);

private:
    static const std::map<std::string, std::string> m_keys_base_map;
    static std::string m_agent_id;
};

template<>
std::string NvmeKeyGenerator::generate_key(const agent_framework::model::Metric& metric, const agent_framework::model::Resource& resource);

template<>
std::string NvmeKeyGenerator::generate_key(const agent_framework::model::MetricDefinition& metric_def);

}
}

