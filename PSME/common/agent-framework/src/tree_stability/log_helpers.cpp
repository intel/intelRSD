/*!
 * @copyright
 * Copyright (c) 2017-2019 Intel Corporation
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
 * @file log_helpers.cpp
 * */

#include "agent-framework/tree_stability/log_helpers.hpp"



namespace agent_framework {

void log_persistent_uuid_generated(const std::string& resource_name,
                                   const Uuid& temporary_uuid,
                                   const Uuid& persistent_uuid) {
    log_debug("tree-stability",
              "Persistent UUID " << persistent_uuid << " generated for " << resource_name << " " << temporary_uuid);
}


void log_key_value_missing(const std::string& resource_name,
                           const Uuid& temporary_uuid) {
    log_error("tree-stability",
              "Persistent UUID for " << resource_name << " with temporary UUID "
                                     << temporary_uuid << " could not be generated");
}


void log_key_value_missing(const std::string& resource_name,
                           const Uuid& temporary_uuid,
                           const std::exception& e) {
    log_error("tree-stability",
              "Persistent UUID for " << resource_name << " with temporary UUID "
                                     << temporary_uuid << " could not be generated");
    log_debug("tree-stability",
              "Persistent UUID for " << resource_name << " with temporary UUID "
                                     << temporary_uuid << " could not be generated: " << e.what());
}

}
