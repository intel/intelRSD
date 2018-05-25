/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2018 Intel Corporation
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
 * @file schema_reader.hpp
 *
 * @brief SchemaReader implementation
 * */

#include "configuration/schema_reader.hpp"
#include "configuration/schema_validator.hpp"
#include "configuration/schema_property.hpp"
#include "configuration/json_path.hpp"
#include "configuration/validators/validators.hpp"

using namespace configuration;

class SchemaReader::Impl {
    JsonPath m_json_path{};
public:
    using ValidatorJsonSPtr = std::shared_ptr<Validator<json::Value>>;

    bool load_schema(const json::Value& schema, SchemaValidator& validator) {
        for (auto it = schema.cbegin(); it != schema.cend(); it++) {
            m_json_path.push_key(it.key());

            if (is_validator_object(*it)) {
                bool mandatory = false;
                if (it->is_member(SchemaProperty::MANDATORY)) {
                    auto schema_item = *it;
                    mandatory = schema_item[SchemaProperty::MANDATORY].as_bool();
                }
                process_json_property(m_json_path.get_path(),
                                      *it, mandatory, validator);
                m_json_path.pop_key();
                continue;
            }

            load_schema(*it, validator);

            m_json_path.pop_key();
        }
        return true;
    }

    void process_json_property(const std::string& path,
                               const json::Value& value,
                               bool mandatory,
                               SchemaValidator& validator) {
        if (is_validator_object(value)) {
            auto property = make_property(path, mandatory);
            make_validators(property, value);

            validator.add_property(property);
        }
    }

    SchemaProperty make_property(const std::string& path, bool mandatory) {
        return SchemaProperty{path, mandatory};
    }

    void make_validators(SchemaProperty& property,
                         const json::Value& item) {
        if (auto validator = create_max_validator(item)) {
            property.add_validator(validator);
        }

        if (auto validator = create_min_validator(item)) {
            property.add_validator(validator);
        }

        if (auto validator = create_more_than_validator(item)) {
            property.add_validator(validator);
        }

        if (auto validator = create_type_validator(item)) {
            property.add_validator(validator);
        }

        if (auto validator = create_address_validator(item)) {
            property.add_validator(validator);
        }

        if (auto validator = create_url_validator(item)) {
            property.add_validator(validator);
        }

        if (auto validator = create_anyof_validator(item)) {
            property.add_validator(validator);
        }
    }

    ValidatorJsonSPtr create_max_validator(const json::Value& value) {
        if (value[SchemaProperty::MAX].is_number()) {
            return std::make_shared<MaxValidator>(
                    value[SchemaProperty::MAX].as_int());
        }
        return nullptr;
    }

    ValidatorJsonSPtr create_min_validator(const json::Value& value) {
        if (value[SchemaProperty::MIN].is_number()) {
            return std::make_shared<MinValidator>(
                    value[SchemaProperty::MIN].as_int());
        }
        return nullptr;
    }

    ValidatorJsonSPtr create_more_than_validator(const json::Value& value) {
        if (value[SchemaProperty::MORE_THAN].is_number()) {
            return std::make_shared<MoreThanValidator>(
                    value[SchemaProperty::MORE_THAN].as_double());
        }
        return nullptr;
    }

    ValidatorJsonSPtr create_type_validator(const json::Value& value) {
        if (value[SchemaProperty::TYPE].is_string()) {
            return std::make_shared<TypeValidator>(
                    value[SchemaProperty::TYPE].as_string());
        }
        return nullptr;
    }

    ValidatorJsonSPtr create_address_validator(const json::Value& value) {
        if (value[SchemaProperty::ADDRESS].is_string()) {
            return std::make_shared<AddressValidator>(
                    value[SchemaProperty::ADDRESS].as_string());
        }
        return nullptr;
    }

    ValidatorJsonSPtr create_url_validator(const json::Value& value) {
        if (value[SchemaProperty::URL].is_string()) {
            return std::make_shared<UrlValidator>(
                    value[SchemaProperty::URL].as_string());
        }
        return nullptr;
    }

    ValidatorJsonSPtr create_anyof_validator(const json::Value& value) {
        if (value[SchemaProperty::ANY_OF].is_array() && value.is_member(SchemaProperty::TYPE)) {
            const auto type = value[SchemaProperty::TYPE].as_string();
            if (SchemaProperty::STRING == type) {
                return create_anyof_string(value);
            }

            if (SchemaProperty::INT == type) {
                return create_anyof_int(value);
            }
        }
        return nullptr;
    }

    ValidatorJsonSPtr create_anyof_string(const json::Value& value) {
        auto validator = std::make_shared<AnyOfValidator<std::string>>();
        for (const auto& val : value[SchemaProperty::ANY_OF]) {
            if (val.is_string()) {
                validator->add_constraint(val.as_string());
            }
        }
        return validator;
    }

    ValidatorJsonSPtr create_anyof_int(const json::Value& value) {
        auto validator = std::make_shared<AnyOfValidator<int>>();
        for (const auto& val : value[SchemaProperty::ANY_OF]) {
            if (val.is_number()) {
                validator->add_constraint(val.as_int());
            }
        }
        return validator;
    }


    bool is_validator_object(const json::Value& value) {
         return value.is_object() && value.is_member(SchemaProperty::VALIDATOR) &&
             (value.is_member(SchemaProperty::MAX) ||
              value.is_member(SchemaProperty::MIN) ||
              value.is_member(SchemaProperty::MORE_THAN) ||
              value.is_member(SchemaProperty::TYPE) ||
              value.is_member(SchemaProperty::ADDR_V) ||
              value.is_member(SchemaProperty::ADDRESS) ||
              value.is_member(SchemaProperty::URL) ||
              value.is_member(SchemaProperty::ANY_OF));
    }
};

SchemaReader::SchemaReader() :
    m_impl{new SchemaReader::Impl} {}

SchemaReader::~SchemaReader() {}

bool SchemaReader::load_schema(const json::Value& schema,
                               SchemaValidator& validator) {
    return m_impl->load_schema(schema, validator);
}
