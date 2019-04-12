/*!
 * @copyright
 * Copyright (c) 2018-2019 Intel Corporation
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
 * @file lun_data.hpp
 * @brief Target LUN data declaration
 */
#pragma once



#include <string>
#include <vector>
#include <memory>



namespace tgt {

/*! Class represents tgt lun model data */
class LunData {
public:
    using Id = std::uint64_t;


    /*!
     * @brief Set lun id
     * @param lun Lun id
     */
    void set_lun(const Id lun);


    /*!
     * @brief Get lun id
     * @return Lun id
     */
    Id get_lun() const;


    /*!
     * @brief Set lun device path
     * @param device_path Lun device path
     */
    void set_device_path(const std::string& device_path);


    /*!
     * @brief Get lun device path
     * @return Lun device path
     */
    const std::string& get_device_path() const;


private:
    Id m_lun{};
    std::string m_device_path{};
};

using LunDataCollection = std::vector<std::shared_ptr<LunData>>;

}
