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
 * @file tgt_target_lun_config.hpp
 * @brief tgt target lun configuration
*/

#pragma once



#include "tgt/lun_data.hpp"

#include <cstdint>
#include <string>

namespace tgt {
namespace config {

/*! Represents TGT target lun configuration */
class TgtTargetLunConfig {
public:

    /*! Default constructor*/
    TgtTargetLunConfig(const LunData& lun) : m_lun(lun) {
    }

    /*! Disable copy constructor*/
    TgtTargetLunConfig(const TgtTargetLunConfig&) = delete;

    /*! Disable assignment operator */
    TgtTargetLunConfig& operator=(const TgtTargetLunConfig&) = delete;

    /*! Disable move */
    TgtTargetLunConfig& operator=(const TgtTargetLunConfig&&) = delete;

    /*!
     * @brief Returns target configuration string
     * @return target configuration string
     */
    const std::string to_string() const;

private:
    LunData m_lun;
};

}
}


