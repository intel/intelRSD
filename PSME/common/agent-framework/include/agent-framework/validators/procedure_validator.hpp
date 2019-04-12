/*!
 * @brief Extended validators for JSON values
 *
 * Set of classes to proper check JSON document in conjunction with JSONRPC
 * library. These introduces second stage of validation, where data is checked
 * according to the schema (provided by request class).
 *
 * @copyright Copyright (c) 2016-2019 Intel Corporation.
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
 * @file procedure_validator.hpp
 */

/*************************************************************************
 * libjson-rpc-cpp
 *************************************************************************
 * @file    specification.h
 * @date    30.04.2013
 * @author  Peter Spiess-Knafl <dev@spiessknafl.at>
 * @license See attached LICENSE.txt
 ************************************************************************/

/*************************************************************************
 * libjson-rpc-cpp
 *************************************************************************
 * @file    procedure.h
 * @date    31.12.2012
 * @author  Peter Spiess-Knafl <dev@spiessknafl.at>
 * @license See attached LICENSE.txt
 ************************************************************************/

#pragma once



#include "agent-framework/validators/checkers/validity_checker.hpp"

/*!
 * @defgroup valmacros JSON schema definitions
 * Set of macros to "hide" how validators "consume" values from the variable
 * args list (with syntax/semantics checking).
 * "Old" style jsonrpc::jsontype_t are allowed as well.
 * @warning No (arithmetic/logic) operators other than these macros are allowed!
 * @warning Checkers cannot be considered expression, macro expansion is used!
 * @see jsonrpc::ProcedureValidator
 * @{ @i{valmacros}
 */

/*!
 * @brief Optional validity checker.
 * @param v Checker to be applied if value is present.
 * @return valid if field is not present otherwise apply checker.
 */
#define VALID_OPTIONAL(v) \
    static_cast<unsigned>(0x1000) | v

/*!
 * @brief Validate value if is not null.
 * @param v Checker to be applied if value is not null
 * @return valid if field is null otherwise apply checker.
 * @warning 'v' cannot be considered expression, macro expansion is used here!
 */
#define VALID_NULLABLE(v) \
    static_cast<unsigned>(0x2000) | v


/*!
 * @defgroup valjsonrpc JSONRPC validators
 * jsonrpc::JSON_xxx enum values are allowed to be put directly as well, but
 * cannot be used in more sophisticated conditions. VALID_JSON_xxx have to be
 * used in such situations instead (value cast is done properly)
 * @{ @i{valjsonrpc}
 */

/*!
 * @brief JSONRPC value enumeration validity checker
 * @param t STRING, BOOLEAN, INTEGER, REAL, OBJECT, or ARRAY to select the type
 * @see jsonrpc::jsontype_t
 */
#define VALID_JSON(t) \
    static_cast<unsigned>(jsonrpc::JSON_##t)

#define VALID_JSON_ANY VALID_VAL(any)
#define VALID_JSON_STRING VALID_JSON(STRING)
#define VALID_JSON_BOOLEAN VALID_JSON(BOOLEAN)
#define VALID_JSON_INTEGER VALID_JSON(INTEGER)
#define VALID_JSON_REAL VALID_JSON(REAL)
#define VALID_JSON_OBJECT VALID_JSON(OBJECT)

/*! @} @i{valjsonrpc} */


/*!
 * @defgroup valcustom Custom validators
 * Macros to cover all semantics aspects of custom validity checkers
 * @{ @i{valcustom}
 */

/*!
 * @brief Validator_t enumeration declared values validity checker
 * @param t any of string, boolean, ..., uuid, enum to select the type
 * @see jsonrpc::ProcedureValidator::Validators_t
 */
#define VALID_VAL(t) \
    static_cast<unsigned>(jsonrpc::ProcedureValidator::Validators::valid_##t)

/*! Always pass validity checker. */
#define VALID_ALWAYS \
    VALID_VAL(always)

/*! @brief Always fail validity checker. */
#define VALID_NEVER \
    VALID_VAL(never)


/*!
 * @defgroup valarray Array validators
 * @{ @i{valarray}
 */

/*! @brief Array validator without checking */
#define VALID_JSON_ARRAY \
    VALID_JSON(ARRAY)

/*!
 * @brief Array validator with values checking
 * @param v validator (by the means of another VALID_xxx macros) to check all
 *          values in the array
 */
#define VALID_ARRAY_OF(v) \
    VALID_VAL(array_of), v

/*!
 * @brief Array validator with checking array size
 * @param min minimal array size
 * @param max maximal array size
 */
#define VALID_ARRAY_SIZE(min, max) \
    VALID_VAL(array_size_of), static_cast<unsigned>(min), static_cast<unsigned>(max), VALID_JSON_ANY

/*!
 * @brief Array validator with full checking.
 *
 * Both value type and array size are checked.
 *
 * @param v validator (by the means of another VALID_xxx macros) to check all
 *          values in the array
 * @param min minimal array size
 * @param max maximal array size
 */
#define VALID_ARRAY_SIZE_OF(v, min, max) \
    VALID_VAL(array_size_of), static_cast<unsigned>(min), static_cast<unsigned>(max), v
/*! @} @i{valarray} */


/*!
 * @defgroup valnumber Numbers validators
 * Any numeric values validity checkers. Either value might be (any) numeric
 * or the type might be specified. For values with specified type min/max values
 * might be specified.
 * @param type Type of the value, INT32, UINT32, and DOUBLE apply
 * @param what which additional arguments were specified; NOTHING, MIN, MAX and BOTH apply
 * @{ @i{valnumber}
 */

/*!
 * @brief Helper macro for other VALID_NUMERIC macros
 * @param type Type of the value, INT32, UINT32, and DOUBLE apply
 * @param what boundaries specification, NOTHING, MIN, MAX or BOTH apply
 */
#define VALID_NUMERIC_TYPE(type, what) \
    (static_cast<unsigned>(jsonrpc::ProcedureValidator::NumberType::TYPE_##type) |\
     static_cast<unsigned>(jsonrpc::ProcedureValidator::NumberRangeConstraint::CONSTR_##what))

/*! @brief Any number validator */
#define VALID_NUMERIC \
    VALID_VAL(numeric)

/*!
 * @brief Typed number validator
 * @param type Type of the value, currently only INT32, UINT32 and DOUBLE are handled
 */
#define VALID_NUMERIC_TYPED(type) \
    VALID_VAL(numeric_typed), VALID_NUMERIC_TYPE(type, NOTHING)

/*!
 * @brief Typed number validator with upper bound
 *
 * Checks if value is equal or less than given value.
 *
 * @param type Type of the value, currently only INT32, UINT32 and DOUBLE are handled
 * @param max Upper bound value
 */
#define VALID_NUMERIC_EQLT(type, max) \
    VALID_VAL(numeric_typed), VALID_NUMERIC_TYPE(type, MAX), static_cast<jsonrpc::type>(max)

/*!
 * @brief Typed number validator with lower bound
 *
 * Checks if value is equal or greater than given value.
 *
 * @param type Type of the value, currently only INT32, UINT32 and DOUBLE are handled
 * @param min Lower bound value
 */
#define VALID_NUMERIC_EQGT(type, min) \
    VALID_VAL(numeric_typed), VALID_NUMERIC_TYPE(type, MIN), static_cast<jsonrpc::type>(min)

/*!
 * @brief Typed number validator with full range checking
 *
 * Checks if value is in the bounds of min and max values inclusively.
 *
 * @param type Type of the value, currently only INT32, UINT32 and DOUBLE are handled
 * @param min Lower bound value
 * @param max Upper bound value
 */
#define VALID_NUMERIC_RANGE(type, min, max) \
    VALID_VAL(numeric_typed), VALID_NUMERIC_TYPE(type, BOTH), \
    static_cast<jsonrpc::type>(min), static_cast<jsonrpc::type>(max)

/*! @} @i{valnumber} */


/*!
 * @brief Attribute validity checker.
 *
 * All fields from the attribute are checked according to attribute schema.
 *
 * @param o Attribute type name, it must have schema defined
 * @see Attributes class
 */
#define VALID_ATTRIBUTE(o) \
    VALID_VAL(attribute), static_cast<jsonrpc::static_procedure_getter_t>(o::get_procedure)

/*!
 * @brief Regex validity checker.
 *
 * Checks if proper string and compares field value with provided regular expression.
 *
 * @param string_regex Regular expression, passed as const raw string (const char*)
 */
#define VALID_REGEX(string_regex) \
    VALID_VAL(regex), static_cast<const char*>(string_regex)

/*!
 * @brief Enumeration validity checker
 *
 * Checks if proper string, and if its value is listed in the enumeration
 *
 * @param e Enumeration type name
 * @see ENUM macro
 */
#define VALID_ENUM(e) \
    VALID_VAL(enum), static_cast<jsonrpc::is_allowable_value_t>(e::is_allowable_value), static_cast<jsonrpc:: get_values_t>(e::get_values)

/*!
 * @brief UUID validity checker
 *
 * UUID must be valid string following 8-4-4-4-12 with hex values.
 * More detailed description is in [RFC4122](https://tools.ietf.org/html/rfc4122)
 */
#define VALID_UUID \
    VALID_VAL(uuid)

/*! @} @i{valcustom} */
/*! @} @i{valmacros} */


namespace jsonrpc {

enum parameterDeclaration_t {
    PARAMS_BY_NAME,
    PARAMS_BY_POSITION
};

enum jsontype_t {
    JSON_STRING = 1,
    JSON_BOOLEAN = 2,
    JSON_INTEGER = 3,
    JSON_REAL = 4,
    JSON_OBJECT = 5,
    JSON_ARRAY = 6
};

/*!
 * @brief Replacer for JSONRPC validators.
 *
 * Procedure validator is executed on the JSON document received by JSONRPC.
 * The document must be valid according RFC and JSONRPC rough validation.
 * ProcedureValidator validates document in more detailed manner (eg. with range
 * checking) and hides JSONRPC implementation details a bit ("old fashioned"
 * procedure usage is allowed).
 * */
class ProcedureValidator {
    friend class ValidatorTester;

public:

    /*!
     * @brief All defined validity checkers
     *
     * This enumeration contains all handled data types. These are
     * "mapped" from appropriate macros VALID_xxx.
     * @see valmacros
     */
    enum class Validators : unsigned int {
        valid_any = 0, //!< value of any type, must exist..

        valid_string = JSON_STRING, //!< JSON string
        valid_boolean = JSON_BOOLEAN, //!< JSON boolean
        valid_integer = JSON_INTEGER, //!< JSON integer, no '.'/'e' are allowed
        valid_real = JSON_REAL, //!< JSON real value, pass only values with '.' or 'e'
        valid_object = JSON_OBJECT, //!< JSON object, no schema validation
        valid_array = JSON_ARRAY, //!< JSON array, no schema validation

        valid_numeric, //!< any number
        valid_numeric_typed, //!< (typed) number of requested type, it might have min/max

        valid_uuid, //!< UUID following 8-4-4-4-12 hex digits format
        valid_enum, //!< string matching the enumerated type
        valid_attribute, //!< object value with schema validation
        valid_regex, //!< string matching regular expression
        valid_array_of, //!< array value with schema validation
        valid_array_size_of, //!< validate array if contains proper values and has proper size

        valid_always, //!< always valid
        valid_never //!< never valid
    };

    /*! @brief Handled numeric types. */
    enum class NumberType : unsigned int {
        TYPE_ANYTHING = 0, //!< any numeric value (either integer or real).

        TYPE_INT32, //!< (signed) 32-bit integer
        TYPE_UINT32, //!< (unsigned) 32-bit integer
        TYPE_INT64, //!< (signed) 64-bit integer
        TYPE_UINT64, //!< (unsigned) 64-bit integer
        TYPE_DOUBLE, //!< real value

        WRONG_TYPE = 0xff //!< wrong value type, validation always fails with assert
    };

    /*! @brief Constraint checks for numbers. */
    enum class NumberRangeConstraint : unsigned int {
        CONSTR_NOTHING = 0x1000, //!< no range checking
        CONSTR_MIN = 0x1100, //!< min value is specified
        CONSTR_MAX = 0x1200, //!< max value is specified
        CONSTR_BOTH = 0x1300  //!< both min and max values are specified
    };

    /*!
     * @brief Method validator
     * @anchor methodValidator
     *
     * @param name Method name to be registered
     * @param paramType format of input document, only PARAMS_BY_NAME is allowed.
     * @param returnType what is reported after handling the method. The param
     *                   is only to distinguish method from notification (value
     *                   doesn't have any effect). Default value JSON_OBJECT is
     *                   to be used.
     * @param ... Variadic list containing schema declaration. Each field
     *            is the field name and validity checker which control JSON
     *            document values. After all declarations <b>nullptr</b> must
     *            be put.
     * @warning Field names must be passed as static strings (either symbols or statically
     *          allocated constants/variables). Dynamically allocated variables are not
     *          allowed, passing such values causes unpredictable results.
     *
     * The most advice is to use @ref valmacros "custom macros" for schema
     * declaration:
     * - VALID_NUMERIC
     * - VALID_NUMERIC_EQLT(DOUBLE, 23.1)
     * - VALID_OPTIONAL(VALID_NUMERIC_RANGE(INT32, 5, 20))
     * - VALID_ENUM(PhysicalDriveType)
     * - VALID_OBJECT(FruInfo)
     * ...
     */
    ProcedureValidator(const std::string& name,
                       parameterDeclaration_t paramType,
                       jsontype_t returnType,
                       ...);


    /*!
     * @brief Notification validator.
     *
     * @param name Notification name to be registered
     * @param paramType Format of input document, only PARAMS_BY_NAME is allowed.
     * @param ... Variadic list with schema declaration.
     *
     * See also @ref methodValidator
     * */
    ProcedureValidator(const std::string& name,
                       parameterDeclaration_t paramType,
                       ...);


    /*!
     * @brief Notification validator without explicit name.
     *
     * @warning This method must not be used to register procedure in a JSON RPC server.
     *
     * @param paramType Format of input document, only PARAMS_BY_NAME is allowed.
     * @param ... Variadic list with schema declaration.
     *
     * See also @ref methodValidator
     * */
    ProcedureValidator(parameterDeclaration_t paramType, ...);

    /*!
     * @brief Validate JSON-CPP value if valid.
     *
     * Validate JSON value (or whole document) if matches schema on which
     * validator is constructed.
     *
     * @warning Pre-check is done just after document is parsed by JSONRPC server,
     * thus invalid (according JSONRPC validation) documents are not validated here.
     *
     * @param req JSON value (document) to be validated
     * @throw InvalidParameters if value (document) is not valid
     * */
    void validate(const json::Json& req) const;

    /*!
     * @brief Gets the procedure name
     * @return Procedure name
     */
    const std::string& get_procedure_name() const {
        return m_procedure_name;
    }

private:
    /*!
     * @brief Default constructor.
     *
     * Not to be used.
     */
    ProcedureValidator() = delete;


    /*!
     * @brief Default copy constructor.
     *
     * Not to be used.
     */
    ProcedureValidator(const ProcedureValidator&) = delete;


    ProcedureValidator& operator=(const ProcedureValidator&) = delete;


    ProcedureValidator(ProcedureValidator&&) = delete;


    ProcedureValidator& operator=(ProcedureValidator&&) = delete;


    /*! @brief Create all validators according var_args passed to the constructor. */
    void create_validators(va_list& args);


    /*!
     * @brief create single validator on given type
     * @param type value got from VALID_xxx macros to identify validators
     * @param args var_args to get necessary values
     * @return unique pointer with created validator
     * */
    static ValidityChecker::Ptr create_validator(unsigned type, va_list& args);


    /*!
     * @brief Remove all members from fields list
     * @param[in,out] json list of not yet handled values
     * @param name to be removed
     * @return number of removed members
     * */
    static unsigned remove_from(json::Json& json, const std::string& name);


    /*! @brief Single JSON value schema declaration */
    struct ValidatorSchema {
        const char* name;
        ValidityChecker::Ptr validator;
    };

    /*! @brief Whole document (or value) schema declaration */
    using ValidatorsMap = std::vector<ValidatorSchema>;

    /*! @brief Whole document (or value) schema definition */
    ValidatorsMap validators{};


    /*! @brief "Extended" validators to JSONRPC mapping declaration */
    using JsonrpcValidators = std::map<Validators, jsontype_t>;

    /*! @brief Mapping between "extended" validators and JSONRPC validators */
    static const JsonrpcValidators JSONRPC_VALIDATORS;

    std::string m_procedure_name{};
};

}
