/*!
 * @brief KeyValueMissingError error class interface.
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
 * @file key_value_missing_error.hpp
 */


#pragma once



#include <stdexcept>



namespace agent_framework {

/*!
 * @brief KeyValueMissingError error class.
 *
 * Instances of this class should be thrown if for some reason a unique key
 * cannot be generated (i.e. there is not enough data known).
 * */
class KeyValueMissingError : public std::runtime_error {
public:
    KeyValueMissingError(const std::string& what_arg) : std::runtime_error(what_arg) {}


    KeyValueMissingError(const KeyValueMissingError&) = default;


    KeyValueMissingError(KeyValueMissingError&&) = default;


    virtual ~KeyValueMissingError();
};

}
