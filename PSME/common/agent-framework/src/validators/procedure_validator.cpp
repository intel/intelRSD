/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2016 Intel Corporation
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
*/

#include "agent-framework/validators/procedure_validator.hpp"
#include "agent-framework/exceptions/exception.hpp"

#include <string.h>
#include <assert.h>
#include <cmath>
#include <stdarg.h>

using namespace std;
using namespace agent_framework::exceptions;

namespace jsonrpc {

#define INVALID(m) \
    THROW(InvalidParameters, "rest", m)

ValidityChecker::~ValidityChecker() { }

void ValidityChecker::validate(const Json::Value &value) const {
    // optional value is not allowed
    if (&value == &(ProcedureValidator::NON_EXISTING_VALUE)) {
        INVALID("Obligatory value");
    }
    if (value.isNull()) {
        INVALID("null value not allowed");
    }
}

constexpr bool FAIL(const char*) {
    return false;
}

/*! @brief AND condition checker */
class CompositeValidityChecker final : public ValidityChecker {
private:
    ValidityCheckerPtr checker1{};
    ValidityCheckerPtr checker2{};
public:
    CompositeValidityChecker(
            ValidityCheckerPtr& _checker1,
            ValidityCheckerPtr& _checker2);

    virtual void validate(const Json::Value& value) const;
};

CompositeValidityChecker::CompositeValidityChecker(
        ValidityCheckerPtr& _checker1, ValidityCheckerPtr& _checker2)
{
    checker1.swap(_checker1);
    checker2.swap(_checker2);
}

void CompositeValidityChecker::validate(const Json::Value& value) const {
    if (checker1) {
        checker1->validate(value);
    }
    if (checker2) {
        checker2->validate(value);
    }
}



/*! @brief Optional value checker */
class OptionalValidityChecker final : public ValidityChecker {
private:
    ValidityCheckerPtr checker{};

public:
    OptionalValidityChecker(ValidityCheckerPtr& _checker);

    virtual void validate(const Json::Value& value) const;
};

OptionalValidityChecker::OptionalValidityChecker(ValidityCheckerPtr& _checker) {
    checker.swap(_checker);
}

void OptionalValidityChecker::validate(const Json::Value& value) const {
    // compare object directly! It is internal "marker"
    if (&value == &(ProcedureValidator::NON_EXISTING_VALUE)) {
        return;
    }
    if (checker) {
        checker->validate(value);
    }
}



/*! @brief Null value allowed checker */
class NullAllowedValidityChecker final : public ValidityChecker {
private:
    ValidityCheckerPtr checker{};

public:
    NullAllowedValidityChecker(ValidityCheckerPtr& _checker);

    virtual void validate(const Json::Value& value) const;
};

NullAllowedValidityChecker::NullAllowedValidityChecker(ValidityCheckerPtr& _checker) {
    checker.swap(_checker);
}

void NullAllowedValidityChecker::validate(const Json::Value& value) const {
    // only if value IS in the document
    if ((&value != &(ProcedureValidator::NON_EXISTING_VALUE)) && (value.isNull())) {
        return;
    }
    if (checker) {
        checker->validate(value);
    }
}



/*! @brief number checker */
class NumberValidityChecker final : public ValidityChecker {
private:
    ProcedureValidator::NumberValidityChecker_t type{};

    // don't want to play with pointers.. just dummy flags and union
    bool hasMin{false};
    bool hasMax{false};

    union ValueUnion {
        int32_t i32;
        uint32_t ui32;
        double d;
    };

    ValueUnion min{};
    ValueUnion max{};

public:
    NumberValidityChecker();
    NumberValidityChecker(va_list args);

    virtual void validate(const Json::Value& value) const;

private:
    static bool isIntegral(double d);
};

NumberValidityChecker::NumberValidityChecker():
    type(ProcedureValidator::NumberValidityChecker_t::TYPE_ANYTHING),
    hasMin(false),
    hasMax(false)
{
    memset(&min, 0, sizeof(ValueUnion));
    memset(&max, 0, sizeof(ValueUnion));
}

NumberValidityChecker::NumberValidityChecker(va_list args) {
    memset(&min, 0, sizeof(ValueUnion));
    memset(&max, 0, sizeof(ValueUnion));

    unsigned t = va_arg(args, unsigned);
    hasMin = ((t & VALID_NUMERIC_TYPE(ANYTHING, MIN)) == VALID_NUMERIC_TYPE(ANYTHING, MIN));
    hasMax = ((t & VALID_NUMERIC_TYPE(ANYTHING, MAX)) == VALID_NUMERIC_TYPE(ANYTHING, MAX));
    type = static_cast<ProcedureValidator::NumberValidityChecker_t>
                    (t & (~VALID_NUMERIC_TYPE(ANYTHING, BOTH)));

    // clear bits responsible for number of arguments to be taken from args
    switch (type) {
        case ProcedureValidator::NumberValidityChecker_t::TYPE_INT32:
            if (hasMin) { min.i32 = va_arg(args, int32_t); }
            if (hasMax) { max.i32 = va_arg(args, int32_t); }
            break;
        case ProcedureValidator::NumberValidityChecker_t::TYPE_UINT32:
            if (hasMin) { min.ui32 = va_arg(args, uint32_t); }
            if (hasMax) { max.ui32 = va_arg(args, uint32_t); }
            break;
        case ProcedureValidator::NumberValidityChecker_t::TYPE_DOUBLE:
            if (hasMin) { min.d = va_arg(args, double); }
            if (hasMax) { max.d = va_arg(args, double); }
            break;

        // constraint checks for numbers
        case ProcedureValidator::NumberValidityChecker_t::CONSTR_NOTHING:
        case ProcedureValidator::NumberValidityChecker_t::CONSTR_MIN:
        case ProcedureValidator::NumberValidityChecker_t::CONSTR_MAX:
        case ProcedureValidator::NumberValidityChecker_t::CONSTR_BOTH:
        // not allowed types
        case ProcedureValidator::NumberValidityChecker_t::TYPE_ANYTHING:
        case ProcedureValidator::NumberValidityChecker_t::WRONG_TYPE:
        // to avoid compiler warinings..
        default:
            assert(FAIL("Allowed number type"));
            type = ProcedureValidator::NumberValidityChecker_t::WRONG_TYPE;
            break;
    }
}

bool NumberValidityChecker::isIntegral(double d) {
    double result = std::fabs(std::fmod(d, 1.0));
    if (result < -0.5) {
        result += 1.0;
    } else if (result > 0.5) {
        result -= 1.0;
    }
    if (result < 0) {
        result = -result;
    }
    return result < 1.0e-10;
}

void NumberValidityChecker::validate(const Json::Value& value) const {
    ValidityChecker::validate(value);

    switch (type) {
        case ProcedureValidator::NumberValidityChecker_t::TYPE_ANYTHING:
            if (value.isBool()) {
                INVALID("Bool not a number");
            }
            if (!value.isNumeric()) {
                INVALID("Not a valid number");
            }
            break;

        case ProcedureValidator::NumberValidityChecker_t::TYPE_INT32:
            if (!value.isConvertibleTo(Json::ValueType::intValue)) {
                INVALID("Value cannot be converted to INT");
            }
            if (value.isDouble() && (!isIntegral(value.asDouble()))) {
                INVALID("DOUBLE value with partial");
            }
            if ((hasMin && (value.asInt() < min.i32)) ||
                (hasMax && (value.asInt() > max.i32)))
            {
                INVALID("INT value out of range");
            }
            break;

        case ProcedureValidator::NumberValidityChecker_t::TYPE_UINT32:
            if (!value.isConvertibleTo(Json::ValueType::uintValue)) {
                INVALID("Value cannot be converted to UINT");
            }
            if (value.isDouble() && (!isIntegral(value.asDouble()))) {
                INVALID("DOUBLE value with partial");
            }
            if ((hasMin && (value.asUInt() < min.ui32)) ||
                (hasMax && (value.asUInt() > max.ui32)))
            {
                INVALID("UINT value out of range");
            }
            break;

        case ProcedureValidator::NumberValidityChecker_t::TYPE_DOUBLE:
            if (!value.isNumeric()) {
                INVALID("Not a valid number");
            }
            if ((hasMin && (value.asDouble() < min.d)) ||
                (hasMax && (value.asDouble() > max.d)))
            {
                INVALID("DOUBLE value out of range");
            }
            break;

        case ProcedureValidator::NumberValidityChecker_t::WRONG_TYPE:
        case ProcedureValidator::NumberValidityChecker_t::CONSTR_NOTHING:
        case ProcedureValidator::NumberValidityChecker_t::CONSTR_MIN:
        case ProcedureValidator::NumberValidityChecker_t::CONSTR_MAX:
        case ProcedureValidator::NumberValidityChecker_t::CONSTR_BOTH:
        default:
            assert(FAIL("Allowed number type"));
            INVALID("Not allowed number type");
            break;
    }
}

/*! @brief jsonrpc types checker */
class JsonrpcValidityChecker final : public ValidityChecker {
private:
    jsontype_t type{};

public:
    JsonrpcValidityChecker(jsontype_t _type);

    virtual void validate(const Json::Value& value) const;
};

JsonrpcValidityChecker::JsonrpcValidityChecker(jsontype_t _type):
    type(_type)
{}

void JsonrpcValidityChecker::validate(const Json::Value& value) const {
    ValidityChecker::validate(value);

    switch (type) {
        case jsontype_t::JSON_STRING:
            if (!value.isString()) {
                INVALID("Not a JSON string");
            }
            break;
        case jsontype_t::JSON_BOOLEAN:
            if (!value.isBool()) {
                INVALID("Not a JSON boolean");
            }
            break;
        case jsontype_t::JSON_INTEGER:
            if (!value.isInt()) {
                INVALID("Not a JSON integer");
            }
            break;
        case jsontype_t::JSON_REAL:
            if (!value.isDouble()) {
                INVALID("Not a JSON real");
            }
            break;
        case jsontype_t::JSON_OBJECT:
            if (!value.isObject()) {
                INVALID("Not a JSON object");
            }
            break;
        case jsontype_t::JSON_ARRAY:
            if (!value.isArray()) {
                INVALID("Not a JSON array");
            }
            break;
        default:
            assert(FAIL("Allowed JSON type"));
            INVALID("Not allowed JSON type");
            break;
    }
}



/*! @brief UUID checker */
class UuidValidityChecker final : public ValidityChecker {
public:
    UuidValidityChecker();

    virtual void validate(const Json::Value& value) const;

private:
    constexpr static bool isHex(char c);
};

UuidValidityChecker::UuidValidityChecker() {}

void UuidValidityChecker::validate(const Json::Value& value) const {
    ValidityChecker::validate(value);
    if (!value.isString()) {
        INVALID("Not a valid string");
    }

    std::string sval = value.asString();
    static const std::string uuid_pattern{"xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx"};
    if (sval.length() != uuid_pattern.length()) {
        INVALID("Incorrect UUID lenght");
    }
    for (unsigned i = 0; i < uuid_pattern.length(); i++) {
        if (uuid_pattern.at(i) == 'x') {
            if (!isHex(sval.at(i))) {
                INVALID("Invalid UUID hex number");
            }
        } else if (uuid_pattern.at(i) != sval.at(i)) {
            INVALID("Invalid UUID format");
        }
    }
}

constexpr bool UuidValidityChecker::isHex(char c) {
    return
            (('0' <= c) && (c <= '9')) ||
            (('a' <= c) && (c <= 'f')) ||
            (('A' <= c) && (c <= 'F'));
}



/*! @brief Array validator */
class ArrayValidityChecker final : public ValidityChecker {
private:
    ValidityCheckerPtr checker{};
    unsigned min;
    unsigned max;

public:
    ArrayValidityChecker(
            ValidityCheckerPtr& _checker,
            unsigned _min = 0, unsigned _max = UINT32_MAX);

    virtual void validate(const Json::Value& value) const;
};

ArrayValidityChecker::ArrayValidityChecker(ValidityCheckerPtr& _checker, unsigned _min, unsigned _max):
    min(_min),
    max(_max)
{
    checker.swap(_checker);
}

void ArrayValidityChecker::validate(const Json::Value& value) const {
    ValidityChecker::validate(value);
    if (!value.isArray()) {
        INVALID("Not a valid array");
    }
    if (max < min) {
        INVALID("Wrong array size");
    }

    if ((min <= value.size()) && (value.size() <= max)) {
        if (checker) {
            // rethrowing and exception, almost copy from ProcedureValidator::validate()..
            for (unsigned index = 0; index < value.size(); index++) {
                // CLion warning if not separated into two instructions..
                std::string id = "[" + std::to_string(index); id += "]";
                try {
                    checker->validate(value[index]);
                } catch (InvalidField fe) {
                    fe.append(id);
                    throw fe;
                } catch (InvalidParameters pe) {
                    // throw invalid field
                    throw InvalidField(pe.get_message(), id);
                }
            }
        }
    } else {
        INVALID("Invalid array size");
    }
}



/*!
 * @brief Always fail validity checker
 * Always reports error message
 * */
class AlwaysFailValidityChecker final : public ValidityChecker {
private:
    const std::string message;

public:
    AlwaysFailValidityChecker(const std::string& _message);
    virtual ~AlwaysFailValidityChecker();

    virtual void validate(const Json::Value&) const __attribute((noreturn));
};

AlwaysFailValidityChecker::AlwaysFailValidityChecker(const std::string& _message):
    message(_message)
{}

AlwaysFailValidityChecker::~AlwaysFailValidityChecker() {}

void AlwaysFailValidityChecker::validate(const Json::Value&) const {
    INVALID(message);
}



/*! @brief Enum validity checker */
class EnumValidityChecker final : public ValidityChecker {
private:
    using is_allowable_value_t = bool (*)(const std::string&);
    is_allowable_value_t is_allowable_value{nullptr};

public:
    EnumValidityChecker(va_list args);
    virtual ~EnumValidityChecker();
    // public/default to avoid CLion warning.. Should be private/deleted
    EnumValidityChecker(const EnumValidityChecker&) = default;

    virtual void validate(const Json::Value& value) const;
private:
    EnumValidityChecker& operator=(const EnumValidityChecker&) = delete;
    EnumValidityChecker() = delete;
};

EnumValidityChecker::EnumValidityChecker(va_list args) {
    is_allowable_value = va_arg(args, is_allowable_value_t);
}

EnumValidityChecker::~EnumValidityChecker() {}

void EnumValidityChecker::validate(const Json::Value& value) const {
    ValidityChecker::validate(value);
    if (!value.isString()) {
        INVALID("Not a valid string");
    }
    if (!is_allowable_value(value.asString())) {
        INVALID("Value not in the enum");
    }
}



/*! @brief Attribute validity checker */
class AttributeValidityChecker final : public ValidityChecker {
private:
    // procedure validator method from the *static* object shall be passed
    // Singletons are adviced to be used.
    using static_procedure_getter_t = const jsonrpc::ProcedureValidator& (*)(void);
    const ProcedureValidator* validator{nullptr};

public:
    AttributeValidityChecker(va_list args);
    virtual ~AttributeValidityChecker();
    // public/default to avoid CLion warning.. Should be private/deleted
    AttributeValidityChecker(const AttributeValidityChecker&) = default;

    virtual void validate(const Json::Value& value) const;
private:
    AttributeValidityChecker& operator=(const AttributeValidityChecker&) = delete;
    AttributeValidityChecker() = delete;
};

AttributeValidityChecker::AttributeValidityChecker(va_list args) {
    static_procedure_getter_t getter = va_arg(args, static_procedure_getter_t);
    if (getter) {
        validator = &(getter());
    }
}

AttributeValidityChecker::~AttributeValidityChecker() {}

void AttributeValidityChecker::validate(const Json::Value& value) const {
    ValidityChecker::validate(value);
    if (!value.isObject()) {
        INVALID("Not a valid attribute");
    }

    // validate using given procedure_validator
    if (validator) {
        validator->validate(value);
    }
}





const Json::Value ProcedureValidator::NON_EXISTING_VALUE;


/* JSONRPC validators.. the question is whether are they anyhow useful?
 * JSONRPC validators are executed before the command reaches "our" code, no
 * usefull reports are logged (only "something in the request is wrong")
 * Reports from custom validators are much more "readable" (at least more custom
 * message with the field name is logged)
 */
const ProcedureValidator::JsonrpcValidators_t ProcedureValidator::JSONRPC_VALIDATORS = {
    {ProcedureValidator::Validators_t::valid_any,
        static_cast<jsontype_t>(ProcedureValidator::Validators_t::valid_any)},

    {ProcedureValidator::Validators_t::valid_string, JSON_STRING},
    {ProcedureValidator::Validators_t::valid_boolean, JSON_BOOLEAN},
    {ProcedureValidator::Validators_t::valid_integer, JSON_INTEGER},
    {ProcedureValidator::Validators_t::valid_real, JSON_REAL},
    {ProcedureValidator::Validators_t::valid_object, JSON_OBJECT},

    {ProcedureValidator::Validators_t::valid_array, JSON_ARRAY},
    {ProcedureValidator::Validators_t::valid_array_of, JSON_ARRAY},
    {ProcedureValidator::Validators_t::valid_array_size_of, JSON_ARRAY},

    {ProcedureValidator::Validators_t::valid_enum, JSON_STRING},
    {ProcedureValidator::Validators_t::valid_uuid, JSON_STRING},

    {ProcedureValidator::Validators_t::valid_attribute, JSON_OBJECT},

    {ProcedureValidator::Validators_t::valid_never,
        static_cast<jsontype_t>(ProcedureValidator::Validators_t::valid_any)}
};

ProcedureValidator::ProcedureValidator(
        const string &name,
        parameterDeclaration_t paramType,
        jsontype_t returnType,
        ...):
    Procedure(name, paramType, returnType, nullptr)
{
    va_list parameters;
    va_start(parameters, returnType);
    create_validators(parameters);
    va_end(parameters);
}

ProcedureValidator::ProcedureValidator(
        const string &name,
        parameterDeclaration_t paramType,
        ...):
    Procedure(name, paramType, nullptr)
{
    va_list parameters;
    va_start(parameters, paramType);
    create_validators(parameters);
    va_end(parameters);
}

ProcedureValidator::ProcedureValidator(const Procedure &procedure):
    Procedure(procedure)
{
    /* nothing added here, validation done only via JSON validator */
}

void ProcedureValidator::create_validators(va_list args) {
    const char* name;
    while ((name = va_arg(args, const char*)) != nullptr) {
        unsigned type = va_arg(args, unsigned);

        // add validator to jsonrpc if "exact" form. Optionals and nulls
        // aren't checked here!
        JsonrpcValidators_t::const_iterator jit =
                JSONRPC_VALIDATORS.find(static_cast<Validators_t>(type));
        if (jit != JSONRPC_VALIDATORS.cend()) {
            /* Procedure:: */ AddParameter(name, (*jit).second);
        }

        // Check if field was already added, in this case "merge" validators
        // with "and" condition.
        bool merged = false;
        for (auto& v : validators) {
            if (v.name == name) {
                ValidityCheckerPtr first;
                first.swap(v.validator);
                ValidityCheckerPtr second = create_validator(type, args);
                ValidityCheckerPtr replacement(
                        new CompositeValidityChecker(first, second));
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

ValidityCheckerPtr ProcedureValidator::create_validator(unsigned type, va_list args) {
    if ((type & (VALID_OPTIONAL(0))) == (VALID_OPTIONAL(0))) {
        ValidityCheckerPtr cond =
            create_validator(type & (~(VALID_OPTIONAL(0))), args);
        return ValidityCheckerPtr(new OptionalValidityChecker(cond));
    }
    if ((type & (VALID_NULLABLE(0))) == (VALID_NULLABLE(0))) {
        ValidityCheckerPtr cond =
            create_validator(type & (~(VALID_NULLABLE(0))), args);
        return ValidityCheckerPtr(new NullAllowedValidityChecker(cond));
    }

    switch (static_cast<Validators_t>(type)) {
        case Validators_t::valid_never:
            return ValidityCheckerPtr(new AlwaysFailValidityChecker("Value never valid"));
        case Validators_t::valid_always:
            return ValidityCheckerPtr(nullptr);

            // Plain jsonrpc checks.
        // checking might be run twice: field is checked by jsonrpc and here if
        // field is obligatory
        case Validators_t::valid_any:
            return ValidityCheckerPtr(nullptr);

        case Validators_t::valid_string:
        case Validators_t::valid_boolean:
        case Validators_t::valid_integer:
        case Validators_t::valid_real:
        case Validators_t::valid_object:
        case Validators_t::valid_array:
            return ValidityCheckerPtr(new JsonrpcValidityChecker(static_cast<jsontype_t>(type)));

        case Validators_t::valid_array_of: {
            type = va_arg(args, unsigned);
            ValidityCheckerPtr cond = create_validator(type, args);
            return ValidityCheckerPtr(new ArrayValidityChecker(cond));
        }
        case Validators_t::valid_array_size_of: {
            unsigned min = va_arg(args, unsigned);
            unsigned max = va_arg(args, unsigned);
            type = va_arg(args, unsigned);
            ValidityCheckerPtr cond = create_validator(type, args);
            return ValidityCheckerPtr(new ArrayValidityChecker(cond, min, max));
        }

        case Validators_t::valid_uuid:
            return ValidityCheckerPtr(new UuidValidityChecker());

        // cannot validate the field, validator not yet created
        case Validators_t::valid_numeric:
            return ValidityCheckerPtr(new NumberValidityChecker());
        case Validators_t::valid_numeric_typed:
            return ValidityCheckerPtr(new NumberValidityChecker(args));

        case Validators_t::valid_enum:
            return ValidityCheckerPtr(new EnumValidityChecker(args));

        case Validators_t::valid_attribute:
            return ValidityCheckerPtr(new AttributeValidityChecker(args));

        default:
            assert(FAIL("Handled validator type"));
            return ValidityCheckerPtr(new AlwaysFailValidityChecker("Unhandled validator type"));
    }

}

unsigned ProcedureValidator::remove_from(
        Json::Value::Members& members,
        const std::string& name)
{
    unsigned entries = 0;
    Json::Value::Members::iterator it = members.begin();
    while (it != members.end()) {
        if (name == (*it)) {
            it = members.erase(it);
            entries++;
        } else {
            it++;
        }
    }
    return entries;
}

void ProcedureValidator::validate(const Json::Value& req) const {
    if (req.isNull()) {
        if (validators.empty()) {
            // nothing to validate
            return;
        }
    } else if (!req.isObject()) {
        INVALID("Not a JSON object");
    }

    Json::Value::Members members = req.getMemberNames();
    for (auto const& v : validators) {
        unsigned removed = remove_from(members, v.name);

        try {
            switch (removed) {
                case 0:
                    if (v.validator) {
                        v.validator->validate(NON_EXISTING_VALUE);
                    }
                    break;
                case 1:
                    if (v.validator) {
                        v.validator->validate(req[v.name]);
                    }
                    break;
                default:
                    // JSONCPP doesn't handle duplicated values properly (the last one
                    // overrides previous ones). So checking if duplicated is not available
                    // THIS IS NEVER EXECUTED WITH CURRENT JSONCPP!!!!
                    INVALID("Duplicated field in json");
                    break;
            }
        } catch (InvalidField fe) {
            // rethrow exception with field
            fe.append(v.name);
            throw fe;
        } catch (InvalidParameters pe) {
            // throw invalid field
            throw InvalidField(pe.get_message(), v.name);
        }
    }
    if (!members.empty()) {
        THROW(InvalidField, "rest", "Unexpected field in json", members.front());
    }

    // if no exception is thrown: json document is valid
}

} // jsonrpc namespace
