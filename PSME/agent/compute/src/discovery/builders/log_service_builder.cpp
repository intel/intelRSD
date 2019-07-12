/*!
 * @brief Log service builder class implementation.
 *
 * @copyright Copyright (c) 2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file log_service_builder.cpp
 */

#include "discovery/builders/log_service_builder.hpp"



using namespace agent::compute::discovery;
using namespace agent_framework::model;


LogService LogServiceBuilder::build_sel_log(const std::string& parent_uuid) {
    LogService log_service{parent_uuid, enums::Component::Manager};

    log_service.set_overwrite_policy(enums::OverWritePolicy::Unknown);
    log_service.set_service_enabled(true);
    log_service.set_log_entry_type(enums::LogEntryType::SEL);

    log_service.add_collection({enums::CollectionName::LogEntries,
                                enums::CollectionType::LogEntries
    });

    return log_service;
}
