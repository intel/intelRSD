/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
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
 * @section DESCRIPTION
 *
 * @file schema_validator.cpp
 *
 * @brief SchemaValidator implementation
 * */

#include "configuration/schema_validator.hpp"
#include "configuration/schema_property.hpp"
#include "configuration/json_path.hpp"
#include "configuration/utils.hpp"
#include "json/json.hpp"
#include <map>

using namespace configuration;

class SchemaValidator::Impl {
public:
    void add_property(const SchemaProperty& property) {
        m_properties.push_back(property);
    }

    void add_mandatory_property_error(const std::string& path,
                                      SchemaErrors& errors) {
        SchemaErrors::Error error{path};
        error.add_error_message("Mandatory field '"+ path + "' is not present.");
        errors.add_error(error);
    }

    void validate_properties(SchemaErrors& errors) {
        for (const auto& property : m_properties) {
            const auto& path = property.get_path();
            bool is_mandatory = property.is_mandatory();

            // Check if field exists in JSON
            if (m_json_map.count(path) == 1) {
                const auto& json_value = m_json_map[path];
                check_property(property, json_value, errors);
            }
            else {
                if (is_mandatory) {
                    add_mandatory_property_error(path, errors);
                }
            }
        }
    }

    void validate(const json::Value& json, SchemaErrors& errors) {
        build_json_map(json);
        validate_properties(errors);
    }

    void check_property(const SchemaProperty& property,
                        const json::Value& value,
                        SchemaErrors& errors) {

        auto error = property.validate(value);
        if (error.count()) {
            error.set_path(property.get_path());
            error.set_value(json_value_to_string(value));
            errors.add_error(error);
        }
    }

private:
    using property_t = std::vector<SchemaProperty>;
    using map_t = std::map<std::string, json::Value>;
    property_t m_properties{};
    map_t m_json_map{};
    JsonPath m_json_path{};

    void build_json_map(const json::Value& json) {
        for (auto it = json.cbegin(); it != json.cend(); ++it) {
            m_json_path.push_key(it.key());
            const auto& json_value = *it;

            m_json_map[m_json_path.get_path()] = json_value;

            if (!json_value.is_array()) {
                build_json_map(json_value);
            }
            m_json_path.pop_key();
        }
    }
};

SchemaValidator::SchemaValidator() :
    m_impl{new SchemaValidator::Impl} {}

SchemaValidator::~SchemaValidator() {}

void SchemaValidator::add_property(const SchemaProperty& property) {
    m_impl->add_property(property);
}

void SchemaValidator::validate(const json::Value& json, SchemaErrors& errors) {
    m_impl->validate(json, errors);
}
