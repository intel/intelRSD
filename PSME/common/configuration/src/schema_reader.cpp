/*!
 * @section LICENSE
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
 * @section DESCRIPTION
 *
 * @file schema_reader.cpp
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
    using ValidatorJsonSPtr = std::shared_ptr<Validator<json::Json>>;

    bool load_schema(const json::Json& schema, SchemaValidator& validator) {
        if (schema.is_object()) {
            for (auto it = schema.cbegin(); it != schema.cend(); it++) {
                m_json_path.push_key(it.key());

                if (is_validator_object(*it)) {
                    bool mandatory = false;
                    if (it->count(SchemaProperty::MANDATORY)) {
                        auto schema_item = *it;
                        mandatory = schema_item.value(SchemaProperty::MANDATORY, bool{});
                    }
                    process_json_property(m_json_path.get_path(),
                                          *it, mandatory, validator);
                    m_json_path.pop_key();
                    continue;
                }

                load_schema(*it, validator);

                m_json_path.pop_key();
            }
        }
        return true;
    }

    void process_json_property(const std::string& path,
                               const json::Json& value,
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
                         const json::Json& item) {
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

    ValidatorJsonSPtr create_max_validator(const json::Json& value) {
        if (value.value(SchemaProperty::MAX, json::Json()).is_number()) {
            return std::make_shared<MaxValidator>(
                value.value(SchemaProperty::MAX, int{}));
        }
        return nullptr;
    }

    ValidatorJsonSPtr create_min_validator(const json::Json& value) {
        if (value.value(SchemaProperty::MIN, json::Json()).is_number()) {
            return std::make_shared<MinValidator>(
                value.value(SchemaProperty::MIN, int{}));
        }
        return nullptr;
    }

    ValidatorJsonSPtr create_more_than_validator(const json::Json& value) {
        if (value.value(SchemaProperty::MORE_THAN, json::Json()).is_number()) {
            return std::make_shared<MoreThanValidator>(
                    value.value(SchemaProperty::MORE_THAN, double{}));
        }
        return nullptr;
    }

    ValidatorJsonSPtr create_type_validator(const json::Json& value) {
        if (value.value(SchemaProperty::TYPE, json::Json()).is_string()) {
            return std::make_shared<TypeValidator>(
                value.value(SchemaProperty::TYPE, std::string{}));
        }
        return nullptr;
    }

    ValidatorJsonSPtr create_address_validator(const json::Json& value) {
        if (value.value(SchemaProperty::ADDRESS, json::Json()).is_string()) {
            return std::make_shared<AddressValidator>(
                value.value(SchemaProperty::ADDRESS, std::string{}));
        }
        return nullptr;
    }

    ValidatorJsonSPtr create_url_validator(const json::Json& value) {
        if (value.value(SchemaProperty::URL, json::Json()).is_string()) {
            return std::make_shared<UrlValidator>(
                value.value(SchemaProperty::URL, std::string{}));
        }
        return nullptr;
    }

    ValidatorJsonSPtr create_anyof_validator(const json::Json& value) {
        if (value.value(SchemaProperty::ANY_OF, json::Json()).is_array() && value.count(SchemaProperty::TYPE)) {
            const auto type = value.value(SchemaProperty::TYPE, std::string{});
            if (SchemaProperty::STRING == type) {
                return create_anyof_string(value);
            }

            if (SchemaProperty::INT == type) {
                return create_anyof_int(value);
            }
        }
        return nullptr;
    }

    ValidatorJsonSPtr create_anyof_string(const json::Json& value) {
        auto validator = std::make_shared<AnyOfValidator<std::string>>();
        for (const auto& val : value[SchemaProperty::ANY_OF]) {
            if (val.is_string()) {
                validator->add_constraint(val.get<std::string>());
            }
        }
        return validator;
    }

    ValidatorJsonSPtr create_anyof_int(const json::Json& value) {
        auto validator = std::make_shared<AnyOfValidator<int>>();
        for (const auto& val : value[SchemaProperty::ANY_OF]) {
            if (val.is_number()) {
                validator->add_constraint(val.get<int>());
            }
        }
        return validator;
    }


    bool is_validator_object(const json::Json& value) {
         return value.is_object() && value.count(SchemaProperty::VALIDATOR) &&
             (value.count(SchemaProperty::MAX) ||
                 value.count(SchemaProperty::MIN) ||
                 value.count(SchemaProperty::MORE_THAN) ||
                 value.count(SchemaProperty::TYPE) ||
                 value.count(SchemaProperty::ADDR_V) ||
                 value.count(SchemaProperty::ADDRESS) ||
                 value.count(SchemaProperty::URL) ||
                 value.count(SchemaProperty::ANY_OF));
    }
};

SchemaReader::SchemaReader() :
    m_impl{new SchemaReader::Impl} {}

SchemaReader::~SchemaReader() {}

bool SchemaReader::load_schema(const json::Json& schema,
                               SchemaValidator& validator) {
    return m_impl->load_schema(schema, validator);
}
