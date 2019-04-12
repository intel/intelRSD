/*!
 * @brief Contains Intel RackScale registry configuration
 *
 * @copyright
 * Copyright (c) 2019 Intel Corporation
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
 * @file intel_rackscale_configuration.hpp
 * */

#pragma once

#include <string>

namespace psme {
namespace rest {
namespace registries {

/*!
 * Make configuration string for Intel RackScale message registry.
 *
 * @return String containing a json object with Intel RackScale registry configuration.
 * */
static std::string make_intel_rackscale_configuration() {
    static std::string intel_rackscale_configuration = R"({
        "MessageRegistryFiles": [
            {
                "Name" : "Intel RackScale Message Registry File",
                "Description" : "Message Registry File for Intel RackScale Message Registry",
                "Registry" : "Intel_RackScale.1.0.0",
                "MessageRegistries" : [
                    {
                        "Language" : "en",
                        "Type" : "external",
                        "PublicationUri" : null,
                        "Uri" : "/registries/Intel_RackScale.1.0.0.json",
                        "ArchiveUri" : null,
                        "ArchiveFile" : null
                    }
                ],
                "Oem" : {}
            }
        ]
    })";

    return intel_rackscale_configuration;
}

}
}
}
