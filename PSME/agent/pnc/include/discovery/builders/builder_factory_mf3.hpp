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
 * @file builder_factory_mf3.hpp
 *
 * @brief Builder factory interface for MF3
 * */

#pragma once

#include "discovery/builders/builder_factory.hpp"
#include "discovery/builders/switch_builder_mf3.hpp"
#include "discovery/builders/chassis_builder_mf3.hpp"

namespace agent {
namespace pnc {
namespace discovery {
namespace builders {

/*!
 * @brief Builder factory class
 * */
class BuilderFactoryMf3 : public BuilderFactory {
public:

    /*! Default constructor */
    BuilderFactoryMf3() = default;

    /*! Default destructor */
    virtual ~BuilderFactoryMf3();

    /*! Enable copy */
    BuilderFactoryMf3(const BuilderFactoryMf3&) = default;
    BuilderFactoryMf3& operator=(const BuilderFactoryMf3&) = default;

    /*!
     * @brief Returns switch builder object
     * */
    virtual std::shared_ptr<SwitchBuilder> get_switch_builder() override {
        return std::shared_ptr<SwitchBuilder>(new SwitchBuilderMf3());
    }

    /*!
    * @brief Returns chassis builder object
    * */
    virtual std::shared_ptr<ChassisBuilder> get_chassis_builder() override {
        return std::shared_ptr<ChassisBuilder>(new ChassisBuilderMf3());
    }
};

}
}
}
}
