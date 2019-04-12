/*!
 * @brief Identifier attribute builder.
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation.
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
 * @file identifier_builder.hpp
 */


#pragma once



#include "agent-framework/module/model/attributes/identifier.hpp"



namespace agent_framework {
namespace discovery {

class IdentifierBuilder final {
public:
    using Identifier = agent_framework::model::attribute::Identifier;
    using Type = agent_framework::model::enums::IdentifierType;


    /*!
     * @brief Set UUID identifier
     * @tparam M Resource type
     * @param model Resource instance
     * @param uuid Identifier to set
     */
    template<typename M>
    static void set_uuid(M& model, const std::string& uuid) {
        add_or_update_identifier<M, Type::UUID>(model, uuid);
    }


    /*!
     * @brief Set NQN identifier
     * @tparam M Resource type
     * @param model Resource instance
     * @param uuid Identifier to set
     */
    template<typename M>
    static void set_nqn(M& model, const std::string& nqn) {
        add_or_update_identifier<M, Type::NQN>(model, nqn);
    }


private:

    template<typename M, Type::base_enum T>
    static void add_or_update_identifier(M& model, const std::string& value) {
        // Find existing identifier or add new one
        auto identifiers = model.get_identifiers();
        for (auto& identifier : identifiers) {
            if (identifier.get_durable_name_format() == T) {
                identifier.set_durable_name(value);
                model.set_identifiers(identifiers);
                return;
            }
        }
        add_identifier<M, T>(model, value);
    }


    template<typename M, Type::base_enum T>
    static void add_identifier(M& model, const std::string& value) {
        Identifier identifier{};
        identifier.set_durable_name(value);
        identifier.set_durable_name_format(T);
        model.add_identifier(std::move(identifier));
    }

};

}
}
