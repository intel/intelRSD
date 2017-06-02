/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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

#include "asset_configuration/asset_configuration.hpp"
#include <fstream>

namespace agent {
namespace {
static AssetConfiguration* g_asset_configuration = nullptr;
bool file_exists(std::string filename) {
    std::ifstream ifile(filename);
    if (ifile) {
        return true;
    }
    return false;
}
}

using agent_framework::module::utils::OptionalField;

AssetConfiguration::AssetConfiguration() {}

xmlpp::Node* AssetConfiguration::get_drawer() {
    if (m_parser.get_document()) {
        return m_parser.get_document()->get_root_node();
    }
    return nullptr;
}

xmlpp::Document* AssetConfiguration::get_document() {
    return m_parser.get_document();
}

void AssetConfiguration::cleanup() {
    if (g_asset_configuration) {
        delete g_asset_configuration;
        g_asset_configuration = nullptr;
    }
}

AssetConfiguration& AssetConfiguration::get_instance() {
    if (nullptr == g_asset_configuration) {
        AssetConfiguration::init();
    }
    return *g_asset_configuration;
}

void AssetConfiguration::init() {
    g_asset_configuration = new AssetConfiguration();
}


void AssetConfiguration::read_configuration(const json::Value& configuration) {
    if (configuration["assetConfiguration"].is_object()) {
        read_filename(configuration["assetConfiguration"]);
    }
}


void AssetConfiguration::read_filename(const json::Value& asset_configuration) {
    if (asset_configuration["input"].is_string()) {
        process_input(asset_configuration["input"].as_string());
    }
    else {
        log_info(GET_LOGGER("agent"), "Cannot read asset configuration.");
    }

    if (asset_configuration["output"].is_string()) {
        process_output(asset_configuration["output"].as_string());
    }
    else {
        log_info(GET_LOGGER("agent"), "No output will be saved.");
    }

    if (asset_configuration["schema"].is_string()) {
        process_schema(asset_configuration["schema"].as_string());
    }
    else {
        log_info(GET_LOGGER("agent"), "No validation will be performed.");
    }
}

void AssetConfiguration::process_input(const json::Value& asset_input) {
    m_input_file = asset_input.as_string();
    m_parser.parse_file(m_input_file);
}

void AssetConfiguration::process_output(const json::Value& asset_output) {
    m_output_file = asset_output.as_string();
    if (file_exists(m_output_file)) {
        log_info(GET_LOGGER("agent"), "Output will be saved to " << m_output_file);
        m_save_output = true;
    }
    else {
        log_info(GET_LOGGER("agent"), "Cannot write to file " << m_output_file);
        m_save_output = false;
    }
}

void AssetConfiguration::process_schema(const json::Value& asset_schema) {
    m_schema_file = asset_schema.as_string();
    if (file_exists(m_schema_file)) {
        log_info(GET_LOGGER("agent"), m_input_file << " will be validated according to " << m_schema_file);
        m_validate = false;
        m_validator.parse_file(m_schema_file);
        m_validator.validate(this->get_document());
    }
    else {
        log_info(GET_LOGGER("agent"), "Cannot read file " << m_schema_file);
        m_validate = false;
    }
}

std::string AssetConfiguration::
read_string(const xmlpp::Node* element, const std::string& name) {
    if (element && element->get_first_child(name) && dynamic_cast<const xmlpp::TextNode*>
                (element->get_first_child(name)->get_first_child())) {
        return (dynamic_cast<const xmlpp::TextNode*>
                (element->get_first_child(name)->get_first_child())->get_content());
    }
    return std::string{};
}

OptionalField<std::string> AssetConfiguration::
read_opt_string(const xmlpp::Node* element, const std::string& name) {
    if (element && element->get_first_child(name) && dynamic_cast<const xmlpp::TextNode*>
                (element->get_first_child(name)->get_first_child())) {
        const std::string return_string = (dynamic_cast<const xmlpp::TextNode*>
                (element->get_first_child(name)->get_first_child()))->get_content();
        return OptionalField<std::string>(return_string);
    }
    return OptionalField<std::string>();
}

std::string AssetConfiguration::
get_content(const xmlpp::Node* element) {
    if (element) {
        auto text_node = dynamic_cast<const xmlpp::TextNode*>
                (element->get_first_child());
        if (text_node) {
            return text_node->get_content();
        }
    }
    return std::string{};
}

uint32_t AssetConfiguration::
read_int(const xmlpp::Node* element, const std::string& name) {
    if (element) {
        try {
            return (read_string(element, name)).empty()?
            0:
            (static_cast<uint32_t>(std::stoul(read_string(element, name))));
        }
        catch (std::invalid_argument&) {
            log_debug(GET_LOGGER("agent"), "Property" <<
                                            " '"<< name << "' "
                                            "is not convertible to unsigned int");
            return 0;
        }
    }
    return 0;
}

OptionalField<uint32_t> AssetConfiguration::
read_opt_int(const xmlpp::Node* element, const std::string& name) {
    if (element) {
        try {
            return
                OptionalField<std::uint32_t>(static_cast<uint32_t>(std::stoul(read_string(element, name))));
        }
        catch (std::invalid_argument&) {
            log_debug(GET_LOGGER("agent"), "Property" <<
                                            " '"<< name << "' "
                                            "is not convertible to unsigned int");
            return OptionalField<std::uint32_t>();
        }
    }
    return OptionalField<std::uint32_t>();
}

OptionalField<double> AssetConfiguration::
read_opt_double(const xmlpp::Node* element, const std::string& name) {
    if (element) {
        try {
            return
            OptionalField<double>(static_cast<double>(std::stod(read_string(element, name))));
        }
        catch (std::invalid_argument&) {
            log_debug(GET_LOGGER("agent"), "Property" <<
                                            " '"<< name << "' "
                                            "is not convertible to double");
            return OptionalField<double>();
        }
    }
    return OptionalField<double>();
}

double AssetConfiguration::
read_double(const xmlpp::Node* element, const std::string& name) {
    if (element) {
        try {
            return (read_string(element, name)).empty()?
                0:
                (static_cast<double>(std::stod(read_string(element, name))));
        }
        catch (std::invalid_argument&) {
            log_debug(GET_LOGGER("agent"), "Property" <<
                                            "'"<< name << "'"
                                            "is not convertible to double");
            return 0.0;
        }
    }
    return 0.0;
}

uint32_t AssetConfiguration::
read_count(const xmlpp::Node* element, const std::string& name) {
    if (element) {
        return (static_cast<uint32_t>((element->get_children(name)).size()));
    }
    return 0;
}

OptionalField<bool> AssetConfiguration::
read_opt_bool(const xmlpp::Node* element, const std::string& name) {
    if (element) {
        if (read_string(element, name) == ("false")) {
            return OptionalField<bool>(false);
        }
        else if (read_string(element, name) == ("true")) {
            return OptionalField<bool>(true);
        }
    }
    return OptionalField<bool>();
}

bool AssetConfiguration::
read_bool(const xmlpp::Node* element, const std::string& name) {
    if (element) {
        return (read_string(element, name).compare("false"));
    }
    return true;
}

void AssetConfiguration::
set_content(xmlpp::Node* parent, const std::string& name, const std::string& content) {
    if (parent) {
        if (!name.empty() && !content.empty()) {
            auto child = dynamic_cast<xmlpp::Element*>
                    (parent->get_first_child(name));
            if (child) {
                child->set_child_text(content);
             }
        }
    }
}

void AssetConfiguration:: remove_element(xmlpp::Node* element) {
    if (element) {
        element->get_parent()->remove_child(element);
    }
}

AssetConfiguration::~AssetConfiguration() {
    xmlpp::Document* output_file = get_document();
    if (m_save_output && output_file) {
        try {
            output_file->write_to_file(m_output_file);
        }
        catch (const xmlpp::exception& e) {
            log_error(GET_LOGGER("agent"), "Writing to file failed! "
                    << e.what());
        }

    }
}
}
