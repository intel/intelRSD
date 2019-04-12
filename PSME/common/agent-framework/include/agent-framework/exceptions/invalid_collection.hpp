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
 *
 * @file invalid_collection.hpp
 * @brief Declaration of Invalid Collection error
 * */

#pragma once



#include "agent-framework/exceptions/invalid_value.hpp"



namespace agent_framework {
namespace exceptions {

/*! @brief Invalid collection exception class */
class InvalidCollection final : public InvalidValue {
public:

    /*!
     * @brief Constructor
     * @param[in] msg Error message
     * */
    explicit InvalidCollection(const std::string& msg);


    InvalidCollection(const InvalidCollection&) = default;


    InvalidCollection& operator=(const InvalidCollection&) = default;


    InvalidCollection(InvalidCollection&&) = default;


    InvalidCollection& operator=(InvalidCollection&&) = default;


    /*! @brief Destructor */
    ~InvalidCollection();
};

}
}
