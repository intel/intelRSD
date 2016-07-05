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
 * */

#ifndef PSME_HANDLER_HELPERS_HPP
#define PSME_HANDLER_HELPERS_HPP

namespace psme {
namespace rest {
namespace model {
namespace handler {
namespace helpers {

/*!
 * @brief Function template compares two sets of uuid and creates vectors of mappings to add and to remove
 * @param fetched List of fetched uuids
 * @param parent_uuid parent uuid
 * @param many2many manager that holds mappings
 * @param out_to_add uuids to add will be added to this vector
 * @param out_to_remove uuids to remove will be added to this vector
 * */
template<typename T>
void what_to_add_what_to_remove(agent_framework::model::attribute::Array<agent_framework::model::attribute::SubcomponentEntry> fetched,
    const std::string &parent_uuid,
    T& many2many,
    std::vector<std::string>& out_to_add,
    std::vector<std::string>& out_to_remove) {

    auto previous = many2many.get_children(parent_uuid);
    auto new_set = std::set<std::string>();
    for (const auto& f : fetched) {
        new_set.insert(f.get_subcomponent());
    }
    auto old_set = std::set<std::string>(previous.begin(), previous.end());

    out_to_add.resize(new_set.size());
    out_to_remove.resize(old_set.size());

    auto it_add = std::set_difference(new_set.begin(), new_set.end(),
                                      old_set.begin(), old_set.end(),
                                      out_to_add.begin());
    out_to_add.resize(static_cast<size_t>(it_add - out_to_add.begin()));

    auto it_rem = std::set_difference(old_set.begin(), old_set.end(),
                                      new_set.begin(), new_set.end(),
                                      out_to_remove.begin());
    out_to_remove.resize(static_cast<size_t>(it_rem - out_to_remove.begin()));
}

}
}
}
}
}

#endif //PSME_HANDLER_HELPERS_HPP
