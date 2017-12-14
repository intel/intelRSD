/*!
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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
 * @file status.hpp
 * @brief Status
 * */

#pragma once

#include "agent-framework/module/enum/common.hpp"
#include "agent-framework/module/utils/utils.hpp"
#include "json-wrapper/json-wrapper.hpp"

#include <string>

namespace agent_framework {
namespace model {
namespace attribute {


/*! Storage  status */
class Status {
public:

    explicit Status();

    Status(const enums::State state, const OptionalField<enums::Health>& health);

    Status(const std::string& state, const std::string& health);

    Status(const bool ok);

    ~Status();

    /*! Enable copy */
    Status(const Status&) = default;
    Status& operator=(const Status&) = default;
    Status(Status&&) = default;
    Status& operator=(Status&&) = default;

    /*!
     * Gets health
     * */
    const enums::State& get_state() const {
        return m_state;
    }

    void set_state(const enums::State& state) {
        m_state = state;
    }

    /*!
     * Gets health
     * */
    const OptionalField<enums::Health>& get_health() const {
        return m_health;
    }

    void set_health(const OptionalField<enums::Health>& health) {
        m_health = health;
    }

    /*
     * Converts this object to json representation
     * @return Json representation of this object
     * */
    json::Json to_json() const;

    /*!
     * @brief construct an object of class Status from JSON
     *
     * @param json the json::Json deserialized to object
     *
     * @return the newly constructed Status object
     */
    static Status from_json(const json::Json& json);

private:
    enums::State m_state{enums::State::Disabled};
    OptionalField<enums::Health> m_health{enums::Health::OK};
};

}
}
}
