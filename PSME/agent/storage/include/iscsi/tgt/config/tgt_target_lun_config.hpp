/*!
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
 * @file tgt_target_lun_config.hpp
 * @brief tgt target lun configuration
*/

#ifndef ISCSI_TGT_TARGET_LUN_CONFIG_HPP
#define	ISCSI_TGT_TARGET_LUN_CONFIG_HPP

#include "agent-framework/module/target.hpp"
#include <cstdint>
#include <string>

namespace agent {
namespace storage {
namespace iscsi {
namespace tgt {
namespace config {

using namespace agent_framework::generic;

/*! Represents TGT target lun configuration */
class TgtTargetLunConfig {
public:

    /*! Default constructor*/
    TgtTargetLunConfig(const Target::Lun& lun) : m_lun(lun) {
    }

    /*! Disable copy constructor*/
    TgtTargetLunConfig(const TgtTargetLunConfig&) = delete;

    /*! Disable assignment operator */
    TgtTargetLunConfig& operator=(const TgtTargetLunConfig&) = delete;

    /*! Disable move */
    TgtTargetLunConfig& operator=(const TgtTargetLunConfig&&) = delete;

    /*!
     * @brief Returns target configuration string
     *
     * @return target configuration string
     */
    const std::string to_string() const;

private:
    Target::Lun m_lun;
};

}
}
}
}
}

#endif	/* ISCSI_TGT_TARGET_LUN_CONFIG_HPP */

