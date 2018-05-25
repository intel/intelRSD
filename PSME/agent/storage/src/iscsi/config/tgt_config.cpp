/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2018 Intel Corporation
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

#include "iscsi/tgt/config/tgt_config.hpp"
#include "iscsi/tgt/config/tgt_target_config.hpp"

#include "agent-framework/module/managers/iscsi_target_manager.hpp"
#include "agent-framework/module/storage_components.hpp"
#include <fstream>

using namespace agent::storage::iscsi::tgt::config;
using namespace agent_framework::model;
using namespace agent_framework::module;
using namespace agent_framework::module::managers;
using namespace std;

constexpr const char* TGT_TARGET_CONF_EXTENSION = ".conf";
constexpr const char* TGT_TARGET_CONF_PATH = "/etc/tgt/conf.d/";


void TgtConfig::add_target(const IscsiTarget& target) const {

    TgtTargetConfig tgtTargetConfig(target);

    auto target_conf_file_name = get_target_conf_file_name(target.get_target_iqn());
    log_info("tgt", "Add TGT target config file: " + target_conf_file_name);

    ofstream targetConfigFile;
    const auto& content = tgtTargetConfig.to_string();

    targetConfigFile.open(target_conf_file_name.c_str(), ios_base::out);
    if (targetConfigFile.is_open()) {
        targetConfigFile << content;
        targetConfigFile.close();
    }
    else {
        throw runtime_error("Error opening file for writing: " + target_conf_file_name);
    }
}


void TgtConfig::remove_target(const string& target_iqn) const {
    auto target_conf_file_name = get_target_conf_file_name(target_iqn);
    log_info("tgt", "Remove TGT target config file: " +
            target_conf_file_name);
    if (0 != remove(target_conf_file_name.c_str())) {
        throw runtime_error("Error removing file: " + target_conf_file_name);
    }
}


string TgtConfig::get_target_conf_file_name(const string& target_iqn) const {
    string configuration_path = m_configuration_path;
    if (configuration_path.empty()) {
        log_warning("tgt", "TGT conf-path is empty. Using default path: " <<
                TGT_TARGET_CONF_PATH);
        configuration_path = TGT_TARGET_CONF_PATH;
    }
    if ('/' != configuration_path.at(configuration_path.size() - 1)) {
        configuration_path += '/';
    }
    return configuration_path + target_iqn + TGT_TARGET_CONF_EXTENSION;
}
