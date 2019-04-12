/*!
 * @brief Definition of AddVolume response class.
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file add_volume.hpp
 */

#pragma once



#include "agent-framework/module/utils/uuid.hpp"
#include "agent-framework/module/model/attributes/oem.hpp"
#include "agent-framework/module/constants/command.hpp"
#include "json-wrapper/json-wrapper.hpp"

#include <string>



namespace agent_framework {
namespace model {
namespace responses {

/*! GAMI AddVolume response */
class AddVolume {
public:

    explicit AddVolume(const Uuid& volume = {},
                       const Uuid& task = {},
                       const attribute::Oem& oem = attribute::Oem{});


    static std::string get_command() {
        return literals::Command::ADD_VOLUME;
    }


    /*!
     * @brief Get volume UUID from response
     * @return Volume UUID
     * */
    const Uuid& get_volume() const {
        return m_volume;
    }


    /*!
     * @brief Set volume UUID in the response
     * @param[in] volume Volume UUID
     * */
    void set_volume(const Uuid& volume) {
        m_volume = volume;
    }


    /*!
     * @brief Set task UUID
     * @param[in] task Task UUID
     * */
    void set_task(const Uuid& task) {
        m_task = task;
    }


    /*!
     * @brief Get task UUID
     * @return Task UUID
     * */
    const Uuid& get_task() const {
        return m_task;
    }


    /*!
     * @brief Transform response to JSON
     * @return Created JSON value
     */
    json::Json to_json() const;


    /*!
     * @brief Create AddVolume from JSON
     *
     * @param[in] json The input argument
     *
     * @return new AddVolume response
     */
    static AddVolume from_json(const json::Json& json);


private:
    Uuid m_volume{};
    Uuid m_task{};
    attribute::Oem m_oem{};
};

}
}
}
