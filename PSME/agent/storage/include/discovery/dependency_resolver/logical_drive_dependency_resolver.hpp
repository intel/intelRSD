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
 *
 * @file logical_drive_dependency_resolver.hpp
 *
 * @brief Resolves dependencies for logical drives
 * */

#ifndef LOGICAL_DRIVE_DEPENDENCY_RESOLVER_HPP
#define	LOGICAL_DRIVE_DEPENDENCY_RESOLVER_HPP

#include "agent-framework/module/hard_drive.hpp"
#include "agent-framework/module/logical_drive.hpp"
#include "dependency_resolver.hpp"

using agent_framework::generic::HardDrive;
using agent_framework::generic::LogicalDrive;

namespace agent {
namespace storage {
namespace discovery {

using LogicalDriveSharedPtr = LogicalDrive::LogicalDriveSharedPtr;
using HardDriveWeakPtr = HardDrive::HardDriveWeakPtr;

/*!
 * @brief Resolves logical drive dependencies
 */
class LogicalDriveDependencyResolver: public DependencyResolver {
public:
    /*!
     * @brief Copy constructor.
     * @param orig object to be copied.
     */
    LogicalDriveDependencyResolver(const LogicalDriveDependencyResolver& orig) = default;

   /*!
     * @brief Move constructor
     * @param orig object to be moved.
     */
    LogicalDriveDependencyResolver(LogicalDriveDependencyResolver&& orig) = default;

    /*! Assignment operator */
    LogicalDriveDependencyResolver& operator=(const LogicalDriveDependencyResolver&) = default;

    /*!
     * @brief Constructor for Module
     * @param module Module
     */
    LogicalDriveDependencyResolver(Module& module) : DependencyResolver(module) {}

    /*!
     * @brief Optional initialization of resolver
     */
    void initialize() final override;

    /*!
     * @brief Resolves logical drive dependencies
     */
    void resolve() final override;

private:
    void resolve_physical_volume(LogicalDriveSharedPtr logical_drive);
    void resolve_physical_volume_hard_disk(LogicalDriveSharedPtr logical_drive);
    void resolve_volume_group(LogicalDriveSharedPtr logical_drive);
    std::map<std::string, HardDriveWeakPtr> m_hard_drive_map{};
};

}
}
}
#endif	/* LOGICAL_DRIVE_DEPENDENCY_RESOLVER_HPP */

