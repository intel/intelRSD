/*!
 * @brief Options set for a logger
 *
 * All logger settings container
 *
 * @copyright Copyright (c) 2016-2019 Intel Corporation
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
 * @file logger_options.hpp
 */

#pragma once

namespace logger_cpp {

/*!
 * @enum Level
 * @brief Log level
 * */
enum class Level {
    EMERGENCY   = 0,
    ALERT       = 1,
    CRITICAL    = 2,
    ERROR       = 3,
    WARNING     = 4,
    NOTICE      = 5,
    INFO        = 6,
    DEBUG       = 7
};

/*!
 * @enum TimeFormat
 * @brief Time stamp format
 * */
enum class TimeFormat {
    NONE        = 0,
    DATE_SEC    = 1,
    DATE_MS     = 2,
    DATE_US     = 3,
    DATE_NS     = 4
};

/*!
 * @class logger_cpp::Options
 * @brief Options for #logger_cpp::Logger and #logger_cpp::Stream objects
 * */
class Options {
private:
    unsigned int m_raw;
    friend class Logger;
    friend class Stream;
public:
    /*!
     * @brief Default constructor. Set options to default:
     *  * Debug level
     *  * Time stamp format set to date with seconds precision
     *  * Coloring output is disabled
     *  * Tagging output is enabled
     *  * More debug information is enabled
     *  * Output is enabled
     * */
    explicit Options();

    /*!
     * @brief Set log level options @see logger_cpp::Level
     * @param[in]   level   Log level
     * */
    void set_level(Level level);

    /*!
     * @brief Get default log level defined in configuration.json
     * @see logger_cpp::Level
     * @return Default log level
     */
    Level get_default_level();

    /*!
     * @brief Set time stamp format @see logger_cpp::TimeFormat
     * @param[in]   time_format  Time stamp format
     * */
    void set_time_format(TimeFormat time_format);

    /*!
     * @brief Enable/disable output coloring
     * @param[in]   enable      Enable/disable coloring
     * */
    void enable_color(bool enable);

    /*!
     * @brief Enable/disable output
     * @param[in]   enable      Enable/disable output
     * */
    void enable_output(bool enable);

    /*!
     * @brief Enable/disable output tagging
     * @param[in]   enable      Enable/disable tagging
     * */
    void enable_tagging(bool enable);

    /*!
     * @brief Enable/disable more debug to output
     * @param[in]   enable      Enable/disable more debug
     * */
    void enable_more_debug(bool enable);
};

}
