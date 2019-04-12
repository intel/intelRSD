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
 * @file target_parser.hpp
 * @brief TGT TargetParser interface
*/

#pragma once
#include "target_data.hpp"

namespace tgt {

/*! Class represents targets parser */
class TargetParser {
private:
    static const constexpr char ONE_WAY[] = "OneWay";
    static const constexpr char MUTUAL[] = "Mutual";

public:
    TargetParser();


    virtual ~TargetParser();

    /*!
     * @brief Parse tgt target string into target data objects
     * @param text tgtadm target string list
     * @return TargetData object list
     */
    TargetDataCollection parse(const std::string& text);

private:
    class Impl;
    std::unique_ptr<Impl> m_impl;
};

}
