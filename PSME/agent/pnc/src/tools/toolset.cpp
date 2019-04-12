/*!
 * @section LICENSE
 *
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
 * @file toolset.cpp
 *
 * @brief Toolset implementation
 * */

#include "tools/toolset.hpp"

using namespace agent::pnc::tools;

Toolset::Toolset(const Toolset& rhs) {
    model_tool = rhs.model_tool;
    i2c_tool = rhs.i2c_tool;
    gas_tool = rhs.gas_tool;
    map_tool = rhs.map_tool;
    oob_fpga_tool = rhs.oob_fpga_tool;
}

Toolset::~Toolset() {}

Toolset Toolset::get() {
    Toolset t{};
    t.model_tool = std::make_shared<ModelTool>();
    t.i2c_tool = std::make_shared<I2cTool>();
    t.gas_tool = std::make_shared<GasTool>();
    t.map_tool = std::make_shared<MapTool>();
    t.oob_fpga_tool = std::make_shared<OobFpgaTool>();
    return t;
}
