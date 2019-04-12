/*!
 * @brief Response object from IPMI.
 *
 * @copyright Copyright (c) 2015-2019 Intel Corporation.
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
 * @file ipmi/response.hpp
 */

#pragma once



#include "message.hpp"
#include "response_error.hpp"

#include <cstdint>
#include <vector>
#include <string>



namespace ipmi {

class Response : public Message {
public:
    /*!
     * @brief Constructor.
     * @param fn Network function.
     * @param cmd Command.
     * @param minimal_response_length Minimal response length required for proper unpack.
     */
    Response(IpmiInterface::NetFn fn, IpmiInterface::Cmd cmd, std::size_t minimal_response_length);


    /*!
     * @brief destructor.
     */
    virtual ~Response();

    /*! Copy constructor */
    Response(const Response&) = default;

    /*! Move constructor */
    Response(Response&&) = default;

    /*! Assignment operator */
    Response& operator=(const Response&) = default;

    /*! Rvalue assignment operator */
    Response& operator=(Response&&) = default;

    /*!
     * @brief Template method processing response data.
     * @throw ResponseError if validation/unpack fails
     */
    void do_unpack(const IpmiInterface::ByteBuffer& data);


protected:
    /*!
     * @brief offset in response ByteBuffer where completion code is stored.
     */
    static constexpr const std::size_t OFFSET_COMPLETION_CODE = 0;


    /*!
     * @brief offset in response ByteBuffer where data begins.
     */
    static constexpr const std::size_t OFFSET_DATA = 1;


    /*!
     * @brief Throws error derived from ResponseError for command specific non-zero completion codes.
     * @param completion_code completion code (non-zero)
     * @throws ResponseError type exception.
     */
    [[noreturn]] virtual void throw_error_on_completion_code(CompletionCode completion_code) const;

private:
    /*
     * minimal response length required to be correctly parsed (unpacked).
     */
    std::size_t minimal_response_length;


    /*!
     * Unpacks raw data bytes from ByteBuffer into Response object.
     *
     * @param data which will be unpacked into object.
     */
    virtual void unpack(const IpmiInterface::ByteBuffer& data) = 0;


    void validate_completion_code(const IpmiInterface::ByteBuffer& data) const;


    void validate_response_size(const IpmiInterface::ByteBuffer& data) const;
};

}
