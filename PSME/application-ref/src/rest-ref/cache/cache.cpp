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
 * */

#include "logger/logger_factory.hpp"
#include "psme/rest-ref/cache/cache.hpp"

using namespace psme::rest;

void Cache::set(const std::string& link, const std::string& data) {
    std::lock_guard<std::mutex> lock{m_mutex};
    m_data.emplace(link, data);
}

std::string Cache::get(const std::string& link) const {
    std::lock_guard<std::mutex> lock{m_mutex};
    auto it = m_data.find(link);
    if (it != m_data.cend()) {
        return it->second;
    }
    return "";
}

void Cache::set_dirty(const std::string& link) {
    std::lock_guard<std::mutex> lock{m_mutex};
    for (auto it = m_data.begin(); it != m_data.end(); ) {
        if (!it->first.compare(0, link.length(), link)) {
            it = m_data.erase(it);
        } else {
            ++it;
        }
    }
}

bool Cache::exist(const std::string& link) {
    std::lock_guard<std::mutex> lock{m_mutex};
    return m_data.find(link) != m_data.cend();
}

