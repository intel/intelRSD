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
 * @file nl_exception_invalid_ifname.hpp
 *
 * @brief Netlink invalid interface name exception
 * */

#pragma once

#include "nl_exception.hpp"

namespace netlink_base {

/*! @brief Netlink message exception class */
class NlExceptionInvalidIfName : public NlException {
public:
    /*!
     * @brief Constructor
     *
     * @param[in] name Interface name
     * */
    explicit NlExceptionInvalidIfName(const std::string& name);

    /*! Copy constructors */
    NlExceptionInvalidIfName(const NlExceptionInvalidIfName&) = default;
    NlExceptionInvalidIfName(NlExceptionInvalidIfName&&) = default;

    /*! Assign constructors */
    NlExceptionInvalidIfName& operator=(const NlExceptionInvalidIfName&) = default;
    NlExceptionInvalidIfName& operator=(NlExceptionInvalidIfName&&) = default;

    /*! @brief Destructor */
    virtual ~NlExceptionInvalidIfName() final;
};

}
