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
 * @file server_error.hpp
 *
 * @brief Declaration of ServerError and ErrorDetail classes
 * */
#pragma once



#include "psme/rest/server/error/message_object.hpp"

#include "json-wrapper/json-wrapper.hpp"
#include <string>
#include <exception>



namespace psme {
namespace rest {
namespace error {

/*!
 * @brief Rest ServerError for extended error handling compatible with redfish spec.
 * */
class ServerError final {
public:

    /*! @brief Indicates that the request failed for an unknown internal error but that the service is still operational. */
    static const constexpr char INTERNAL_ERROR[] = "Base.1.0.InternalError";

    /*! @brief Indicates that the resource is in use. */
    static const constexpr char RESOURCE_IN_USE[] = "Base.1.0.ResourceInUse";

    /*! @brief Indicates that a general error has occurred. */
    static const constexpr char GENERAL_ERROR[] = "Base.1.0.GeneralError";

    /*! @brief Indicates that the request body was malformed JSON. Could be duplicate, syntax error, etc. */
    static const constexpr char MALFORMED_JSON[] = "Base.1.0.MalformedJSON";

    /*! @brief Indicates that a resource change or creation was attempted but that the operation cannot proceed because the resource already exists. */
    static const constexpr char RESOURCE_ALREADY_EXISTS[] = "Base.1.0.ResourceAlreadyExists";

    /*! @brief Indicates that the operation expected an image or other resource at the provided URI but none was found. */
    static const constexpr char RESOURCE_MISSING_AT_URI[] = "Base.1.0.ResourceMissingAtURI";

    /*! @brief Indicates that the attempt to access the resource/file/image at the URI was unauthorized. */
    static const constexpr char RESOURCE_AT_URI_UNAUTHORIZED[] = "Base.1.0.ResourceAtUriUnauthorized";

    /*! @brief Indicates that the credentials associated with the established session do not have sufficient privileges for the requested operation. */
    static const constexpr char INSUFFICIENT_PRIVILEGE[] = "Base.1.0.InsufficientPrivilege";

    /*! @brief Indicates the service is temporarily unavailable. */
    static const constexpr char SERVICE_TEMPORARILY_UNAVAILABLE[] = "Base.1.0.ServiceTemporarilyUnavailable";

    /*! @brief Indicates that a duplicate property was included in the request body. */
    static const constexpr char PROPERTY_DUPLICATE[] = "Base.1.0.PropertyDuplicate";

    /*! @brief Indicates that a property was given the wrong value type, such as when a number is supplied for a property that requires a string. */
    static const constexpr char PROPERTY_VALUE_TYPE_ERROR[] = "Base.1.0.PropertyValueTypeError";

    /*! @brief Indicates that an unknown property was included in the request body. */
    static const constexpr char PROPERTY_UNKNOWN[] = "Base.1.0.PropertyUnknown";

    /*! @brief Indicates that a required property was not supplied as part of the request. */
    static const constexpr char PROPERTY_MISSING[] = "Base.1.0.PropertyMissing";

    /*! @brief Indicates that a property was given the correct value type but the value of that property was not supported. This values not in an enumeration. */
    static const constexpr char PROPERTY_VALUE_NOT_IN_LIST[] = "Base.1.0.PropertyValueNotInList";

    /*! @brief Indicates that a property was given the correct value type but the value of that property was not supported. This includes value size/length exceeded. */
    static const constexpr char PROPERTY_VALUE_FORMAT_ERROR[] = "Base.1.0.PropertyValueFormatError";

    /*! @brief Indicates that a property cannot be modified even though the metadata specifies it as writable. */
    static const constexpr char PROPERTY_NOT_MODIFIABLE[] = "Intel_RackScale.1.0.0.PropertyNotModifiable";

    /*! @brief Indicates that the value given for a property is not within restrictions imposed by the Service (even though it may be correct according to metadata). */
    static const constexpr char PROPERTY_VALUE_RESTRICTED[] = "Intel_RackScale.1.0.0.PropertyValueRestricted";


    /*!
     * @brief Create ServerError object with given HTTP status code.
     * @param[in] http_status_code HTTP status code associated with the error.
     * */
    explicit ServerError(std::uint32_t http_status_code);


    /*! @brief Default copy constructor */
    ServerError(const ServerError&) = default;


    /*! @brief Assignment operator */
    ServerError& operator=(const ServerError&) = default;


    /*! @brief Destructor */
    ~ServerError();


    /*!
     * @brief Set HTTP status code.
     *
     * @param[in] http_status_code HTTP status code associated with the error.
     * */
    void set_http_status_code(std::uint32_t http_status_code);


    /*!
     * @brief Get HTTP status code.
     *
     * @return HTTP status code associated with the error.
     * */
    std::uint32_t get_http_status_code() const {
        return m_http_status_code;
    }


    /*!
     * @brief Set error code.
     *
     * @param[in] code String indicating a specific error or message
     * (not to be confused with the HTTP status code). This code can be used
     * to access a detailed message from message registry.
     * */
    void set_code(const std::string& code);


    /*!
     * @brief Error code getter.
     *
     * @return Error code.
     * */
    std::string get_code() const;


    /*!
     * @brief Set error message.
     *
     * @param[in] message A human readable error message indicating the semantics
     * associated with the error.
     * */
    void set_message(const std::string& message);


    /*!
     * @brief Error message getter.
     *
     * @return Error message.
     * */
    std::string get_message() const;


    /*!
     * @brief Add extended message.
     *
     * @param[in] info Additional human readable error message.
     * */
    void add_extended_message(const std::string& info);


    /*!
     * @brief Add extended message with message ID specified.
     *
     * @param[in] message_id String indicating a specific error or message.
     * @param[in] info Additional human readable error message.
     * */
    void add_extended_message(const std::string& message_id, const std::string& info);


    /*!
     * @brief Get extended messages in a Json format
     *
     * @return Json array containing all error messages.
     * */
    json::Json get_extended_messages() const;


    /*!
     * @brief Add extended message with message object.
     *
     * @param[in] object MessageObject with extended message info.
     * */
    void add_extended_message(const MessageObject& object);


    /*!
     * @brief Indicates whether or not there are any extended messages added to the object.
     *
     * @return True if there are any extended messages, false otherwise
     * */
    bool has_extended_messages() const;


    /*!
     * @brief Convert error JSON value to string
     *
     * @return JSON value as a string
     * */
    std::string as_string() const;


private:
    std::uint32_t m_http_status_code;
    json::Json m_error;
};

}
}
}


