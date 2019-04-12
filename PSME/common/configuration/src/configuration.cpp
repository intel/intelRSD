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
 * @file configuration.cpp
 *
 * @brief Application configuration implementation
 * */

#include "configuration/configuration.hpp"
#include "configuration/utils.hpp"
#include "logger/logger_factory.hpp"
#include "utils/crypt_utils.hpp"
#include "utils/conversion.hpp"

#include <libgen.h>
#include <string.h>
#include <string>
#include <fstream>
#include <streambuf>
#include <stdexcept>
#include <algorithm>
#include <cstdlib>
#include <cstring>



using namespace configuration;

constexpr const char Configuration::DEFAULT_ENV_FILE[];

constexpr const char Configuration::DEFAULT_FILE[];

constexpr const char Configuration::DEFAULT_CONFIGURATION[];

constexpr const char Configuration::KEY_FILE[];

static Configuration* g_configuration = nullptr;


std::string Configuration::decrypt_value(const std::string& value) const {
    if (value.empty() || m_encryption_key.empty() || m_encryption_init_vector.empty()) {
        return value;
    }

    std::string encrypted_value = utils::hex_string_to_string(value);
    return utils::decrypt(encrypted_value, m_encryption_key, m_encryption_init_vector);
}


std::string Configuration::load_file(const std::string& file_name) const {
    /* Load user defined configuration file */
    std::ifstream file_stream(file_name);

    if (!file_stream.good()) {
        throw std::runtime_error("Cannot open file " + file_name);
    }

    /* Go to the file end */
    file_stream.seekg(0, std::ios::end);

    /* Get file size */
    std::ifstream::pos_type size = file_stream.tellg();
    if (size < 0) {
        throw std::runtime_error("Cannot read size of file " + file_name);
    }

    std::string configuration{};

    /* Reserve some space for reading file */
    configuration.reserve(std::string::size_type(size));

    /* Back to the file begin */
    file_stream.seekg(0, std::ios::beg);

    /* Copy all file from input file stream to string */
    configuration.assign((std::istreambuf_iterator<char>(file_stream)),
                         std::istreambuf_iterator<char>());

    log_info("configuration", "Loaded file " << file_name);

    return configuration;
}


std::string Configuration::get_key_file_path(
    const std::string& configuration_file_path) const {
    char* path = strdup(configuration_file_path.c_str());
    if (nullptr == path) {
        throw std::runtime_error("cannot allocate memory");
    }
    char* dir = dirname(path);

    std::string key_file_path = dir;
    key_file_path += std::string("/") + std::string(KEY_FILE);

    free(path);

    return key_file_path;
}


void Configuration::load_key_file() {
    for (const auto& config_file : m_configuration_files) {
        if (try_load_key_file(config_file)) {
            return;
        }
    }
}

bool Configuration::try_load_key_file(const std::string& configuration_file) {
    try {
        std::string key_file_path = get_key_file_path(configuration_file);
        auto encryption_initializers = utils::load_crypt_initializers(key_file_path);

        std::string encryption_key = encryption_initializers.first;
        std::string init_vector = encryption_initializers.second;

        if (!(encryption_key.empty() || init_vector.empty())) {
            m_encryption_key = encryption_key;
            m_encryption_init_vector = init_vector;
            return true;
        }
    }
    catch (const std::exception&) {
        return false;
    }

    return false;
}


Configuration::FileContentList Configuration::create_file_content_list() const {
    FileContentList file_content_list{};

    /* Load default file
     * */
    const char* env = std::getenv(m_default_env_file.c_str());
    if ((nullptr != env) && std::ifstream(env).good()) {
        log_info("configuration", "Load file from environment");
        file_content_list.emplace_back(env, load_file(env));
    }
    else if (std::ifstream(m_default_file).good()) {
        log_info("configuration", "Load default file");
        file_content_list.emplace_back(m_default_file, load_file(m_default_file));
    }
    else {
        log_warning("configuration", "Cannot load default file");
    }

    /* Load configuration from collection of files
     * */
    for (const auto& config_file : m_configuration_files) {
        log_info("configuration", "Load file");
        file_content_list.emplace_back(config_file, load_file(config_file));
    }

    /* Load internal defaults
     * */
    file_content_list.emplace_back("default", m_default_configuration);
    log_info("configuration", "Load internal defaults");

    return file_content_list;
}


const json::Json& Configuration::to_json() {
    if (!m_new_content[Content::JSON]) {
        return m_json_configuration;
    }
    m_new_content.reset(Content::JSON);

    m_json_configuration = nullptr;

    try {
        const auto content_list = create_file_content_list();

        for (const auto& content : content_list) {
            try {
                json::Json value = json::Json::parse(content.second);
                if ("default" != content.first) {
                    update_json(m_json_configuration, value);
                }
                else {
                    std::string json_path = std::string("<configuration>");
                    update_json_with_defaults(m_json_configuration, value, json_path);
                }
            }
            catch (const std::exception& e) {
                throw std::runtime_error(std::string{"Configuration "} + e.what());
            }
        }
    }
    catch (const std::exception& e) {
        log_error("configuration", e.what());
        std::exit(-1);
    }

    return m_json_configuration;
}


void Configuration::update_json(json::Json& old_value,
                                const json::Json& new_value) {

/* Updating all members in old JSON object from new JSON object,
     * for others JSON types override old with new JSON value
     * */
    if (old_value.is_object() && new_value.is_object()) {
        for (auto new_member = new_value.begin(); new_member != new_value.end(); ++new_member) {
            if (old_value.count(new_member.key())) {
                /* Iterate deeper through JSON object */
                update_json(old_value[new_member.key()], new_member.value());
            }
            else {
                /* Add new JSON member to JSON object */
                old_value[new_member.key()] = new_member.value();
            }
        }
    }
    else {
        /* Override JSON value with new */
        old_value = new_value;
    }
}


void Configuration::update_json_with_defaults(json::Json& old_value,
                                              const json::Json& new_value, std::string& in_path) {

    /* Checks if provided configuration overwrites the default one
     * and sets default values if not.
     * */
    if (old_value.is_object() && new_value.is_object()) {
        for (auto new_member = new_value.begin(); new_member != new_value.end(); ++new_member) {
            in_path.append("::" + new_member.key());
            if (old_value.count(new_member.key())) {
                /* Iterate deeper through JSON object */
                update_json_with_defaults(old_value[new_member.key()], new_member.value(), in_path);
            }
            else {
                /* Add new JSON member to JSON object */
                log_warning("configuration",
                            "No configuration section " + in_path + " provided. Using internal defaults.");
                old_value[new_member.key()] = new_member.value();
            }
            in_path.erase(in_path.rfind(std::string("::")), std::string::npos);
        }
    }
    else {
        /* Override JSON value with new */
        if (old_value.is_object() || new_value.is_object()) {
            log_warning("configuration", "No configuration section " + in_path + " provided. Using internal defaults.");
            old_value = new_value;
        }
    }
}


void Configuration::add_file(const std::string& file_name) {
    /* Find existing file name */
    const auto it = std::find(m_configuration_files.cbegin(),
                              m_configuration_files.cend(), file_name);

    /* Add new unique element */
    if (it == m_configuration_files.cend()) {
        m_configuration_files.emplace_back(file_name);
        m_new_content.set();
        log_debug("configuration", "Added file " << file_name);
    }
}


Configuration& Configuration::get_instance() {
    if (nullptr == g_configuration) {
        g_configuration = new Configuration();
    }
    return *g_configuration;
}


void Configuration::cleanup() {
    log_debug("configuration", "Cleanup");
    delete g_configuration;
    g_configuration = nullptr;
}
