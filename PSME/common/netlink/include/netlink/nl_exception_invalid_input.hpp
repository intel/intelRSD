/*!
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
 * @file nl_exception_invalud_input.hpp
 *
 * @brief Netlink invalid input exception
 * */

#pragma once

#include "nl_exception.hpp"

namespace netlink_base {

/*! @brief Netlink message exception class */
class NlExceptionInvalidInput : public NlException {
public:
    /*!
     * @brief Constructor
     *
     * */
    explicit NlExceptionInvalidInput();

    /*! Copy constructors */
    NlExceptionInvalidInput(const NlExceptionInvalidInput&) = default;
    NlExceptionInvalidInput(NlExceptionInvalidInput&&) = default;

    /*! Assign constructors */
    NlExceptionInvalidInput& operator=(const NlExceptionInvalidInput&) = default;
    NlExceptionInvalidInput& operator=(NlExceptionInvalidInput&&) = default;

    /*! @brief Destructor */
    virtual ~NlExceptionInvalidInput() final;
};

}
