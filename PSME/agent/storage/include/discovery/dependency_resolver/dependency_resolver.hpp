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
 * @file dependency_resolver.hpp
 *
 * @brief Base class for dependency resolvers
 * */

#ifndef DEPENDENCY_RESOLVER_HPP
#define	DEPENDENCY_RESOLVER_HPP

#include "agent-framework/module/module.hpp"

using agent_framework::generic::Module;

namespace agent {
namespace storage {
namespace discovery {

/*!
 * @brief Base class for dependency resolver
 */
class DependencyResolver {
public:
     /*!
     * @brief Copy constructor.
     * @param orig object to be copied.
     */
    DependencyResolver(const DependencyResolver& orig) = default;

     /*!
     * @brief Move constructor
     * @param orig object to be moved.
     */
    DependencyResolver(DependencyResolver&& orig) = default;

    /*! Assignment operator */
    DependencyResolver& operator=(const DependencyResolver&) = default;

    /*!
     * @brief Constructor for Module
     * @param module Module
     */
    DependencyResolver(Module& module) : m_module(module) {}

    /*!
     * @brief Optional initialization of resolver
     */
    virtual void initialize();

    /*!
     * @brief Resolves dependencies
     */
    virtual void resolve() = 0;

    /*!
     * @brief Default destructor.
     */
    virtual ~DependencyResolver();

protected:
    Module& m_module;
};

}
}
}
#endif	/* DEPENDENCY_RESOLVER_HPP */

