/*!
 * @section LICENSE
 *
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
 * @section DESCRIPTION
 *
 * @file message.hpp
 *
 * @brief Message interface
 * */

#pragma once
#include <map>
#include <string>
#include <vector>
#include <functional>

namespace ipmi {

using std::map;
using std::pair;
using std::vector;
using std::string;
using std::function;

/*!
 * @brief Generic message
 * */
class Message {
public:
    /*!
     * @brief Concrete Message object will unpack given raw packed data payload
     * and store content to own variable members
     *
     * @param[in]  raw_payload  Raw packed data payload to unpack by concrete
     *                          Message object
     * */
    virtual void unpack(const vector<uint8_t>& raw_payload) = 0;

    /*!
     * @brief Pack all data members from concrete Message object in
     * one packed data payload
     *
     * @param[out]  raw_payload Get raw packed data payload from concrete
     *                          Message object
     * */
    virtual void pack(vector<uint8_t>& raw_payload) const = 0;

    /*!
     * @brief Message destructor
     * */
    virtual ~Message();
};

/*!
 * @brief Message factory that create messages to send/receive by various
 * transports classes
 * */
class MessageFactory {
public:
    /*! Create message function */
    typedef function<Message*()> create_message_t;

    /*! Create message functions pair for creating request/response object */
    typedef pair<create_message_t, create_message_t> create_message_pair_t;

    /*! Map create functions pair with unique key string value */
    typedef map<string, create_message_pair_t> create_message_map_t;
private:
    create_message_map_t m_create_message_map;
public:
    /*!
     * @brief Default MessageFactor constructor
     * */
    MessageFactory();

    /*!
     * @brief Register create functions pair for creating request/response
     * object
     *
     * @param[in]   message_name    Unique key string in map
     * @param[in]   pair            Pair of functions that create object.
     *                              First pair: request object
     *                              Second pair: response object
     * */
    void register_message(const string& message_name, const create_message_pair_t& pair);

    /*!
     * @brief Call function that creates request object
     *
     * @param[in]   message_type    Unique key string in map
     * */
    Message* create_request(const string& message_type);

    /*!
     * @brief Call function that creates response object
     *
     * @param[in]   message_type    Unique key string in map
     * */
    Message* create_response(const string& message_type);
};

}

