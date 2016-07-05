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
 * @file iscsi_target_dependency_resolver.hpp
 *
 * @brief Resolves dependencies for iscsi targets
 * */

#ifndef ISCSI_TARGET_DEPENDENCY_RESOLVER_HPP
#define	ISCSI_TARGET_DEPENDENCY_RESOLVER_HPP

#include "dependency_resolver.hpp"

using agent_framework::generic::LogicalDrive;

namespace agent {
namespace storage {
namespace discovery {

using LogicalDriveSharedPtr = LogicalDrive::LogicalDriveSharedPtr;
using Target = agent_framework::generic::Target;

/*!
 * @brief Resolves iscsi target dependencies
 */
class IscsiTargetDependencyResolver: public DependencyResolver {
public:
    /*!
     * @brief Copy constructor.
     * @param orig object to be copied.
     */
    IscsiTargetDependencyResolver(const IscsiTargetDependencyResolver& orig) = default;

     /*!
     * @brief Move constructor
     * @param orig object to be moved.
     */
    IscsiTargetDependencyResolver(IscsiTargetDependencyResolver&& orig) = default;

    /*! Assignment operator */
    IscsiTargetDependencyResolver& operator=(const IscsiTargetDependencyResolver&) = default;

    /*!
     * @brief Constructor for Module
     * @param module Module
     */
    IscsiTargetDependencyResolver(Module& module) : DependencyResolver(module) {}

    /*!
     * @brief Optional initialization of resolver
     */
    void initialize() final override;

    /*!
     * @brief Resolves iscsi target dependencies
     */
    void resolve() final override;

private:
    void resolve_iscsi_target(Target::TargetSharedPtr target);
    std::map<std::string, LogicalDriveSharedPtr> m_logical_drive_map{};
};

}
}
}
#endif	/* ISCSI_TARGET_DEPENDENCY_RESOLVER_HPP */

