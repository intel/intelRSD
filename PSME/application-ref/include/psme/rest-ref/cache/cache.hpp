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

#ifndef APP_REST_CACHE_HPP
#define APP_REST_CACHE_HPP

#include <map>
#include <mutex>

namespace psme {
namespace rest {

class Cache {
public:
    using CacheData = std::map<const std::string, const std::string>;

    void set(const std::string& link, const std::string& data);

    std::string get(const std::string& link) const;

    void set_dirty(const std::string& link);

    bool exist(const std::string& link);

private:
    mutable std::mutex m_mutex{};
    CacheData m_data{};
};

}
}

#endif
