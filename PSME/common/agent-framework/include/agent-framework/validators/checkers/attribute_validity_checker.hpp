/*!
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
 * @file attribute_validity_checker.hpp
 * */
#pragma once

#include "validity_checker.hpp"

namespace jsonrpc {

// Forward declaration
class ProcedureValidator;

/*! @brief Attribute validity checker */
class AttributeValidityChecker final : public ValidityChecker {
public:
    AttributeValidityChecker(va_list& args);

    virtual ~AttributeValidityChecker();

    /*!
     * @brief Default copy constructor.
     *
     * Must be defined in the class with pointers.
     * It is public/default to avoid CLion warning, but should be private/deleted.
     */
    AttributeValidityChecker(const AttributeValidityChecker&) = default;

    virtual void validate(const json::Json& value) const;

private:
    /*! Default copy operator. Not to be used, must be defined in the class with pointers. */
    AttributeValidityChecker& operator=(const AttributeValidityChecker&) = delete;

    /*! Default constructor. Not to be used, must be defined in the class with pointers. */
    AttributeValidityChecker() = delete;

    /*!
     * @brief Procedure validator method from the *static* object
     *
     * Singletons are adviced to be used.
     */
    const ProcedureValidator* validator{nullptr};
};

}
