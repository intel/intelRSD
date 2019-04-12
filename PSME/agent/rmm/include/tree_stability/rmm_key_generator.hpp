/*!
 * @brief RMMKeyGenerator class interface.
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file rmm_key_generator.hpp
 */

#pragma once



#include "agent-framework/tree_stability/key_value_missing_error.hpp"

#include <map>
#include <string>



namespace agent {
namespace rmm {

/*!
 * @brief RmmKeyGenerator class interface.
 *
 * This class provides a uniform interface for generating unique keys
 * for resources. Those keys should be later used by during resource tree
 * stabilization. Please refer to the relevant source code file to check
 * what arguments should be provided for different resource types.
 */
class RmmKeyGenerator {
public:
    /*!
     * @brief Generate unique key for resource of type T.
     *
     * This method accepts a particular resource and generates the
     * unique key basing on this resource and data already present
     * in the model. However, it is also possible to provide additional
     * data via variable arguments list. WARNING: if information
     * necessary for key generation is not present, this method throws
     * an exception of type KeyValueMissingError.
     *
     * @param resource Resource for which unique key should be generated.
     * @return Resource unique key.
     * */
    template<class T, class ... Args>
    static const std::string generate_key(const T& resource, const Args& ... args);


    /*!
     * @brief Set agent id for the key generator.
     * @param agent_id Agent id.
     * */
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
     * */
    template<class T>
    static const std::string generate_key_base(const T& resource) {
        return m_agent_id + m_keys_base_map.at(resource.get_component().to_string());
    }


private:
    static const std::map<std::string, std::string> m_keys_base_map;
    static std::string m_agent_id;
};

}
}
