/*!
 * @brief Identifier attribute builder.
 *
 * @header{License}
 * @copyright Copyright (c) 2017-2018 Intel Corporation.
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
 * @header{Filesystem}
 * @file identifier_builder.hpp
 */


#pragma once



#include "agent-framework/module/model/attributes/identifier.hpp"


namespace agent {
namespace storage {
namespace discovery {

class IdentifierBuilder final {
public:
    using Identifier = agent_framework::model::attribute::Identifier;
    using Type = agent_framework::model::enums::IdentifierType;

    template <typename M>
    static void add_system_path_identifier(M& model, const std::string& system_path) {
        Identifier path_identifier{};
        path_identifier.set_durable_name(system_path);
        path_identifier.set_durable_name_format(Type::SystemPath);
        model.add_identifier(std::move(path_identifier));
    }

    template <typename M>
    static void add_uuid_identifier(M& model, const std::string& uuid) {
        Identifier uuid_identifier{};
        uuid_identifier.set_durable_name(uuid);
        uuid_identifier.set_durable_name_format(Type::UUID);
        model.add_identifier(std::move(uuid_identifier));
    }

    template <typename M>
    static void update_uuid_identifier(M& model, const std::string& uuid) {
        // Find existing UUID identifier or add new one
        auto identifiers = model.get_identifiers();
        for (auto& identifier : identifiers) {
            if (identifier.get_durable_name_format() == Type::UUID) {
                identifier.set_durable_name(uuid);
                model.set_identifiers(identifiers);
                return;
            }
        }
        add_uuid_identifier(model, uuid);
    }
};

}
}
}
