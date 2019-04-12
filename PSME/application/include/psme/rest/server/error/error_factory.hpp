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
 * @file error_factory.hpp
 *
 * @brief Declaration of ErrorFactory class
 * */
#pragma once



#include "psme/rest/server/error/server_exception.hpp"
#include "agent-framework/module/model/attributes/array.hpp"
#include "agent-framework/module/model/attributes/result_status.hpp"
#include "agent-framework/exceptions/exception.hpp"



namespace psme {
namespace rest {
namespace error {

/*!
 * @brief ServerError factory.
 *
 * Creates SeverError objects.
 * */
class ErrorFactory {
private:
    using GamiResultStatus = agent_framework::model::attribute::ResultStatus;
    using HttpStatusCode = std::uint32_t;
    using RedfishErrorCode = std::string;

    template<typename T>
    using Array = agent_framework::model::attribute::Array<T>;

public:

    /*!
     * @brief Create Redfish-defined error for internal server error.
     * @param[in] message Optional extended message.
     * @return Internal server error object.
     * */
    static ServerError create_internal_error(const std::string& message = {});

    /*!
     * @brief Create Redfish-defined error for resource in use.
     * @param[in] message Optional extended message.
     * @return Resource in use error object.
     * */
    static ServerError create_resource_in_use_error(const std::string& message = {});

    /*!
     * @brief Create Redfish-defined error for malformed JSON.
     * @param[in] message Optional extended message.
     * @return Malformed JSON error object.
     * */
    static ServerError create_malformed_json_error(const std::string& message = {});


    /*!
     * @brief Create Redfish-defined error for already existing resource.
     * @param[in] message Optional extended message.
     * @return Already existing resource error object.
     * */
    static ServerError create_resource_already_exists_error(const std::string& message = {});


    /*!
     * @brief Create Redfish-defined error for not existing resource.
     * @param[in] resource_uri Resource at requested URI.
     * @param[in] message Optional extended message.
     * @return Not existing resource error object.
     * */
    static ServerError create_resource_missing_error(const std::string& resource_uri, const std::string& message = {});


    /*!
     * @brief Create Redfish-defined error for resource gone.
     * @param[in] resource_uri Resource at requested URI.
     * @param[in] message Mandatory extended message.
     * @return Gone error object.
     * */
    static ServerError create_resource_gone_error(const std::string& resource_uri, const std::string& message);


    /*!
     * @brief Create Redfish-defined error for problems with connection to agent.
     * @param[in] message Optional extended message.
     * @return Agent unreachable error object.
     * */
    static ServerError create_agent_unreachable_error(const std::string& message = {});


    /*!
     * @brief Create Redfish-defined error for not method not allowed.
     * @param[in] message Mandatory extended message.
     * @param[in] resolution Optional resolution.
     * @return Method not allowed error object.
     * */
    static ServerError create_method_not_allowed_error(const std::string& message, const std::string& resolution = {});


    /*!
     * @brief Create Redfish-defined error for unauthorized access.
     * @param[in] resource_uri Resource at requested URI.
     * @param[in] message Optional extended message.
     * @return Unauthorized access error object.
     * */
    static ServerError create_unauthorized_error(const std::string& resource_uri, const std::string& message = {});


    /*!
     * @brief Create Redfish-defined error for insufficient privilege access.
     * @param[in] message Optional extended message.
     * @return Insufficient privilege access error object.
     * */
    static ServerError create_insufficient_privilege_error(const std::string& message = {});


    /*!
     * @brief Create Redfish-defined error for duplicated property.
     * @param[in] property Name of duplicated property.
     * @param[in] message Optional extended message.
     * @return Property duplicated error object.
     * */
    static ServerError create_property_duplicated_error(const std::string& property, const std::string& message = {});


    /*!
     * @brief Create Redfish-defined error for unknown property.
     * @param[in] property Name of unknown property.
     * @param[in] message Optional extended message.
     * @return Property unknown error object.
     * */
    static ServerError create_property_unknown_error(const std::string& property, const std::string& message = {});


    /*!
     * @brief Create Redfish-defined error for missing property.
     *
     * @param[in] property Name of missing property.
     * @param[in] message Optional extended message.
     * @param[in] related_properties Optional list of related JSON properties.
     *
     * @return Property missing error object.
     * */
    static ServerError create_property_missing_error(const std::string& property,
                                                     const std::string& message = {},
                                                     const std::vector<std::string>& related_properties = {});


    /*!
     * @brief Create Redfish-defined error for value format of the property.
     * @param[in] property Name of invalid property.
     * @param[in] property_value Invalid value of the property.
     * @param[in] message Optional extended message.
     * @return Value format error object.
     * */
    static ServerError create_value_format_error(const std::string& property,
                                                 const std::string& property_value,
                                                 const std::string& message = {});



    /*!
     * @brief Create Redfish-defined error for value not in list.
     *
     * @param[in] property Name of invalid property.
     * @param[in] property_value Invalid value of the property.
     * @param[in] message Optional extended message.
     * @param[in] resolution Optional resolution.
     * @param[in] related_properties Optional list of related JSON properties.
     *
     * @return Value not in list error object.
     * */
    static ServerError create_value_not_in_list_error(const std::string& property,
                                                      const std::string& property_value,
                                                      const std::string& message = {},
                                                      const std::string& resolution = {},
                                                      const std::vector<std::string>& related_properties = {});


    /*!
     * @brief Create Redfish-defined error for wrong type of property.
     * @param[in] property Name of invalid property.
     * @param[in] property_value Invalid value of the property.
     * @param[in] message Optional extended message.
     * @return Property type error object.
     * */
    static ServerError create_property_type_error(const std::string& property,
                                                  const std::string& property_value,
                                                  const std::string& message = {});


    /*!
     * @brief Create Redfish-defined error for invalid payload.
     *
     * @param[in] message Optional error message.
     * @param[in] json_fields Optional list of invalid JSON properties.
     * @param[in] resolution Optional resolution.
     *
     * @return Invalid payload error object.
     * */
    static ServerError create_invalid_payload_error(const std::string& message = {},
                                                    const std::vector<std::string>& json_fields = {},
                                                    const std::string& resolution = {}) __attribute__((deprecated));


    /*!
     * @brief Create Redfish-defined error for not allowable action value.
     *
     * @param[in] message Extended error message.
     * @param[in] property_name Name of property with not allowable value.
     * @param[in] allowable_values_property_name Optional name of property with allowable values.
     *
     * @return Value not in list error.
     * */
    static ServerError create_error_for_not_allowable_value(const std::string& message,
                                                            const std::string& property_name,
                                                            const std::string& property_value,
                                                            const std::string& allowable_values_property_name = {});


    /*!
     * @brief Create Redfish-defined error for conflict.
     * @param[in] message Mandatory extended message.
     * @param[in] resolution Optional resolution.
     * @return Conflict error object.
     * */
    static ServerError create_conflict_error(const std::string& message, const std::string& resolution = {});


    /*!
     * @brief Create Redfish-defined error for method not implemented.
     * @param[in] message Mandatory extended message.
     * @param[in] resolution Optional resolution.
     * @return Not implemented error object.
     * */
    static ServerError create_not_implemented_error(const std::string& message, const std::string& resolution = {});


    /*!
     * @brief Create Redfish-defined error from SetComponentAttributes execution results.
     *
     * @param[in] statuses SetComponentAttributes results.
     * @param[in] fields_map Pairs with GAMI properties and REST API properties.
     *
     * @return Error object with extended messages from SetComponentAttributes command.
     * */
    static ServerError create_error_from_set_component_attributes_results(
        Array<GamiResultStatus> statuses, const std::map<std::string, std::string>& fields_map);


    /*!
     * @brief Create Intel RackScale-defined error for unmodifiable property.
     *
     * @param[in] property Name of unmodifiable property.
     * @param[in] message Optional extended message.
     * @param[in] related_properties Optional list of related JSON properties.
     *
     * @return Property not modifiable error object.
     * */
    static ServerError create_property_not_modifiable_error(const std::string& property,
                                                     const std::string& message = {},
                                                     const std::vector<std::string>& related_properties = {});


    /*!
     * @brief Create Intel RackScale-defined error for restricted value of property.
     *
     * @param[in] property Name of invalid property.
     * @param[in] message Optional extended message.
     * @param[in] related_properties Optional list of related JSON properties.
     *
     * @return Property restricted value error object.
     * */
    static ServerError create_property_value_restricted_error(const std::string& property,
                                                     const std::string& property_value,
                                                     const std::string& message = {},
                                                     const std::vector<std::string>& related_properties = {});


    /*!
     * @brief Create Redfish-defined error from GAMI error.
     * @param[in] exception GAMI exception.
     * @return Error object created from GAMI error.
     * */
    static ServerError create_error_from_gami_exception(const agent_framework::exceptions::GamiException& exception);


private:
    ErrorFactory() = delete;


    ErrorFactory(const ErrorFactory&) = delete;


    ErrorFactory& operator=(const ErrorFactory&) = delete;


    static ServerError create_error(const HttpStatusCode& code,
                                    const RedfishErrorCode& redfish_code,
                                    const char* format, ...);
};

}
}
}

