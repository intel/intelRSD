/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
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
 * */

#ifndef ASSET_CONFIGURATION_HPP
#define ASSET_CONFIGURATION_HPP

#include "agent-framework/logger_ext.hpp"
#include "agent-framework/module-ref/utils/utils.hpp"
#include "uuid++.hh"
#include "json/json.hpp"

#include <libxml++/libxml++.h>

namespace agent {

    using agent_framework::module::utils::OptionalField;
    /*!
     * @brief Class responsible for managing reading asset configuration from an xml file.
     * */
class AssetConfiguration {

public:

    /*!
     * @brief Get instance
     *
     * @return reference to the instance of class
     * */
    static AssetConfiguration& get_instance();

    /*!
     * @brief Read json configuration
     *
     * @param[in] configuration reference to json::Value configuration
     * */
    void read_configuration(const json::Value& configuration);

    /*!
     * @brief Get drawer
     *
     * @return pointer to the node representing a drawer
     * */
    xmlpp::Node* get_drawer();

    /*!
     * @brief Get document
     *
     * @return pointer to the DOM tree representing the asset
     * */
    xmlpp::Document* get_document();

    /*!
     * @brief Read string
     *
     * @param[in] element pointer to the node within whose children content will be
     * searched
     * @param[in] name name of the child node whose content is to be obtained
     *
     * @return string with content of the searched node
     * */
    static std::string
        read_string(const xmlpp::Node* element, const std::string& name);

    /*!
     * @brief Read optional string
     *
     * @param[in] element pointer to the node within whose children content will be
     * searched
     * @param[in] name name of the child node whose content is to be obtained
     *
     * @return optional string with content of the searched node
     * */
    static OptionalField<std::string>
        read_opt_string(const xmlpp::Node* element, const std::string& name);

    /*!
     * @brief Get content
     *
     * @param element Pointer to an element whose content will be obtained
     *
     * @return string with the content of the node
     * */
    static std::string get_content(const xmlpp::Node* element);

    /*!
     * @brief Read int
     *
     * @param[in] element pointer to the node within whose children content will be
     * searched
     * @param[in] name name of the child node whose content is to be obtained
     *
     * @return value of number contained by the searched node
     * */
    static uint32_t
    read_int(const xmlpp::Node* element, const std::string& name);

    /*!
     * @brief Read optional int
     *
     * @param[in] element pointer to the node within whose children content will be
     * searched
     * @param[in] name name of the child node whose content is to be obtained
     *
     * @return optional value of number contained by the searched node
     * */
    static OptionalField<uint32_t>
    read_opt_int(const xmlpp::Node* element, const std::string& name);

    /*!
     * @brief Read double
     *
     * @param[in] element pointer to the node within whose children content will be searched
     * @param[in] name name of the child node whose content is to be obtained
     *
     * @return value of number contained by the searched node
     * */
    static double
    read_double(const xmlpp::Node* element, const std::string& name);

    /*!
     * @brief Read optional double
     *
     * @param[in] element pointer to the node within whose children content will be searched
     * @param[in] name name of the child node whose content is to be obtained
     *
     * @return optional value of number contained by the searched node
     * */
    static OptionalField<double>
    read_opt_double(const xmlpp::Node* element, const std::string& name);

    /*!
     * @brief Read count
     *
     * @param[in] element pointer to the node within whose children specific nodes will be searched
     * @param[in] name name of the child nodes which will be counted
     *
     * @return number of child nodes with given name
     * */
    static uint32_t
    read_count(const xmlpp::Node* element, const std::string& name);

    /*!
     * @brief Read bool
     *
     * @param[in] element pointer to the node within whose children specific node will be searched
     * @param[in] name name of the child node whose content is to be obtained
     *
     * @return true if node's content is 'true', false otherwise
     * */
    static bool read_bool(const xmlpp::Node* element, const std::string& name);

    /*!
     * @brief Read optional bool
     *
     * @param[in] element pointer to the node within whose children specific node will be searched
     * @param[in] name name of the child node whose content is to be obtained
     *
     * @return optional value of bool: true if node's content is 'true', false if nodes content is 'false', null otherwise
     * */
    static OptionalField<bool> read_opt_bool(const xmlpp::Node* element, const std::string& name);

    /*!
     * @brief Set content
     *
     * @param[out] parent pointer to the parent node
     * @param[in] name name of the child node which will be added to the parent
     * @param[in] content content of the new child node
     * */
    static void
    set_content(xmlpp::Node* parent, const std::string& name, const std::string& content);

    /*!
     * @brief Remove element
     *
     * @param[out] element pointer to the element which will be deleted
     * */
    static void remove_element(xmlpp::Node* element);
    /*!
     * @brief Clean up, remove the instance of the class, if exists
     * */
    static void cleanup();
    ~AssetConfiguration();

private:
    bool m_save_output{false};
    bool m_validate{false};
    std::string m_input_file{};
    std::string m_output_file{};
    std::string m_schema_file{};
    xmlpp::SchemaValidator m_validator{};
    xmlpp::DomParser m_parser{};

    AssetConfiguration();
    AssetConfiguration& operator=(AssetConfiguration&) = delete;
    AssetConfiguration(const AssetConfiguration&) = delete;
    AssetConfiguration(AssetConfiguration&&) = delete;
    AssetConfiguration& operator=(AssetConfiguration&&) = delete;
    static void init();
    void read_filename(const json::Value& filename);
    void process_input(const json::Value& asset_input);
    void process_schema(const json::Value& asset_schema);
    void process_output(const json::Value& asset_output);

};

}

#endif /* ASSET_CONFIGURATION */
