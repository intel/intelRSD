/*!
 * @brief Opaepp wrapper error types declaration
 *
 * @copyright Copyright (c) 2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file opaepp_error.hpp
 */

#pragma once



#include <opae/types.h>
#include <string>
#include <stdexcept>
#include "agent-framework/module/enum/enum_builder.hpp"



namespace opaepp {

ENUM(OpaeApiFpgaResultCode, unsigned,
     FPGA_OK,
     FPGA_INVALID_PARAM,
     FPGA_BUSY,
     FPGA_EXCEPTION,
     FPGA_NOT_FOUND,
     FPGA_NO_MEMORY,
     FPGA_NOT_SUPPORTED,
     FPGA_NO_DRIVER,
     FPGA_NO_DAEMON,
     FPGA_NO_ACCESS,
     FPGA_RECONF_ERROR
);

/*!
 * @brief OPAE error base class declaration
 */
class OpaeppError : public std::exception {
public:
    /*!
     * @brief Constructor with error message as an argument
     * @param message text message of the OPAE error
     */
    OpaeppError(const std::string& message) : m_message(message) {
        make_what();
    }


    /*!
      * @brief Gets error message
      * @return message which this error type represents
      */
    virtual const char* what() const throw() {
        return m_what.c_str();
    }


    /*!
     * @brief Default destructor
     */
    virtual ~OpaeppError() {}


protected:
    std::string m_message{};
    std::string m_what{};

private:
    /*! Prepares m_what field returned on what() method call */
    void make_what() {
        m_what = "Opaepp error: " + m_message;
    }
};

/*!
 * @brief OPAE API error declaration
 */
class OpaeApiError : public OpaeppError {
public:
    /*!
     * @brief Constructor
     * @param result_code code returned from opae command
     * @param command_name name of the opae command
     */
    OpaeApiError(OpaeApiFpgaResultCode result_code, const std::string& command_name) : OpaeppError(command_name),
                                                                                       m_result_code(result_code) {
        make_what();
    }


    /*!
     * @brief Constructor
     * @param result_code code returned from opae command
     * @param command_name name of the opae command
     * @param extended_error_message additional error message
     */
    OpaeApiError(OpaeApiFpgaResultCode result_code, const std::string& command_name,
                 const std::string& extended_error_message) : OpaeppError(command_name), m_result_code(result_code),
                                                              m_extended_error_message(extended_error_message) {
        make_what();
    }


    /*!
     * @brief Default destructor
     */
    virtual ~OpaeApiError() = default;


    /*!
     * @brief Gets error message
     * @return message which this error type represents
     */
    virtual const char* what() const throw() {
        return m_what.c_str();
    }


    /*!
     * @brief Throws if given result code is not equal FPGA_OK
     * @param result code returned from opae command
     * @param command_name name of the opae command
     */
    static void throw_if_unexpected_result(fpga_result& result, const std::string& command_name) {
        if (result != FPGA_OK) {
            throw OpaeApiError(static_cast<OpaeApiFpgaResultCode::base_enum>(result), command_name);
        }
    }


    /*!
     * @brief Throws if given result code is not equal FPGA_OK
     * @param result code returned from opae command
     * @param command_name name of the opae command
     * @param extended_error_message additional error message
     */
    static void throw_if_unexpected_result(fpga_result& result, const std::string& command_name,
                                           const std::string& extended_error_message) {
        if (result != FPGA_OK) {
            throw OpaeApiError(static_cast<OpaeApiFpgaResultCode::base_enum>(result), command_name,
                               extended_error_message);
        }
    }


private:
    /*! Prepares m_what field returned on what() method call */
    void make_what() {
        m_what = "Opae API command " + m_message + " returned error code: " + m_result_code.to_string() + ". " +
                 m_extended_error_message;
    }


    OpaeApiFpgaResultCode m_result_code;
    std::string m_extended_error_message{};
};

}