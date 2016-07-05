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
 * @file storage_dependency_resolver.hpp
 *
 * @brief Resolves storage dependencies
 * */

#ifndef STORAGE_DEPENDENCY_RESOLVER_HPP
#define	STORAGE_DEPENDENCY_RESOLVER_HPP

#include "logical_drive_dependency_resolver.hpp"
#include "iscsi_target_dependency_resolver.hpp"

namespace agent {
namespace storage {
namespace discovery {

using DependencyResolverSharedPtr = std::shared_ptr<DependencyResolver>;

/*!
 * @brief Resolves logical drive and iscsi target dependencies
 */
class StorageDependencyResolver: public DependencyResolver {
public:

    /*!
     * @brief Constructor for Module
     * @param module Module
     */
    StorageDependencyResolver(Module& module) : DependencyResolver(module) {}

    /*!
     * @brief Resolves logical drive and iscsi target dependencies
     */
    void resolve() final override;

private:
    std::vector<DependencyResolverSharedPtr> m_dependency_resolvers {
                std::make_shared<LogicalDriveDependencyResolver>(m_module),
                std::make_shared<IscsiTargetDependencyResolver>(m_module)
    };
};

}
}
}
#endif	/* STORAGE_DEPENDENCY_RESOLVER_HPP */

