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
 * @file tgt_target_config.hpp
 * @brief tgt target configuration
*/

#pragma once



#include "tgt/target_data.hpp"

#include <cstdint>
#include <string>

namespace tgt {
namespace config {

/*! Represents TGT target configuration */
class TgtTargetConfig {
public:

    /*! Default constructor*/
    TgtTargetConfig(const TargetData& target) : m_target(target) {
    }

    /*! Disable copy constructor*/
    TgtTargetConfig(const TgtTargetConfig&) = delete;

    /*! Disable assignment operator */
    TgtTargetConfig& operator=(const TgtTargetConfig&) = delete;

    /*! Disable move */
    TgtTargetConfig& operator=(const TgtTargetConfig&&) = delete;

    /*!
     * @brief Returns target configuration string
     *
     * @return target configuration string
     */
    const std::string to_string() const;

private:
    TargetData m_target;

    const std::string get_target_begin_tag() const;
    const std::string get_target_initiator_address() const;
    const std::string get_target_end_tag() const;
};

}
}


