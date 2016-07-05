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
 */

#include "iscsi/tgt/config/tgt_target_lun_config.hpp"
#include <cstring>
#include <sstream>
#include <exception>

using namespace agent::storage::iscsi::tgt::config;
using namespace std;

constexpr const char* TGT_CONFIG_PROPERTY_LUN = "lun";
constexpr const char* TGT_CONFIG_PROPERTY_DEVICE_TYPE = "backing-store";
constexpr const char* TAB = "\t";
constexpr const char* SPACE = " ";
constexpr const char* LT = "<";
constexpr const char* GT = ">";

const string TgtTargetLunConfig::to_string() const {
    const auto& device_path = m_lun.get_device_path();
    if (device_path.empty()) {
        throw invalid_argument("Device path is empty");
    }
    ostringstream content;
    content << TAB << LT << TGT_CONFIG_PROPERTY_DEVICE_TYPE << SPACE << m_lun.get_device_path() << GT << endl;
    content << TAB << TAB << TGT_CONFIG_PROPERTY_LUN << SPACE << m_lun.get_lun_id() << endl;
    content << TAB << LT << "/" << TGT_CONFIG_PROPERTY_DEVICE_TYPE << GT << endl;
    return content.str();
}
