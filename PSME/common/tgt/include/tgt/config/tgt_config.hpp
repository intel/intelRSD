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
 * @file tgt_config.hpp
 * @brief tgt configuration
*/

#pragma once



#include "tgt/target_data.hpp"

#include <cstdint>
#include <string>

namespace tgt {
namespace config {

/*! Config class adds and removes tgt config file for specified target */
class TgtConfig {
public:

    /*! Constructor */
    TgtConfig(const std::string configuration_path) :
        m_configuration_path(configuration_path) {
    }

    /*! Default constructor*/
    TgtConfig() = delete;

    /*! Disable copy */
    TgtConfig(const TgtConfig&) = delete;
    TgtConfig& operator=(const TgtConfig&) = delete;

    /*!
     * @brief Adds tgt target configuration file
     *
     * @param target Target
     */
    void add_target(const TargetData& target) const;


    /*!
     * @brief Removes tgt target configuration file
     *
     * @param target_iqn Target IQN
     *
     */
    void remove_target(const std::string& target_iqn) const;

private:
    std::string m_configuration_path{};


    std::string get_target_conf_file_name(const std::string& target_iqn) const;
};

}
}
