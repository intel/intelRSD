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


#ifndef APP_REST_MODEL_FINDER_HPP
#define APP_REST_MODEL_FINDER_HPP

#include "agent-framework/module-ref/compute_manager.hpp"
#include "agent-framework/module-ref/network_manager.hpp"
#include "psme/rest-ref/endpoints/utils.hpp"
#include <string>
#include <sstream>

namespace psme {
namespace rest {
namespace model {

/*! Find helper class that simplifies accessing Resource specified by rest http request.
 *  It purpose is to be used in http endpoint code.
 *  For example:
 *    Find<Processor>(req.params["processorID"]).via<Manager>(req.params["managerID"]).
 *      via<System>(req.params["systemID"]).get_one()
 * */
template <typename M>
class Find {
public:
    /*!
     * @brief Constructs Find object that can be used to find resource
     * by id
     *
     * @param id Rest Id of resource to find (this is not uuid)
     */
    Find(const std::string& id) : m_id(psme::rest::endpoint::utils::id_to_uint64(id)) { }

    /*!
     * @brief Using this method one can specify path to resource.
     * For example:
     *    Find<Processor>(req.params["processorID"]).via<Manager>(req.params["managerID"]).
     *      via<System>(req.params["systemID"]).get_one()
     *
     * @param _id Rest Id of indirect resource being on the path of requested resource
     */
    template <typename T>
    Find<M>& via(const std::string& _id) {
        std::uint64_t id = psme::rest::endpoint::utils::id_to_uint64(_id);
        m_parent_uuid = agent_framework::module::get_manager<T>().rest_id_to_uuid(id, m_parent_uuid);
        return *this;
    }

    /*!
     * @brief Transforms Find object into single resource
     *
     */
    agent_framework::generic::ObjReference<M, std::recursive_mutex> get_one() const {
        auto& manager = agent_framework::module::get_manager<M>();
        auto uuid = manager.rest_id_to_uuid(m_id, m_parent_uuid);

        return manager.get_entry_reference(uuid);
    }

    /*!
     * @brief returns the found model object
     *
     * @return a copy of the found M type object
     */
    M get() const {
        auto& manager = agent_framework::module::get_manager<M>();
        auto uuid = manager.rest_id_to_uuid(m_id, m_parent_uuid);

        return manager.get_entry(uuid);
    }

protected:
    std::uint64_t m_id;
    std::string m_parent_uuid = {};
};

}
}
}

#endif //APP_REST_MODEL_FINDER_HPP
