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
 * @file ipmb/command/command.hpp
 * @brief IPMI command.
 * */

#pragma once
#include <ipmb/ipmi_message.hpp>
#include <memory>

/*! Agnet namespace */
namespace agent {
/*! CHassis namespace */
namespace chassis {
/*! IPMB namespace */
namespace ipmb {
/*! Command namespace */
namespace command {

/*! IPMI Commnad interface*/
class Command {
public:
    virtual ~Command();

    /*!
     * Unpacks IPMI Message.
     * @param msg IPMI Message to unpack
     * */
    virtual void unpack(IpmiMessage& msg) = 0;

    /*!
     * Packs IPMI Message.
     * @param msg IPMI Message to pack
     * */
    virtual void pack(IpmiMessage& msg) = 0;

protected:

    /*! Base class for IPMI request */
    class Request {};

    /*! Base class for IPMI response */
    class Response {
        uint16_t m_len{};
    protected:
        CompletionCode m_cc{};
    public:
        /*!
         * Sets resposne lenght
         * @param len Response lenght
         * */
        void set_len(uint16_t len) {
            m_len = len;
        }

        /*!
         * Gets resposne lenght
         * @return Response lenght
         * */
        uint16_t get_len() {
            return m_len;
        }

        /*!
         * Sets completion code
         * @param cc Completion code to set
         * */
        void set_cc(const CompletionCode cc) {
            m_cc = cc;
        }

        /*!
         * Gets completion code
         * @return Completion code
         * */
        CompletionCode get_cc() const {
            return m_cc;
        }

        /*!
         * Extends Response lenght
         * @param msg IPMI message.
         * */
        virtual void add_data(IpmiMessage& msg) = 0;

        virtual ~Response();
    };

};

using CommandUniquePtr = std::unique_ptr<Command>;
using CommandSharedPtr = std::shared_ptr<Command>;

}
}
}
}

