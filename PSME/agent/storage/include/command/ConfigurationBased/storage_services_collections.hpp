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
 * */
#ifndef PSME_STORAGE_COMMAND_STORAGE_SERVICES_COLLECTIONS_HPP
#define PSME_STORAGE_COMMAND_STORAGE_SERVICES_COLLECTIONS_HPP

/*! Storage services collections name and types */
class StorageServicesCollections {
public:

    static constexpr const char PHYSICAL_DRIVES_NAME[] = "PhysicalDrives";
    static constexpr const char PHYSICAL_DRIVES_TYPE[] = "PhysicalDrives";
    static constexpr const char LOGICAL_DRIVES_NAME[] = "LogicalDrives";
    static constexpr const char LOGICAL_DRIVES_TYPE[] = "LogicalDrives";
    static constexpr const char ISCSI_TARGETS_NAME[] = "iSCSITargets";
    static constexpr const char ISCSI_TARGETS_TYPE[] = "iSCSITargets";
    static constexpr const char STORAGE_SERVICES_NAME[] = "StorageServices";
    static constexpr const char STORAGE_SERVICES_TYPE[] = "StorageServices";

};

#endif /* PSME_STORAGE_COMMAND_STORAGE_SERVICES_COLLECTIONS_HPP */
