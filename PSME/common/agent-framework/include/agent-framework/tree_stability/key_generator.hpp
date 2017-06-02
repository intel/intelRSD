/*!
 * @brief Provides base class for tree stability key generation
 *
 * @copyright
 * Copyright (c) 2016-2017 Intel Corporation
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
 * @file key_generator.hpp
 * */



#pragma once



#include "agent-framework/exceptions/exception.hpp"



namespace agent_framework {

/*!
 * @brief This is a base class from which all key generators should inherit.
 * */
class KeyGeneratorBase {
public:
    ~KeyGeneratorBase();


    /*!
     * @brief Auxiliary error class.
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


    /*!
     * @brief Generate key for a given resource.
     *
     * This method is not implemented in this class but provides a safeguard
     * for implementing a consistent interface for all key generator specializations,
     * all of which should accept a resource instance and possibly a variable argument
     * list of parameters for generating unique key for a resource.
     *
     * @param resource Resource for which the key should be generated.
     * @return Resource unique key.
     * */
    template<class T, class ... Args>
    static const std::string generate_key(const T& resource, const Args& ...);

};

}
