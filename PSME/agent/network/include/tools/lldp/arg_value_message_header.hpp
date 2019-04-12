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
 * @file arg_value_message_header.hpp
 *
 * @brief LLDP argument value message header
 * */

#pragma once

#include "message_header.hpp"

namespace agent {
namespace network {
namespace api {
namespace lldp {

/*!
 * @brief LLDP argument value message header class
 * */
class ArgValueMessageHeader final : public MessageHeader {
public:
    /*! Default constructor */
    explicit ArgValueMessageHeader() = default;

    /*!
     * @brief Default constructor
     *
     * @param[in] arg Argument name
     * @param[in] value Argument value
     *
     * */
    explicit ArgValueMessageHeader(const std::string& arg,
                                    const std::string& value);

    /*!
     * @brief Parse raw message
     *
     * @param[in] raw Raw message
     *
     * */
    void parse_raw(const std::string& raw) override;

    /*!
     * @brief Get raw message
     *
     * @return Raw message
     *
     * */
    std::string get_raw() const override;

    /*!
     * @brief Get raw message size
     *
     * @return Raw message size
     *
     * */
    size_t get_raw_size() const override;

    /*!
     * @brief Get argument value
     *
     * @return Argument value
     *
     * */
    const std::string& get_value() const {
        return m_value;
    }

    /*!
     * @brief Get argument name
     *
     * @return Argument name
     *
     * */
    const std::string& get_name() const {
        return m_arg;
    }

private:
    std::string m_arg{};
    std::string m_value{};
};

}
}
}
}
