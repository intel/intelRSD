/*!
 * @brief Implements non-abstract methods of TreeStabilizer class
 *
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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
 * @file tree_stabilizer.cpp
 * */



#include "agent-framework/tree_stability/tree_stabilizer.hpp"

#include "agent-framework/logger_ext.hpp"



using namespace agent_framework;


TreeStabilizer::~TreeStabilizer() { }


void TreeStabilizer::log_persistent_uuid_generated(const std::string& resource_name,
                                                   const std::string& temporary_uuid,
                                                   const std::string persistent_uuid) const {
    log_debug(LOGUSR,
              "Persistent UUID " << persistent_uuid << " generated for " << resource_name << " " << temporary_uuid);
}


void TreeStabilizer::log_key_value_missing(const std::string& resource_name,
                                           const std::string& temporary_uuid) const {
    log_error(LOGUSR,
              "Persistent UUID for " << resource_name << " with temporary UUID " << temporary_uuid <<
              " could not be generated");
}
