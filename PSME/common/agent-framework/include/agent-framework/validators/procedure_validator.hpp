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


#ifndef PSME_FW_BUILD_PROCEDUREVALIDATOR_H
#define PSME_FW_BUILD_PROCEDUREVALIDATOR_H

#include <jsonrpccpp/common/procedure.h>
// for unique_ptrs
#include <memory>

/* "decorators", they are "chained" with other validators in the simplest manner
 * WARNING 'v' cannot be considered expression, another values might be appended!
 * WARNING No (arythmetic/logic) other than these operators are allowed to be used!
 */
#define VALID_OPTIONAL(v) 0x1000 | v
#define VALID_NULLABLE(v) 0x2000 | v

/* definitions of validators */
#define VALID_VAL(v) static_cast<unsigned>(jsonrpc::ProcedureValidator::Validators_t::valid_##v)
#define VALID_JSON_ANY VALID_VAL(any)

/* general true/false validators */
#define VALID_ALWAYS VALID_VAL(always)
#define VALID_NEVER VALID_VAL(never)

/* jsonrpc::JSON_xxx enum values are allowed to be put directly as well, but
 * cannot be used in more sophisticated conditions (like optional). VALID_JSON
 * have to be used instead (value cast is done properly)
 */
#define VALID_JSON(v) static_cast<unsigned>(jsonrpc::JSON_##v)
#define VALID_JSON_ANY VALID_VAL(any)
#define VALID_JSON_STRING VALID_JSON(STRING)
#define VALID_JSON_BOOLEAN VALID_JSON(BOOLEAN)
#define VALID_JSON_INTEGER VALID_JSON(INTEGER)
#define VALID_JSON_REAL VALID_JSON(REAL)
#define VALID_JSON_OBJECT VALID_JSON(OBJECT)

#define VALID_JSON_ARRAY VALID_JSON(ARRAY)
#define VALID_ARRAY_OF(v) VALID_VAL(array_of), v
#define VALID_ARRAY_SIZE(min, max) VALID_VAL(array_size_of), min, max, VALID_JSON_ANY
#define VALID_ARRAY_SIZE_OF(v, min, max) VALID_VAL(array_size_of), min, max, v

#define VALID_NUMERIC VALID_VAL(numeric)
#define VALID_NUMERIC_TYPE(type, what) \
    (static_cast<unsigned>(jsonrpc::ProcedureValidator::NumberValidityChecker_t::TYPE_##type) |\
     static_cast<unsigned>(jsonrpc::ProcedureValidator::NumberValidityChecker_t::CONSTR_##what))
#define VALID_NUMERIC_TYPED(type) VALID_VAL(numeric_typed), VALID_NUMERIC_TYPE(type, NOTHING)
#define VALID_NUMERIC_EQLT(type, max) VALID_VAL(numeric_typed), VALID_NUMERIC_TYPE(type, MAX), max
#define VALID_NUMERIC_EQGT(type, min) VALID_VAL(numeric_typed), VALID_NUMERIC_TYPE(type, MIN), min
#define VALID_NUMERIC_RANGE(type, min, max) VALID_VAL(numeric_typed), VALID_NUMERIC_TYPE(type, BOTH), min, max

#define VALID_ATTRIBUTE(o) VALID_VAL(attribute), o::get_procedure
#define VALID_ENUM(e) VALID_VAL(enum), e::is_allowable_value
#define VALID_UUID VALID_VAL(uuid)

namespace jsonrpc {

/*! @brief interface to validate value from JSON document (custom validator) */
class ValidityChecker {
public:
    virtual ~ValidityChecker();
    /*!
     * @brief validation method
     * @param value json value to be checked
     * @throws InvalidParameters exception if not valid
     */
    virtual void validate(const Json::Value& value) const
        noexcept(false);

protected:
    /*! @brief Default constructor, available only for subclasses */
    ValidityChecker() = default;

private:
    /*! @brief Copy constructor not allowed */
    ValidityChecker(const ValidityChecker&) = delete;
    ValidityChecker& operator=(const ValidityChecker&) = delete;
};

/*! @brief Type alias for custom validators */
using ValidityCheckerPtr = std::unique_ptr<ValidityChecker>;


/*!
 * @brief Replacer for JSONRPC validators
 * Procedure validator is executed on the JSON document received by JSONRPC.
 * The document must be valid according RFC and JSONRPC rough validation.
 * ProcedureValidator validates document in more detailed manner (eg. with range
 * checking) and hides JSONRPC implementation details a bit ("old fashioned"
 * procedure usage is allowed).
 * */
class ProcedureValidator : public Procedure {
public:
    /*!
     * @brief special value for fields not in the object, to distinguish from
     * plain nulls
     * */
    static const Json::Value NON_EXISTING_VALUE;

    /*! @brief List of all defined validator types handled by the class */
    enum class Validators_t : unsigned int {
        // Check if field exists, but do not check against the type
        valid_any = 0,

        // jsonrpc validators (as enumeration values) mith be passed directly
        // thus values must be the same. Backward compability.
        valid_string = JSON_STRING,
        valid_boolean = JSON_BOOLEAN,
        valid_integer = JSON_INTEGER,
        valid_real = JSON_REAL,
        valid_object = JSON_OBJECT,
        valid_array = JSON_ARRAY,

        valid_numeric, // any int/float number
        valid_numeric_typed, // any value between given two numbers


        valid_uuid, // must be valid UUID 8-4-4-4-12 hex digits
        valid_enum, // validate string if match the enumerated type
        valid_attribute, // validate object agains the type
        valid_array_of, // validate array if contains proper objects
        valid_array_size_of, // validate array if contains proper objects and size

        valid_always, // field is always valid
        valid_never // never valid
    };

    /*! @brief All numeric types */
    enum class NumberValidityChecker_t : unsigned int {
        // no checking at all
        TYPE_ANYTHING = 0,

        // primary valu types
        TYPE_INT32,
        TYPE_UINT32,
        TYPE_DOUBLE,

        // if given unknown type
        WRONG_TYPE     = 0xff,


        // constraint checks for numbers
        CONSTR_NOTHING = 0x1000,
        CONSTR_MIN     = 0x1100,
        CONSTR_MAX     = 0x1200,
        CONSTR_BOTH    = 0x1300
    };

    /*!
     * @brief Compability constructor from JSON Procedure.
     * No additional validation here (only "pre" validation in JSONRPC is done).
     * @deprecated To be used only from CommandJson.
     * When all commands are refactored, it should be removed!
     * */
    explicit ProcedureValidator(const Procedure &procedure);

    /*!
     * @brief Method validator
     * Variadic list contains all fields (by name) with validators. Mostly they
     * should be jsontype_t values. "Custom" validators are handled via macros.
     * Some validators have "own" additional values. These must be passed
     * as arguments to appropriate macro (named as validator), eg:
     * - VALID_NUMERIC
     * - VALID_NUMERIC_LT(DOUBLE, 23.1)
     * - VALID_STRING_LONGER(10)
     * - VALID_ENUM(PhysicalDriveType)
     * - VALID_OBJECT(FruInfo)
     * ...
     * The last parameter must be NULL.
     * @param name name to be registered
     * @param returntype something to be given, I duno what
     * @param ... tuples with name and validator type.
     * */
    ProcedureValidator(const std::string& name,
                       parameterDeclaration_t paramType,
                       jsontype_t returntype,
                       ...);

    /*!
     * @brief Notification validator
     * @see Method validator
     * */
    ProcedureValidator(const std::string& name,
                       parameterDeclaration_t paramType,
                       ...);

    /*!
     * @brief validate input JSON value (or whole document) if valid.
     * Pre-check is done just after document is parsed by JSONRPC server.
     * @param req JSON value (document) to be validated
     * @throw InvalidParameters if value (document) is not valid
     * */
    void validate(const Json::Value &val) const;

private:

    /*! @brief Dummy constructor, not to be used */
    ProcedureValidator() = delete;
    /*! @brief default copy constructor, not to be used */
    ProcedureValidator(const ProcedureValidator &) = delete;

    /*! @brief create all validators according var_args passed to the constructor */
    void create_validators(va_list args);

    /*!
     * @brief create single validator on given type
     * @param type value got from VALID_xxx macros to identify validators
     * @param args var_args to get necessary values
     * @return uniq pointer with created validator
     * */
    static ValidityCheckerPtr create_validator(unsigned type, va_list args);

    /*!
     * @brief Remove all members from fields list
     * @param fields list of not handled values
     * @param name to be removed
     * @return number of removed members
     * */
    static unsigned remove_from(Json::Value::Members& members, const std::string& name);


    /*! @brief Whole definition of value (document) schema */
    struct Validator_s {
        std::string name;
        ValidityCheckerPtr validator;
    };
    using ValidatorsMap_t = std::vector<Validator_s>;
    ValidatorsMap_t validators{};

    /*! @brief Mapping between "extended" validators and JSONRPC validators */
    using JsonrpcValidators_t = std::map<Validators_t, jsontype_t>;
    static const JsonrpcValidators_t JSONRPC_VALIDATORS;
};

}
#endif //PSME_FW_BUILD_PROCEDUREVALIDATOR_H
