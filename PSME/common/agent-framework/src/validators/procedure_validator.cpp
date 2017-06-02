/*!
 * @brief Implementation of "extended" validators.
 *
 * File contains all definitions of methods and all classes derived from ValidityChecker.
 *
 * @copyright Copyright (c) 2016-2017 Intel Corporation
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
 * @header{Files}
 * @file procedure_validator.cpp
 */

#include "agent-framework/validators/procedure_validator.hpp"

#include "agent-framework/validators/checkers/composite_validity_checker.hpp"
#include "agent-framework/validators/checkers/optional_validity_checker.hpp"
#include "agent-framework/validators/checkers/null_allowed_validity_checker.hpp"
#include "agent-framework/validators/checkers/number_validity_checker.hpp"
#include "agent-framework/validators/checkers/jsonrpc_validity_checker.hpp"
#include "agent-framework/validators/checkers/uuid_validity_checker.hpp"
#include "agent-framework/validators/checkers/array_validity_checker.hpp"
#include "agent-framework/validators/checkers/always_fail_validity_checker.hpp"
#include "agent-framework/validators/checkers/enum_validity_checker.hpp"
#include "agent-framework/validators/checkers/attribute_validity_checker.hpp"
#include "agent-framework/validators/checkers/regex_validity_checker.hpp"

#include <cassert>



using namespace std;
using namespace agent_framework::exceptions;

namespace jsonrpc {

/*!
 * @brief Mapping between local validators and JSONRPC types.
 *
 * JSONRPC validators are executed before the command reaches "our" code, no
 * usefull reports are logged (only "something in the request is wrong")
 * Reports from custom validators are much more "readable" (at least more custom
 * message with the field name is logged)
 *
 * @bug "double" validation takes place for the document.
 */
const ProcedureValidator::JsonrpcValidators ProcedureValidator::JSONRPC_VALIDATORS = {
    {ProcedureValidator::Validators::valid_any, static_cast<jsontype_t>(ProcedureValidator::Validators::valid_any)},

    {ProcedureValidator::Validators::valid_string, JSON_STRING},
    {ProcedureValidator::Validators::valid_boolean, JSON_BOOLEAN},
    {ProcedureValidator::Validators::valid_integer, JSON_INTEGER},
    {ProcedureValidator::Validators::valid_real, JSON_REAL},
    {ProcedureValidator::Validators::valid_object, JSON_OBJECT},

    {ProcedureValidator::Validators::valid_array, JSON_ARRAY},
    {ProcedureValidator::Validators::valid_array_of, JSON_ARRAY},
    {ProcedureValidator::Validators::valid_array_size_of, JSON_ARRAY},

    {ProcedureValidator::Validators::valid_enum, JSON_STRING},
    {ProcedureValidator::Validators::valid_uuid, JSON_STRING},
    {ProcedureValidator::Validators::valid_regex, JSON_STRING},

    {ProcedureValidator::Validators::valid_attribute, JSON_OBJECT},

    {ProcedureValidator::Validators::valid_never, static_cast<jsontype_t>(ProcedureValidator::Validators::valid_any)}
};

ProcedureValidator::ProcedureValidator(const string& name,
                                       parameterDeclaration_t param_type,
                                       jsontype_t return_type,
                                       ...) : Procedure(name, param_type, return_type, nullptr) {
    va_list parameters;
    va_start(parameters, return_type);
    create_validators(parameters);
    va_end(parameters);
}

ProcedureValidator::ProcedureValidator(const string& name,
                                       parameterDeclaration_t param_type,
                                       ...) : Procedure(name, param_type, nullptr) {
    va_list parameters;
    va_start(parameters, param_type);
    create_validators(parameters);
    va_end(parameters);
}

ProcedureValidator::ProcedureValidator(const Procedure& procedure) : Procedure(procedure) {
    /* nothing added here, validation done only via JSON validator */
}

void ProcedureValidator::create_validators(va_list args) {
    const char* name;
    while ((name = va_arg(args, const char*)) != nullptr) {
        unsigned type = va_arg(args, unsigned);

        /*
         * add validator to JSONRPC if "exact" form. Optionals and nulls
         * aren't checked here!
         */
        JsonrpcValidators::const_iterator jit = JSONRPC_VALIDATORS.find(static_cast<Validators>(type));
        if (jit != JSONRPC_VALIDATORS.cend()) {
            /* Procedure:: */ AddParameter(name, (*jit).second);
        }

        /*
         * Check if field was already added, in this case "merge" validators
         * with "and" condition.
         */
        bool merged = false;
        for (auto& v : validators) {
            if (v.name == name) {
                ValidityChecker::Ptr first;
                first.swap(v.validator);
                ValidityChecker::Ptr second = create_validator(type, args);
                ValidityChecker::Ptr replacement(new CompositeValidityChecker(first, second));
                v.validator.swap(replacement);
                merged = true;
                break;
            }
        }

        if (!merged) {
            validators.push_back({name, create_validator(type, args)});
        }
    }
}

ValidityChecker::Ptr ProcedureValidator::create_validator(unsigned type, va_list args) {
    if ((type & (VALID_OPTIONAL(0))) == (VALID_OPTIONAL(0))) {
        ValidityChecker::Ptr cond = create_validator(type & (~(VALID_OPTIONAL(0))), args);
        return ValidityChecker::Ptr(new OptionalValidityChecker(cond));
    }
    if ((type & (VALID_NULLABLE(0))) == (VALID_NULLABLE(0))) {
        ValidityChecker::Ptr cond = create_validator(type & (~(VALID_NULLABLE(0))), args);
        return ValidityChecker::Ptr(new NullAllowedValidityChecker(cond));
    }

    switch (static_cast<Validators>(type)) {
        case Validators::valid_never:
            return ValidityChecker::Ptr(new AlwaysFailValidityChecker("This value is never valid."));
        case Validators::valid_always:
            return ValidityChecker::Ptr(nullptr);

        /*
         * Plain JSONRPC checks.
         * Checking might be run twice: field is checked by JSONRPC and here if field is obligatory.
         */
        case Validators::valid_any:
            return ValidityChecker::Ptr(new ValidityChecker());

        case Validators::valid_string:
        case Validators::valid_boolean:
        case Validators::valid_integer:
        case Validators::valid_real:
        case Validators::valid_object:
        case Validators::valid_array:
            return ValidityChecker::Ptr(new JsonrpcValidityChecker(static_cast<jsontype_t>(type)));

        case Validators::valid_array_of: {
            type = va_arg(args, unsigned);
            ValidityChecker::Ptr cond = create_validator(type, args);
            return ValidityChecker::Ptr(new ArrayValidityChecker(cond));
        }
        case Validators::valid_array_size_of: {
            unsigned min = va_arg(args, unsigned);
            unsigned max = va_arg(args, unsigned);
            type = va_arg(args, unsigned);
            ValidityChecker::Ptr cond = create_validator(type, args);
            return ValidityChecker::Ptr(new ArrayValidityChecker(cond, min, max));
        }

        case Validators::valid_uuid:
            return ValidityChecker::Ptr(new UuidValidityChecker());

        /* cannot validate the field, validator not yet created */
        case Validators::valid_numeric:
            return ValidityChecker::Ptr(new NumberValidityChecker());

        case Validators::valid_numeric_typed:
            return ValidityChecker::Ptr(new NumberValidityChecker(args));

        case Validators::valid_enum:
            return ValidityChecker::Ptr(new EnumValidityChecker(args));

	case Validators::valid_regex:
            return ValidityChecker::Ptr(new RegexValidityChecker(args));

        case Validators::valid_attribute:
            return ValidityChecker::Ptr(new AttributeValidityChecker(args));

        default:
            assert(fail("Handled validator type"));
            return ValidityChecker::Ptr(new AlwaysFailValidityChecker("Unhandled validator type."));
    }
}

unsigned ProcedureValidator::remove_from(Json::Value::Members& members, const std::string& name) {
    unsigned entries = 0;
    Json::Value::Members::iterator it = members.begin();
    while (it != members.end()) {
        if (name == (*it)) {
            it = members.erase(it);
            entries++;
        }
        else {
            it++;
        }
    }
    return entries;
}

void ProcedureValidator::validate(const Json::Value& request) const {
    try {
        if (request.isNull()) {
            if (validators.empty()) {
                /* nothing to validate */
                return;
            }
        }
        else if (!request.isObject()) {
            THROW(ValidityChecker::ValidationException, "agent-framework",
                  ErrorCode::INVALID_FIELD, "Request is not a JSON object.", request);
        }

        Json::Value::Members members = request.getMemberNames();
        for (auto const& v : validators) {
            unsigned removed = remove_from(members, v.name);

            try {
                switch (removed) {
                    case 0:
                        if (v.validator) {
                            v.validator->validate(ValidityChecker::NON_EXISTING_VALUE);
                        }
                        break;
                    case 1:
                        if (v.validator) {
                            v.validator->validate(request[v.name]);
                        }
                        break;
                    default:
                        /*!
                         * @bug Duplicated values checking is not detected with current JSONCPP.
                         *
                         * JSONCPP doesn't handle duplicated values properly (the last one overrides previous ones).
                         * So checking if duplicated is not available.
                         * */
                        THROW(ValidityChecker::ValidationException, "agent-framework",
                              ErrorCode::DUPLICATED_FIELD, "Duplicated field in JSON.", request[v.name]);
                        break;
                }
            }
            catch (ValidityChecker::ValidationException& ex) {
                /* rethrow exception with current field appended */
                ex.append(v.name);
                throw ex;
            }
        }

        if (!members.empty()) {
            const auto& member = members.front();
            THROW(ValidityChecker::ValidationException, "agent-framework",
                  ErrorCode::UNEXPECTED_FIELD, "Unexpected field in json.",
                  request[member], member);
        }

        /* JSON document is valid if no exception is thrown */
        return;
    }
    catch (const ValidityChecker::ValidationException& ex) {
        throw agent_framework::exceptions::GamiException(
            ex.get_error_code(), ex.get_message(),
            InvalidField::create_json_data_from_field(ex.get_field(), ex.get_field_value()));
    }
}

Json::Value ProcedureValidator::to_json_rpc(const json::Value& src) {
    switch (src.get_type()) {
        case json::Value::Type::NIL:
            return Json::Value();
        case json::Value::Type::OBJECT: {
            Json::Value val{Json::objectValue};
            for (const json::Pair& pair : src.as_object()) {
                val[pair.first] = to_json_rpc(pair.second);
            }
            return val;
        }
        case json::Value::Type::ARRAY: {
            Json::Value val{Json::arrayValue};
            for (size_t i = 0; i < src.size(); i++) {
                val[static_cast<int>(i)] = to_json_rpc(src[i]);
            }
            return val;
        }
        case json::Value::Type::STRING:
            return Json::Value(src.as_string());
        case json::Value::Type::NUMBER:
            if (src.is_int()) {
                // a workaround for Json-CPP "ambiguous conversion" or "call to constructor is ambiguous" errors.
                long long int number = src.as_int64();
                return Json::Value(number);
            }
            if (src.is_uint()) {
                // see comment above
                unsigned long long int number = src.as_uint64();
                return Json::Value(number);
            }
            return Json::Value(src.as_double());
        case json::Value::Type::BOOLEAN:
            return Json::Value(src.as_bool());

        default:
            assert(fail("Unhandled json value type"));
            return Json::Value();
    }
}

void ProcedureValidator::validate(const json::Value& req) const {
    Json::Value value = to_json_rpc(req);
    validate(value);
}

} /*! @i{jsonrpc namespace} */
