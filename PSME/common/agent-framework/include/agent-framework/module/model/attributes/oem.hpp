/*!
 * @copyright
 * Copyright (c) 2015-2019 Intel Corporation
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
 * @brief OEM specific data used in commands response
 * */

#pragma once

#include "json-wrapper/json-wrapper.hpp"

namespace agent_framework {
namespace model {
namespace attribute {

/*! Class representing oem attribute
 *  for commands response */
class Oem {
public:

    explicit Oem();
    ~Oem();

    /*! Enable copy */
    Oem(const Oem&) = default;
    Oem& operator=(const Oem&) = default;
    Oem(Oem&&) = default;
    Oem& operator=(Oem&&) = default;

    /*!
     * @brief transform the object to JSon
     *
     * @return the object serialized to json::Json
     */
    json::Json to_json() const;

    /*!
	 * @brief construct an object of class Oem from JSON
	 *
	 * @param json the json::Json deserialized to object
	 *
	 * @return the newly constructed Oem object
	 */
	static Oem from_json(const json::Json& json);

};

}
}
}
