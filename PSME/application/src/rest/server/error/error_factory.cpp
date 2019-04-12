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
 * */
#include "psme/rest/server/error/error_factory.hpp"

#include "psme/rest/server/error/message_object.hpp"
#include "psme/rest/server/status.hpp"
#include "psme/rest/endpoints/utils.hpp"
#include "logger/logger.hpp"

#include <stdarg.h>
#include <cstdio>



using namespace agent_framework::exceptions;
using namespace psme::rest::error;
using namespace psme::rest::server::status_4XX;
using namespace psme::rest::server::status_5XX;

namespace {

/* List of messages and resolutions for Redfish-defined error objects */
static const constexpr char GENERAL_ERROR_MESSAGE[] = "A general error has occurred. See ExtendedInfo for more information.";

static const constexpr char INTERNAL_ERROR_MESSAGE[] = "The request failed due to an internal service error. The service is still operational.";
static const constexpr char INTERNAL_ERROR_RESOLUTION[] = "Resubmit the request. If the problem persists, consider resetting the service.";

static const constexpr char RESOURCE_IN_USE_MESSAGE[] = "The change to the requested resource failed because the resource is in use or in transition.";
static const constexpr char RESOURCE_IN_USE_RESOLUTION[] = "Remove the condition and resubmit the request if the operation failed.";

static const constexpr char RESOURCE_ALREADY_EXISTS_MESSAGE[] = "The requested resource already exists.";
static const constexpr char RESOURCE_ALREADY_EXISTS_RESOLUTION[] = "Do not repeat the create operation as the resource has already been created.";

static const constexpr char RESOURCE_MISSING_AT_URI_MESSAGE[] = "The resource at the URI %s was not found.";
static const constexpr char RESOURCE_MISSING_AT_URI_RESOLUTION[] = "Place a valid resource at the URI or correct the URI and resubmit the request.";

static const constexpr char MALFORMED_JSON_MESSAGE[] = "The request body submitted was malformed JSON and could not be parsed by the receiving service.";
static const constexpr char MALFORMED_JSON_RESOLUTION[] = "Ensure that the request body is valid JSON and resubmit the request.";

static const constexpr char UNAUTHORIZED_MESSAGE[] = "While accessing the resource at %s, the service received an authorization error.";
static const constexpr char UNAUTHORIZED_RESOLUTION[] = "Ensure that the appropriate access is provided for the service in order for it to access the URI.";

static const constexpr char INSUFFICIENT_PRIVILEGE_MESSAGE[] = "There are insufficient privileges for the account or credentials associated with the current session to perform the requested operation.";
static const constexpr char INSUFFICIENT_PRIVILEGE_RESOLUTION[] = "Either abandon the operation or change the associated access rights and resubmit the request if the operation failed.";

static const constexpr char PROPERTY_DUPLICATE_MESSAGE[] = "The property %s was duplicated in the request.";
static const constexpr char PROPERTY_DUPLICATE_RESOLUTION[] = "Remove the duplicate property from the request body and resubmit the request if the operation failed.";

static const constexpr char PROPERTY_UNKNOWN_MESSAGE[] = "The property %s is not in the list of valid properties for the resource.";
static const constexpr char PROPERTY_UNKNOWN_RESOLUTION[] = "Remove the unknown property from the request body and resubmit the request if the operation failed.";

static const constexpr char PROPERTY_MISSING_MESSAGE[] = "The property %s is a required property and must be included in the request.";
static const constexpr char PROPERTY_MISSING_RESOLUTION[] = "Ensure that the property is in the request body and has a valid value and resubmit the request if the operation failed.";

static const constexpr char PROPERTY_VALUE_TYPE_ERROR_MESSAGE[] = "The value %s for the property %s is a different type than the property can accept.";
static const constexpr char PROPERTY_VALUE_TYPE_ERROR_RESOLUTION[] = "Correct the value for the property in the request body and resubmit the request if the operation failed.";

static const constexpr char PROPERTY_VALUE_NOT_IN_LIST_MESSAGE[] = "The value %s for the property %s is not in the list of acceptable values.";
static const constexpr char PROPERTY_VALUE_NOT_IN_LIST_RESOLUTION[] = "Choose a value from the enumeration list that the implementation can support and resubmit the request if the operation failed.";

static const constexpr char PROPERTY_VALUE_FORMAT_ERROR_MESSAGE[] = "The value %s for the property %s is a different format than the property can accept.";
static const constexpr char PROPERTY_VALUE_FORMAT_ERROR_RESOLUTION[] = "Correct the value for the property in the request body and resubmit the request if the operation failed.";

static const constexpr char SERVICE_TEMPORARILY_UNAVAILABLE_MESSAGE[] = "The service is temporarily unavailable. Retry in %d seconds.";
static const constexpr char SERVICE_TEMPORARILY_UNAVAILABLE_RESOLUTION[] = "Wait for the indicated retry duration and retry the operation.";

/* List of messages and resolutions for Intel RackScale-defined error objects */
static const constexpr char PROPERTY_NOT_MODIFIABLE_MESSAGE[] = "The service is unable to modify the property %s even though metadata specifies it as writeable.";
static const constexpr char PROPERTY_NOT_MODIFIABLE_RESOLUTION[] = "Remove the unmodifiable property from the request body and resubmit the request.";

static const constexpr char PROPERTY_VALUE_RESTRICTED_MESSAGE[] = "The value %s for property %s is not within restrictions imposed by the Service.";
static const constexpr char PROPERTY_VALUE_RESTRICTED_RESOLUTION[] = "Correct the value for the property in the request body and resubmit the request.";


const MessageObject create_message_object(const std::string& redfish_code, const std::string& message,
                                          Severity severity, const std::string& resolution = {},
                                          const std::vector<std::string>& related_properties = {}) {
    auto message_object = MessageObject{redfish_code, message};
    message_object.set_severity(severity);
    message_object.set_resolution(resolution);
    message_object.set_related_properties(related_properties);
    return message_object;
}


const MessageObject create_message_object_from_gami_code(ErrorCode gami_error_code,
                                                         const std::string& gami_message,
                                                         const json::Json& data = {}) {

    std::string redfish_code{};
    std::string message{gami_message};
    std::string resolution{};
    std::string property_name{};
    std::vector<std::string> related_properties{};
    Severity severity{Severity::OK};

    switch (gami_error_code) {
        case ErrorCode::INVALID_UUID:
            redfish_code = ServerError::INTERNAL_ERROR;
            message = "Could not find one of the requested resources.";
            severity = Severity::Critical;
            resolution = ::INTERNAL_ERROR_RESOLUTION;
            break;

        case ErrorCode::INVALID_PARAMETER:
        case ErrorCode::INVALID_COLLECTION:
            redfish_code = ServerError::INTERNAL_ERROR;
            severity = Severity::Critical;
            break;

        case ErrorCode::COMPUTE:
        case ErrorCode::IPMI:
        case ErrorCode::NETWORK:
        case ErrorCode::FM10000:
        case ErrorCode::STORAGE:
        case ErrorCode::ISCSI:
        case ErrorCode::LVM:
        case ErrorCode::NVME:
        case ErrorCode::SPDK:
        case ErrorCode::PCIE_FABRIC:
        case ErrorCode::CHASSIS:
        case ErrorCode::FPGAOF:
        case ErrorCode::CERTIFICATE:
            redfish_code = ServerError::INTERNAL_ERROR;
            severity = Severity::Critical;
            resolution = ::INTERNAL_ERROR_RESOLUTION;
            break;

        case ErrorCode::UNKNOWN_ERROR:
        default:
            redfish_code = ServerError::GENERAL_ERROR;
            severity = Severity::Critical;
            break;

        case ErrorCode::METHOD_CONFLICT:
        case ErrorCode::METHOD_NOT_FOUND:
        case ErrorCode::NOT_IMPLEMENTED:
            redfish_code = ServerError::GENERAL_ERROR;
            severity = Severity::Critical;
            break;

        case ErrorCode::METHOD_NOT_ALLOWED:
            redfish_code = ServerError::GENERAL_ERROR;
            severity = Severity::Critical;
            break;

        case ErrorCode::INVALID_FIELD:
        case ErrorCode::UNSUPPORTED_FIELD:
            redfish_code = ServerError::GENERAL_ERROR;
            property_name = InvalidField::get_field_name_from_json_data(data);
            severity = Severity::Critical;
            break;

        case ErrorCode::INVALID_FIELD_TYPE:
            redfish_code = ServerError::PROPERTY_VALUE_TYPE_ERROR;
            property_name = InvalidField::get_field_name_from_json_data(data);
            severity = Severity::Warning;
            resolution = ::PROPERTY_VALUE_TYPE_ERROR_RESOLUTION;
            break;

        case ErrorCode::DUPLICATED_FIELD:
            redfish_code = ServerError::PROPERTY_DUPLICATE;
            property_name = InvalidField::get_field_name_from_json_data(data);
            severity = Severity::Warning;
            resolution = ::PROPERTY_DUPLICATE_RESOLUTION;
            break;

        case ErrorCode::UNEXPECTED_FIELD:
            redfish_code = ServerError::PROPERTY_UNKNOWN;
            property_name = InvalidField::get_field_name_from_json_data(data);
            severity = Severity::Warning;
            resolution = ::PROPERTY_UNKNOWN_RESOLUTION;
            break;

        case ErrorCode::MISSING_FIELD:
            redfish_code = ServerError::PROPERTY_MISSING;
            property_name = InvalidField::get_field_name_from_json_data(data);
            severity = Severity::Warning;
            resolution = ::PROPERTY_MISSING_RESOLUTION;
            break;

        case ErrorCode::INVALID_ENUM:
            redfish_code = ServerError::PROPERTY_VALUE_NOT_IN_LIST;
            property_name = InvalidField::get_field_name_from_json_data(data);
            severity = Severity::Warning;
            resolution = ::PROPERTY_VALUE_NOT_IN_LIST_RESOLUTION;
            break;

        case ErrorCode::INVALID_VALUE_FORMAT:
            redfish_code = ServerError::PROPERTY_VALUE_FORMAT_ERROR;
            severity = Severity::Warning;
            resolution = ::PROPERTY_VALUE_FORMAT_ERROR_RESOLUTION;
            break;

        case ErrorCode::INVALID_VALUE:
        case ErrorCode::UNSUPPORTED_VALUE:
            redfish_code = ServerError::GENERAL_ERROR;
            severity = Severity::Critical;
            break;

        case ErrorCode::NOT_FOUND:
            redfish_code = ServerError::RESOURCE_MISSING_AT_URI;
            severity = Severity::Critical;
            resolution = ::RESOURCE_MISSING_AT_URI_RESOLUTION;
            break;
    }

    if (!property_name.empty()) {
        related_properties.push_back(property_name);
    }

    return create_message_object(redfish_code, message, severity, resolution, related_properties);
}


std::string resolve_va_args(const char* format, va_list args) {

    va_list args_copy;
    va_copy(args_copy, args);

    /* Get how much bytes to allocate for message string */
    int message_length = vsnprintf(NULL, 0, format, args_copy) + 1;
    char* buffer = new char[message_length];

    va_end(args_copy);

    /* Write the error message */
    vsnprintf(buffer, size_t(message_length), format, args);

    std::string message{buffer};
    delete[] buffer;
    return message;
}

}


ServerError ErrorFactory::create_error(const HttpStatusCode& code,
                                       const RedfishErrorCode& redfish_code,
                                       const char* format, ...) {
    va_list args;
    va_start(args, format);
    auto message = ::resolve_va_args(format, args);
    va_end(args);

    ServerError server_error{code};
    server_error.set_code(redfish_code);
    server_error.set_message(message);
    return server_error;
}


ServerError ErrorFactory::create_internal_error(const std::string& message) {
    auto server_error = create_error(INTERNAL_SERVER_ERROR, ServerError::INTERNAL_ERROR, ::INTERNAL_ERROR_MESSAGE);
    if (!message.empty()) {
        server_error.add_extended_message(
            ::create_message_object(
                ServerError::INTERNAL_ERROR,
                message,
                Severity::Critical,
                ::INTERNAL_ERROR_RESOLUTION
            )
        );
    }
    return server_error;
}


ServerError ErrorFactory::create_resource_in_use_error(const std::string& message) {
    auto server_error = create_error(BAD_REQUEST, ServerError::RESOURCE_IN_USE, ::RESOURCE_IN_USE_MESSAGE);
    if (!message.empty()) {
        server_error.add_extended_message(
            ::create_message_object(
                ServerError::RESOURCE_IN_USE,
                message,
                Severity::Warning,
                ::RESOURCE_IN_USE_RESOLUTION
            )
        );
    }
    return server_error;
}


ServerError ErrorFactory::create_malformed_json_error(const std::string& message) {
    auto server_error = create_error(BAD_REQUEST, ServerError::MALFORMED_JSON, ::MALFORMED_JSON_MESSAGE);
    if (!message.empty()) {
        server_error.add_extended_message(
            ::create_message_object(
                ServerError::MALFORMED_JSON,
                message,
                Severity::Critical,
                ::MALFORMED_JSON_RESOLUTION
            )
        );
    }
    return server_error;
}


ServerError ErrorFactory::create_resource_already_exists_error(const std::string& message) {
    auto server_error = create_error(BAD_REQUEST, ServerError::RESOURCE_ALREADY_EXISTS,
                                     ::RESOURCE_ALREADY_EXISTS_MESSAGE);
    if (!message.empty()) {
        server_error.add_extended_message(
            ::create_message_object(
                ServerError::RESOURCE_ALREADY_EXISTS,
                message,
                Severity::Critical,
                ::RESOURCE_ALREADY_EXISTS_RESOLUTION
            )
        );
    }
    return server_error;
}


ServerError ErrorFactory::create_resource_missing_error(const std::string& resource_uri, const std::string& message) {
    auto server_error = create_error(NOT_FOUND, ServerError::RESOURCE_MISSING_AT_URI,
                                     ::RESOURCE_MISSING_AT_URI_MESSAGE, resource_uri.c_str());
    if (!message.empty()) {
        server_error.add_extended_message(
            ::create_message_object(
                ServerError::RESOURCE_MISSING_AT_URI,
                message,
                Severity::Critical,
                ::RESOURCE_MISSING_AT_URI_RESOLUTION
            )
        );
    }
    return server_error;
}


ServerError ErrorFactory::create_resource_gone_error(const std::string& resource_uri, const std::string& message) {
    auto server_error = create_error(GONE, ServerError::RESOURCE_MISSING_AT_URI,
                                     ::RESOURCE_MISSING_AT_URI_MESSAGE, resource_uri.c_str());
    if (!message.empty()) {
        server_error.add_extended_message(
            ::create_message_object(
                ServerError::RESOURCE_MISSING_AT_URI,
                message,
                Severity::Critical,
                ::RESOURCE_MISSING_AT_URI_RESOLUTION
            )
        );
    }
    return server_error;
}


ServerError ErrorFactory::create_method_not_allowed_error(const std::string& message, const std::string& resolution) {
    auto server_error = create_error(METHOD_NOT_ALLOWED, ServerError::GENERAL_ERROR, ::GENERAL_ERROR_MESSAGE);
    if (!message.empty()) {
        server_error.add_extended_message(
            ::create_message_object(
                ServerError::GENERAL_ERROR,
                message,
                Severity::Critical,
                resolution
            )
        );
    }
    return server_error;
}


ServerError ErrorFactory::create_unauthorized_error(const std::string& resource_uri, const std::string& message) {
    auto server_error = create_error(UNAUTHORIZED, ServerError::RESOURCE_AT_URI_UNAUTHORIZED,
                                     ::UNAUTHORIZED_MESSAGE, resource_uri.c_str());
    if (!message.empty()) {
        server_error.add_extended_message(
            ::create_message_object(
                ServerError::RESOURCE_AT_URI_UNAUTHORIZED,
                message,
                Severity::Critical,
                ::UNAUTHORIZED_RESOLUTION
            )
        );
    }
    return server_error;
}


ServerError ErrorFactory::create_insufficient_privilege_error(const std::string& message) {
    auto server_error = create_error(UNAUTHORIZED, ServerError::INSUFFICIENT_PRIVILEGE,
                                     ::INSUFFICIENT_PRIVILEGE_MESSAGE);
    if (!message.empty()) {
        server_error.add_extended_message(
            ::create_message_object(
                ServerError::INSUFFICIENT_PRIVILEGE,
                message,
                Severity::Critical,
                ::INSUFFICIENT_PRIVILEGE_RESOLUTION
            )
        );
    }
    return server_error;
}


ServerError ErrorFactory::create_property_duplicated_error(const std::string& property, const std::string& message) {
    auto server_error = create_error(BAD_REQUEST, ServerError::PROPERTY_DUPLICATE,
                                     ::PROPERTY_DUPLICATE_MESSAGE, property.c_str());
    if (!message.empty()) {
        server_error.add_extended_message(
            ::create_message_object(
                ServerError::PROPERTY_DUPLICATE,
                message,
                Severity::Warning,
                ::PROPERTY_DUPLICATE_RESOLUTION
            )
        );
    }
    return server_error;
}


ServerError ErrorFactory::create_property_unknown_error(const std::string& property, const std::string& message) {
    auto server_error = create_error(BAD_REQUEST, ServerError::PROPERTY_UNKNOWN,
                                     ::PROPERTY_UNKNOWN_MESSAGE, property.c_str());
    if (!message.empty()) {
        server_error.add_extended_message(
            ::create_message_object(
                ServerError::PROPERTY_UNKNOWN,
                message,
                Severity::Warning,
                ::PROPERTY_UNKNOWN_RESOLUTION
            )
        );
    }
    return server_error;
}


ServerError ErrorFactory::create_property_type_error(const std::string& property,
                                                     const std::string& property_value,
                                                     const std::string& message) {
    auto server_error = create_error(BAD_REQUEST, ServerError::PROPERTY_VALUE_TYPE_ERROR,
                                     ::PROPERTY_VALUE_TYPE_ERROR_MESSAGE, property_value.c_str(), property.c_str());
    if (!message.empty()) {
        server_error.add_extended_message(
            ::create_message_object(
                ServerError::PROPERTY_VALUE_TYPE_ERROR,
                message,
                Severity::Warning,
                ::PROPERTY_VALUE_TYPE_ERROR_RESOLUTION
            )
        );
    }
    return server_error;
}


ServerError ErrorFactory::create_property_missing_error(const std::string& property,
                                                        const std::string& message,
                                                        const std::vector<std::string>& related_properties) {

    auto server_error = create_error(BAD_REQUEST, ServerError::PROPERTY_MISSING,
                                     ::PROPERTY_MISSING_MESSAGE, property.c_str());
    if (!message.empty()) {
        server_error.add_extended_message(
            ::create_message_object(
                ServerError::PROPERTY_MISSING,
                message,
                Severity::Warning,
                ::PROPERTY_MISSING_RESOLUTION,
                related_properties
            )
        );
    }
    return server_error;
}


ServerError ErrorFactory::create_value_format_error(const std::string& property,
                                                    const std::string& property_value,
                                                    const std::string& message) {
    auto server_error = create_error(BAD_REQUEST, ServerError::PROPERTY_VALUE_FORMAT_ERROR,
                                     ::PROPERTY_VALUE_FORMAT_ERROR_MESSAGE, property_value.c_str(), property.c_str());
    if (!message.empty()) {
        server_error.add_extended_message(
            ::create_message_object(
                ServerError::PROPERTY_VALUE_FORMAT_ERROR,
                message,
                Severity::Warning,
                ::PROPERTY_VALUE_FORMAT_ERROR_RESOLUTION
            )
        );
    }
    return server_error;
}


ServerError ErrorFactory::create_value_not_in_list_error(const std::string& property,
                                                         const std::string& property_value,
                                                         const std::string& message,
                                                         const std::string& resolution,
                                                         const std::vector<std::string>& related_properties) {

    auto server_error = create_error(BAD_REQUEST, ServerError::PROPERTY_VALUE_NOT_IN_LIST,
                                     ::PROPERTY_VALUE_NOT_IN_LIST_MESSAGE, property_value.c_str(), property.c_str());
    if (!message.empty()) {
        server_error.add_extended_message(
            ::create_message_object(
                ServerError::PROPERTY_VALUE_NOT_IN_LIST,
                message,
                Severity::Warning,
                resolution.empty() ? ::PROPERTY_VALUE_NOT_IN_LIST_RESOLUTION : resolution,
                related_properties
            )
        );
    }
    return server_error;
}


ServerError ErrorFactory::create_agent_unreachable_error(const std::string& message) {
    auto server_error = create_error(SERVICE_UNAVAILABLE, ServerError::SERVICE_TEMPORARILY_UNAVAILABLE,
                                     ::SERVICE_TEMPORARILY_UNAVAILABLE_MESSAGE, 5);
    if (!message.empty()) {
        server_error.add_extended_message(
            ::create_message_object(
                ServerError::SERVICE_TEMPORARILY_UNAVAILABLE,
                message,
                Severity::Critical,
                ::SERVICE_TEMPORARILY_UNAVAILABLE_RESOLUTION
            )
        );
    }
    return server_error;
}


ServerError ErrorFactory::create_invalid_payload_error(const std::string& message,
                                                       const std::vector<std::string>& json_fields,
                                                       const std::string& resolution) {

    auto server_error = create_error(BAD_REQUEST, ServerError::GENERAL_ERROR, ::GENERAL_ERROR_MESSAGE);
    if (!message.empty()) {
        server_error.add_extended_message(
            ::create_message_object(
                ServerError::GENERAL_ERROR,
                message,
                Severity::Critical,
                resolution,
                json_fields
            )
        );
    }
    return server_error;
}


ServerError ErrorFactory::create_error_for_not_allowable_value(const std::string& message,
                                                               const std::string& property_name,
                                                               const std::string& property_value,
                                                               const std::string& allowable_values_property_name) {
    std::stringstream resolution{};
    if (!allowable_values_property_name.empty()) {
        resolution << "Use one of the allowable values defined in ";
        resolution << allowable_values_property_name;
        resolution << " property.";
    }

    return error::ErrorFactory::create_value_not_in_list_error(
        property_name, property_value, message, resolution.str(), {property_name});
}


ServerError ErrorFactory::create_conflict_error(const std::string& message, const std::string& resolution) {
    auto server_error = create_error(CONFLICT, ServerError::GENERAL_ERROR, ::GENERAL_ERROR_MESSAGE);
    if (!message.empty()) {
        server_error.add_extended_message(
            ::create_message_object(
                ServerError::GENERAL_ERROR,
                message,
                Severity::Critical,
                resolution
            )
        );
    }
    return server_error;
}


ServerError ErrorFactory::create_not_implemented_error(const std::string& message, const std::string& resolution) {
    auto server_error = create_error(NOT_IMPLEMENTED, ServerError::GENERAL_ERROR, ::GENERAL_ERROR_MESSAGE);
    if (!message.empty()) {
        server_error.add_extended_message(
            ::create_message_object(
                ServerError::GENERAL_ERROR,
                message,
                Severity::Critical,
                resolution
            )
        );
    }
    return server_error;
}


ServerError ErrorFactory::create_error_from_set_component_attributes_results(
    Array<GamiResultStatus> statuses, const std::map<std::string, std::string>& fields_map) {

    auto error = create_internal_error();
    for (const auto& status : statuses) {
        auto message_object = ::create_message_object_from_gami_code(status.get_status_code(), status.get_message());
        log_error("rest", "SetComponentAttributes failed for attribute '"
            << status.get_attribute() << "' with message '" << int(status.get_status_code())
            << ":" << status.get_message() << "'");

        try {
            message_object.add_related_property(fields_map.at(status.get_attribute()));
        }
        catch (...) {
            // Do not add attribute name to response
            log_warning("rest",
                        "Create REST error: unrecognized GAMI attribute: " << status.get_attribute());
        }

        error.add_extended_message(message_object);
    }
    return error;
}


ServerError ErrorFactory::create_property_not_modifiable_error(const std::string& property,
                                                        const std::string& message,
                                                        const std::vector<std::string>& related_properties) {

    auto server_error = create_error(BAD_REQUEST, ServerError::PROPERTY_NOT_MODIFIABLE,
                                     ::PROPERTY_NOT_MODIFIABLE_MESSAGE, property.c_str());
    if (!message.empty()) {
        server_error.add_extended_message(
            ::create_message_object(
                ServerError::PROPERTY_NOT_MODIFIABLE,
                message,
                Severity::Warning,
                ::PROPERTY_NOT_MODIFIABLE_RESOLUTION,
                related_properties
            )
        );
    }
    return server_error;
}


ServerError ErrorFactory::create_property_value_restricted_error(const std::string& property,
                                                        const std::string& property_value,
                                                        const std::string& message,
                                                        const std::vector<std::string>& related_properties) {

    auto server_error = create_error(BAD_REQUEST, ServerError::PROPERTY_VALUE_RESTRICTED,
                                     ::PROPERTY_VALUE_RESTRICTED_MESSAGE, property_value.c_str(), property.c_str());
    if (!message.empty()) {
        server_error.add_extended_message(
            ::create_message_object(
                ServerError::PROPERTY_VALUE_RESTRICTED,
                message,
                Severity::Warning,
                ::PROPERTY_VALUE_RESTRICTED_RESOLUTION,
                related_properties
            )
        );
    }
    return server_error;
}


ServerError ErrorFactory::create_error_from_gami_exception(const GamiException& exception) {
    const auto& gami_error_code = exception.get_error_code();
    const auto& message = exception.get_message();
    const auto& data = exception.get_data();
    ServerError server_error{0};

    switch (gami_error_code) {
        case ErrorCode::UNKNOWN_ERROR:
        case ErrorCode::INVALID_PARAMETER:
        case ErrorCode::INVALID_UUID:
        case ErrorCode::INVALID_COLLECTION:
        case ErrorCode::COMPUTE:
        case ErrorCode::IPMI:
        case ErrorCode::NETWORK:
        case ErrorCode::FM10000:
        case ErrorCode::STORAGE:
        case ErrorCode::ISCSI:
        case ErrorCode::LVM:
        case ErrorCode::NVME:
        case ErrorCode::SPDK:
        case ErrorCode::PCIE_FABRIC:
        case ErrorCode::CHASSIS:
        case ErrorCode::FPGAOF:
        case ErrorCode::CERTIFICATE:
        default:
            server_error = create_internal_error();
            break;

        case ErrorCode::UNSUPPORTED_FIELD:
        case ErrorCode::INVALID_FIELD:
        case ErrorCode::UNSUPPORTED_VALUE:
        case ErrorCode::INVALID_VALUE:
            server_error = create_invalid_payload_error();
            break;

        case ErrorCode::INVALID_ENUM:
            server_error = create_value_not_in_list_error(InvalidField::get_field_name_from_json_data(data, false),
                                                          InvalidField::get_field_value_from_json_data(data, false));
            break;

        case ErrorCode::INVALID_VALUE_FORMAT:
            server_error = create_value_format_error(InvalidField::get_field_name_from_json_data(data, false),
                                                     InvalidField::get_field_value_from_json_data(data, false));
            break;

        case ErrorCode::MISSING_FIELD:
            server_error = create_property_missing_error(InvalidField::get_field_name_from_json_data(data, false));
            break;

        case ErrorCode::UNEXPECTED_FIELD:
            server_error = create_property_unknown_error(InvalidField::get_field_name_from_json_data(data, false));
            break;

        case ErrorCode::INVALID_FIELD_TYPE:
            server_error = create_property_type_error(InvalidField::get_field_name_from_json_data(data, false),
                                                      InvalidField::get_field_value_from_json_data(data, false));
            break;

        case ErrorCode::DUPLICATED_FIELD:
            server_error = create_property_duplicated_error(InvalidField::get_field_name_from_json_data(data, false));
            break;

        case ErrorCode::NOT_IMPLEMENTED:
        case ErrorCode::METHOD_NOT_FOUND:
            // Creates error with empty message because there will be added extended message later.
            server_error = create_not_implemented_error({});
            break;

        case ErrorCode::METHOD_NOT_ALLOWED:
            // Creates error with empty message because there will be added extended message later.
            server_error = create_method_not_allowed_error({});
            break;

        case ErrorCode::METHOD_CONFLICT: {
            auto task_uuid = MethodConflict::get_task_uuid(data);
            try {
                auto task_url = psme::rest::endpoint::utils::get_component_url(
                    agent_framework::model::enums::Component::Task, task_uuid);
                auto resolution = "Please wait until task " + task_url + " is finished, then try again";
                server_error = create_conflict_error(message, resolution);
            }
            catch (const NotFound&) {
                log_error("rest", "GAMI command failed because of a conflict with an unknown task: " + task_uuid);
                server_error = create_conflict_error(message);
            }
            return server_error;
        }

        case ErrorCode::NOT_FOUND:
            server_error = create_resource_missing_error(NotFound::get_uri_from_json_data(data, false));
            break;
    }

    server_error.add_extended_message(::create_message_object_from_gami_code(gami_error_code, message, data));
    return server_error;
}
