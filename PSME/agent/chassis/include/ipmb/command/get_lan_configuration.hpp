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
 * @file ipmb/command/get_device_id.hpp
 * @brief GetLanConfiguration IPMI command.
 * */

#pragma once
#include <ipmb/command/command.hpp>
#include <ipmb/command/get-lan-selector/invalid_selector.hpp>
#include <map>

/*! Agent namesapce */
namespace agent {
/*! Chassis namesapce */
namespace chassis {
/*! IPMB namesapce */
namespace ipmb {
/*! Command namesapce */
namespace command {

/*! GetLanConfiguration command */
class GetLanConfiguration final : public Command {
public:
    enum Selector : std::uint8_t {
        IP = 3,
        SOURCE = 4,
        MASK = 6
    };
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

    /*! Copy constructor */
    GetLanConfiguration(const GetLanConfiguration&) = default;
    /*! Assigment construcor */
    GetLanConfiguration& operator=(const GetLanConfiguration&) = default;

    explicit GetLanConfiguration();
    ~GetLanConfiguration();
private:
    static const constexpr uint8_t OFFSET_SELECTOR = 1;

    void process_selector(const IpmiMessage& msg);

    using SelectorMap = std::map<std::uint8_t, CommandSharedPtr>;

    CommandSharedPtr m_current{nullptr};
    CommandSharedPtr m_invalid_selector{new InvalidSelector};
    SelectorMap m_selectors;
};

}
}
}
}

