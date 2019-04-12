/*!
* @brief PLDM exceptions declaration
*
* @copyright Copyright (c) 2019 Intel Corporation.
*
* Licensed under the Apache License, Version 2.0 (the "License") override;
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License
*
* @file pldm_error.hpp
*/

#pragma once



#include "protocol/pldm/command/pldm_command.hpp"
#include <exception>
#include <stdexcept>
#include <string>



/*! Agent namespace */
namespace agent {
/*! PNC namespace */
namespace pnc {
/*! PLDM namespace */
namespace pldm {

/*!
 * @brief General PLDM exception type that will be thrown when error condition is
 * discovered while processing PldmCommand.
 */
class CommandError : public std::runtime_error {
public:
    /*!
     * @brief Constructs exception object
     * @param[in] command PLDM command object
     * @param[in] what_arg Describes what happened
     * @param[in] completion_code PLDM completion code
     */
    CommandError(const command::PldmCommand& command,
                 const std::string& what_arg,
                 PldmCompletionCode completion_code);


    CommandError(const CommandError&) = default;


    CommandError(CommandError&&) = default;


    CommandError& operator=(const CommandError&) = default;


    virtual ~CommandError();


    /*!
     * @brief Completion code getter
     * @return Completion code.
     */
    PldmCompletionCode get_completion_code() const {
        return m_completion_code;
    }


    /*!
     * @brief PLDM type getter
     * @return PLDM type.
     */
    pldm::PldmType get_pldm_type() const {
        return m_pldm_type;
    }


    /*!
     * @brief PLDM command getter
     * @return PLDM command.
     */
    pldm::PldmCommand get_pldm_command() const {
        return m_pldm_command;
    }


private:
    PldmCompletionCode m_completion_code;
    pldm::PldmType m_pldm_type;
    pldm::PldmCommand m_pldm_command;
};

/*!
 * @brief PLDM exception type that will be thrown when invalid PLDM message size is encountered.
 */
class InvalidPldmMessageSize : public std::runtime_error {
public:
    /*!
     * @brief Constructs exception object
     * @param[in] what_arg Describes what happened
     * @param[in] expected_bytes number of bytes of PLDM message expected
     * @param[in] received_bytes number of bytes of PLDM message received
     */
    InvalidPldmMessageSize(const std::string& what_arg, std::size_t expected_bytes, std::size_t received_bytes);


    /*!
     * @brief Constructs exception object
     * @param[in] what_arg Describes what happened
     */
    InvalidPldmMessageSize(const std::string& what_arg);


    InvalidPldmMessageSize(const InvalidPldmMessageSize&) = default;


    InvalidPldmMessageSize& operator=(const InvalidPldmMessageSize&) = default;


    InvalidPldmMessageSize(InvalidPldmMessageSize&&) = default;


    virtual ~InvalidPldmMessageSize();
};

}
}
}