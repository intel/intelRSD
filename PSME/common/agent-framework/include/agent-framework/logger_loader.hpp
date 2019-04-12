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
 * */
#pragma once



#include "json-wrapper/json-wrapper.hpp"
#include "logger/logger_factory.hpp"

#include <vector>
#include <array>



namespace logger_cpp {

/*!
 * @brief The LoggerLoader class for loading logger configuration
 */
class LoggerLoader {
public:
    /*!
     * @brief LoggerLoader Create loader object for given configuration
     * @param[in] config Configuration JSON data
     */
    LoggerLoader(const json::Json& config) : m_config_json{config} {}

    /*!
     * @brief load Create loggers in the logger factory from configuration data
     * @param factory logger factory where the loggers are to be configured
     */
    void load(LoggerFactory& factory);

private:
    /*!
     * @brief add_logger Create logger object for given configuration
     * @param[in] config Configuration data for logger
     * @return Logger Object
     */
    LoggerSPtr get_logger(const json::Json& config);

    /*!
     * @brief add_streams Create stream object for given configuration
     * @param[in] logger Logger object
     * @param[in] config Configuration data for stream
     */
    void add_streams(LoggerSPtr logger, const json::Json& config);

    /*!
     * @brief add_options Add additional options to logger
     * @param logger Logger object
     * @param config Configuration data for options
     */
    void add_options(LoggerSPtr logger, const json::Json& config);

    /*!
     * @brief get_stream Create stream object for given configuration
     * @param[in] config Configuration data for stream
     * @return  Stream object
     */
    StreamSPtr get_stream(const json::Json& config);

    /*!
     * @brief get_options Create option object for given configuration
     * @param[in] config Configuration data for options
     * @return Option object
     */
    Options get_options(const json::Json& config);

    /*!
     * @brief process_loggers Iterate through loggers list
     * @param[in] loggers JSON loggers object
     * @return Logger map object
     */
    LoggerFactory::loggers_t process_loggers(const json::Json& loggers);

    /*!
     * @brief process_streams Iterate through streams list
     * @param[in] logger object
     * @param[in] streams JSON stream list
     */
    bool process_streams(LoggerSPtr logger, const json::Json& streams);

    /*!
     * @brief set_stream_output Sets stream output
     * @param[in] type stream type
     * @param[in] stream stream object
     * @param[in] config config JSON object
     */
    void set_stream_output(Stream::Type type, StreamSPtr stream, const json::Json& config);

    /*!
     * @brief check_enums check if given JSON property in option is valid enum type
     * @param[in] options List with enum names
     * @param[in] type Searching name
     * @return The number, that can be converted to enum type, on error, -1 is returned
     */
    template<std::size_t N>
    static int check_enums(const std::array<const char*, N>& options, const std::string& type) {
        for (auto i = 0u; i < N; i++) {
            if(options[i] == type) {
                return int(i);
            }
        }
        return -1;
    }

    static std::array<const char*, 8> g_level;
    static std::array<const char*, 5> g_time_format;
    static std::array<const char*, 6> g_stream_type;

    /*!
     * @brief Reference to configuration data
     */
    const json::Json& m_config_json;
};
}


