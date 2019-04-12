/*!
 * @brief GetSledPresence IPMI command interface.
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
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
 * @file get_sled_presence.hpp
 */

#pragma once



#include <ipmb/command/command.hpp>

#include <vector>

/*! Agent namesapce */
namespace agent {
/*! Compute namesapce */
namespace compute {
/*! IPMB namesapce */
namespace ipmb {
/*! Command namesapce */
namespace command {

/*! GetSledPresence command */
class GetSledPresence final : public Command {
public:
    /*! Default constructor */
    GetSledPresence() {}


    /*! Copy constructor */
    GetSledPresence(const GetSledPresence&) = default;


    /*! Assigment construcor */
    GetSledPresence& operator=(const GetSledPresence&) = default;


    /*!
     * Unpacks IPMI Message
     * @param msg IPMI Message
     * */
    void unpack(IpmiMessage& msg);


    /*!
     * Packs IPMI Message
     * @param msg IPMI Message
     * */
    void pack(IpmiMessage& msg);


    /*!
     * @brief Returns sled presence
     * @return Sled presence
     */
    std::uint8_t get_sled_presence() const {
        return m_sled_presence;
    }

    /*!
     * @brief Returns True if stored value is correct
     * @return True if operation was successful
     */
    bool is_valid_value() const {
        return !m_invalid_value;
    }


private:
    static const constexpr uint8_t CMD_REQUEST_DATA_LENGTH = 1;
    static const constexpr uint8_t OFFSET_CC = 0;

    std::uint8_t m_sled_presence{};
    bool m_invalid_value{};
};

}
}
}
}
