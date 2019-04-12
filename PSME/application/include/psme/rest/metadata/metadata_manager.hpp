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
 * @file metadata_manager.hpp
 * @brief MetadataManager model interface
 * */

#pragma once

#include "include-generated/generated_headers.hpp"

#include <unordered_map>
#include <string>

namespace psme {
namespace rest {
namespace metadata {

using xml_iterator = std::unordered_map<std::string,std::string>::iterator;

/*! MetadataManager */
class MetadataManager {
private:
    static std::unordered_map<std::string, std::string> xml_map;

public:
    /*!
     * @brief get an xml string which is the content of a given metadata file
     *
     * @param key the name of the metadata file
     *
     * @return the xml string
     */
    static const std::string& get_xml(const std::string& key);
};

} /* metadata */
} /* rest */
} /* psme */

