/*!
 * @copyright
 * Copyright (c) 2016-2019 Intel Corporation
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
 * @file storage_subsystem_builder.hpp
 *
 * @brief StorageSubsystemBuilder interface
 * */

#pragma once

#include "discovery/builders/abstract_builder.hpp"
#include "agent-framework/module/common_components.hpp"

namespace agent {
namespace pnc {
namespace discovery {
namespace builders {

/*!
 * @brief Builder used to create storage subsystem objects
 */
class StorageSubsystemBuilder : public AbstractBuilder<agent_framework::model::StorageSubsystem> {

public:

    using ReturnType = StorageSubsystemBuilder&;

    StorageSubsystemBuilder() = default;
    ~StorageSubsystemBuilder() = default;

protected:
    /*!
     * @brief Performs construction of the default object
     * */
    virtual void build_default() override;
};

}
}
}
}
