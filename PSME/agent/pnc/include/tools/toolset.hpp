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
 * @file toolset.hpp
 *
 * @brief Toolset class interface
 * */

#pragma once

#include "tools/i2c_tool.hpp"
#include "tools/model_tool.hpp"
#include "tools/gas_tool.hpp"
#include "tools/map_tool.hpp"
#include "tools/fpga_oob/oob_fpga_tool.hpp"

namespace agent {
namespace pnc {
namespace tools {

/*! Toolset class, set of all tools */
class Toolset final {
public:

    /*! Default constructor */
    Toolset() {}

    /*! Copy constructor */
    Toolset(const Toolset& rhs);

    /*! Default destructor */
    ~Toolset();

    /*!
     * @brief Gets a default toolset used in the agent
     * @return Default toolset object
     * */
    static Toolset get();

    ModelToolPtr model_tool{};
    I2cToolPtr i2c_tool{};
    GasToolPtr gas_tool{};
    MapToolPtr map_tool{};
    OobFpgaToolPtr oob_fpga_tool{};
};

}
}
}
