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
 * @file configuration.hpp
 * @brief Application configuration interface
 * */

#pragma once
#include "json-wrapper/json-wrapper.hpp"

#include <string>
#include <bitset>
#include <climits>

namespace configuration {

/*!
 * @brief Configuration singleton
 *
 * Get configuration settings from file or when not file exist, load default
 * settings
 * */
class Configuration {
public:
    /*! Default environment variable */
    static constexpr const char DEFAULT_ENV_FILE[] = "CONFIGURATION_FILE";

    /*! Default file to load */
    static constexpr const char DEFAULT_FILE[] = "configuration.json";

    /*! Default configuration */
    static constexpr const char DEFAULT_CONFIGURATION[] = R"({})";

    /*! Files separator */
    static constexpr const char FILES_SEPARATOR = ':';

    /*! Key file name */
    static constexpr const char KEY_FILE[] = "psme-key";

    /*!
     * @brief Get configuration in JSON C++ object
     *
     * @return  JSON C++ object that contain configuration settings
     * */
    const json::Json& to_json();

    /*!
     * @brief Set default configuration
     *
     * @param[in]   default_configuration   Default configuration
     * */
    void set_default_configuration(const std::string& default_configuration) {
        m_new_content.set();
        m_default_configuration = default_configuration;
    }

    /*!
     * @brief Set default file configuration
     *
     * @param[in]   default_file            New configuration file name
     * */
    void set_default_file(const std::string& default_file) {
        m_new_content.set();
        m_default_file = default_file;
    }

    /*!
     * @brief Set default environment variable name
     *
     * @param[in]   default_env_file       New env for configuration files
     * */
    void set_default_env_file(const std::string& default_env_file) {
        m_new_content.set();
        m_default_env_file = default_env_file;
    }

    /*!
     * @brief Add file from which load configuration
     *
     * @param[in]   file_name   Path to file configuration
     * */
    void add_file(const std::string& file_name);

    /*!
     * @brief singleton. Get configuration
     *
     * After first use, it creates configuration object
     *
     * @return Configuration object
     * */
    static Configuration& get_instance();

    /*!
     * @brief Decrypt encrypted value using configuration key.
     *
     * @param[in] value Encrypted value
     * @return Decrypted value
     * */
    std::string decrypt_value(const std::string& value) const;

    /*!
     * @brief Load encryption key
     * */
    void load_key_file();

    /*!
     * @brief Singleton. Configuration cleanup
     * */
    static void cleanup();
private:
    using FileName = const std::string;
    using FileContent = const std::string;
    using FileContentList = std::vector<std::pair<FileName, FileContent>>;

    static const constexpr unsigned MAX_CONTENTS = 8;

    enum Content {
        JSON    = 0x01
    };

    std::bitset<MAX_CONTENTS> m_new_content{ULLONG_MAX};
    std::string m_default_configuration{DEFAULT_CONFIGURATION};
    std::string m_default_env_file{DEFAULT_ENV_FILE};
    std::string m_default_file{DEFAULT_FILE};
    json::Json m_json_configuration{};
    std::vector<std::string> m_configuration_files{};
    std::string m_encryption_key{};
    std::string m_encryption_init_vector{};

    Configuration() { }
    Configuration(const Configuration&) = delete;
    Configuration(Configuration&&) = delete;
    Configuration& operator=(const Configuration&) = delete;
    Configuration& operator=(Configuration&&) = delete;

    std::string load_file(const std::string& file_name) const;
    std::string get_key_file_path(const std::string& configuration_file_name) const;
    bool try_load_key_file(const std::string& key_file_path);
    FileContentList create_file_content_list() const;
    static void update_json(json::Json& new_value,
            const json::Json& old_value);
    static void update_json_with_defaults(json::Json& new_value,
            const json::Json& old_value,  std::string& in_path);
};

} /* namespace configuration */

