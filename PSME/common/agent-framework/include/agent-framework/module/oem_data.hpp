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
 * @file module/oem_data.hpp
 * @brief OEM specific data used in commands response
 * */

#ifndef AGENT_FRAMEWORK_MODULE_OEM_DATA_HPP
#define AGENT_FRAMEWORK_MODULE_OEM_DATA_HPP

#include <jsonrpccpp/server.h>

#include <string>
#include <memory>

namespace agent_framework {
namespace generic {

using std::string;
using std::unique_ptr;

/*! Class representing oem attribute
 *  for commands response */
class OEMData {
public:
    /*! Unique pointer to OEMData definition */
    using OEMDataConstUniquePtr = unique_ptr<const OEMData>;

    /*!
     * @brief Convert OEMData to JSON object
     * @return JSON representation of OEMData object
     * */
    virtual Json::Value to_json() const;

    /*! OEMData constructors */
    OEMData() = default;
    OEMData(const OEMData&) = default;
    OEMData& operator=(const OEMData&) = default;

    /*! OEMData virtual destructor */
    virtual ~OEMData();
};

}
}

#endif /* AGENT_FRAMEWORK_MODULE_OEM_DATA_HPP */
