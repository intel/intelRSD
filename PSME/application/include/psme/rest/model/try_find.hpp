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
 * @file try_find.hpp
 *
 * @brief Definition of the find API. Does not throw exceptions.
 * */


#pragma once



#include "psme/rest/endpoints/utils.hpp"
#include "psme/rest/constants/constants_templates.hpp"
#include "generic/last_template_type.hpp"

#include <string>
#include <sstream>



namespace psme {
namespace rest {
namespace model {

/*!
 * @brief Helper struct for find function that stores state of the try_find function. Usage of this class does not
 * throw exceptions.
 *
 * TryFindStateObject is designed to be the result of find function. Its purpose is to store data
 * between each find steps and to retrieve find results.
 *
 * For example:
 *     TryFindStateObject f = try_find<Manager, System, Processor>(params);
 *     auto uuid_opt = f.get_uuid();
 *
 * @tparam M type of the object which has to be found.
 * */
template<typename M>
struct TryFindStateObject {
public:

    template<typename T, typename ... Tn>
    friend
    struct TryFindImpl;


    /*!
    * @brief Transforms TryFindStateObject object into single resource.
    *
    * @return ObjReference object for found resource type M.
    */
    OptionalField<agent_framework::generic::ObjReference<M, std::recursive_mutex>> get_one() const {
        if (!has_already_failed()) {
            auto& manager = agent_framework::module::get_manager<M>();
            try {
                return manager.get_entry_reference(m_wanted_uuid);
            }
            catch (agent_framework::exceptions::InvalidUuid&) {
                find_failed = true;
            }
        }
        return OptionalField<agent_framework::generic::ObjReference<M, std::recursive_mutex>>();
    }


    /*!
     * @brief returns the found model object
     *
     * @return a copy of the found M type object
     * */
    OptionalField<M> get() const {
        if (!has_already_failed()) {
            const auto& manager = agent_framework::module::get_manager<M>();
            try {
                return manager.get_entry(m_wanted_uuid);
            }
            catch (agent_framework::exceptions::InvalidUuid&) {
                find_failed = true;
            }
        }
        return OptionalField<M>();
    }


    /*!
     * @brief returns the found object's uuid
     *
     * @return a the found M type object's uuid
     * */
    OptionalField<Uuid> get_uuid() const {
        if (!has_already_failed()) {
            return OptionalField<Uuid>(m_wanted_uuid);
        }
        return OptionalField<Uuid>();
    }


    /*!
     * @brief Using this method one can convert TryFindStateObject object to bool type
     * based on find results.
     *
     * For example:
     *     bool flag = static_cast<bool>(
     *          try_find<Manager, Processor>(params));
     *
     * @return true if this object found any required Resource, false if didn't found it.
     * */
    operator bool() const {
        return !find_failed;
    }


private:
    std::uint64_t m_id{};
    std::string m_parent_uuid{};
    std::string m_wanted_uuid{};
    mutable bool find_failed = false;


    /*!
     * @brief tells whether resource search has failed during previous search path on this object.
     *
     * @return true if in any search stage this object failed to search given resource, false if not failed so far.
     * */
    bool has_already_failed() const {
        return find_failed;
    }

};

/*!
 * @brief Wrapper structure for try_find_impl_method recursive template method.
 *
 * It is used as generic template for further specialization but is also used for last recursive call
 * of the try_find_impl_method.
 *
 * @tparam T TryFindStateObject type - result of the find.
 * @tparam Tn endpoint types of the find path.
 */
template<typename T, typename ... Tn>
struct TryFindImpl {
};

/*!
 * @brief Wrapper structure specialization for try_find_impl_method recursive template method.
 *
 * It is used to resolve template parameter pack and reduce it by 1, performing one find step.
 *
 * @tparam T first type of the endpoint types parameter pack.
 * @tparam Tn remaining types of endpoints.
 */
template<typename T, typename ... Tn>
struct TryFindImpl<TryFindStateObject<typename LastOf<Tn ...>::type>, T, Tn ...> {
    /*!
     * @brief Internal recursive method which performs one step of the find process.
     *
     * Resolves template parameter pack reducing it by 1, and performs one find step.
     *
     * @param find_state TryFindStateObject object exchanged between recursive calls.
     * @param params Parameters object container with endpoints to find within.
     * @return Object of TryFindStateObject type containing results.
     */
    static TryFindStateObject<typename LastOf<Tn ...>::type>
    try_find_impl_method(TryFindStateObject<typename LastOf<Tn ...>::type>& find_state,
                         const server::Parameters& params) {

        if (!find_state.has_already_failed()) {
            try {
                std::uint64_t id = psme::rest::endpoint::utils::id_to_uint64(params[constants::get_resource_id<T>()]);
                find_state.m_parent_uuid = agent_framework::module::get_manager<T>().rest_id_to_uuid(id,
                                                                                                     find_state.m_parent_uuid);
            }
            catch (const agent_framework::exceptions::NotFound&) {
                find_state.find_failed = true;
            }

            return TryFindImpl<TryFindStateObject<typename LastOf<Tn ...>::type>, Tn ...>
                ::try_find_impl_method(find_state, params);
        }
        return find_state;

    }
};

/*!
 * @brief Wrapper structure specialization for try_find_impl_method recursive template method.
 *
 * It is used for last recursive call of the try_find_impl_method.
 *
 * @tparam T last type.
 */
template<typename T>
struct TryFindImpl<TryFindStateObject<typename LastOf<T>::type>, T> {
    /*!
     * @brief Internal method which performs last step of the find process.
     *
     * Performs final find step and returns TryFindStateObject with results.
     *
     * @param find_state TryFindStateObject object exchanged between recursive calls.
     * @param params Parameters object container with endpoints to find within.
     * @return Object of TryFindStateObject type containing results.
     */
    static TryFindStateObject<typename LastOf<T>::type>
    try_find_impl_method(TryFindStateObject<typename LastOf<T>::type>& find_state,
                         const server::Parameters& params) {

        if (!find_state.has_already_failed()) {
            try {
                find_state.m_id = psme::rest::endpoint::utils::id_to_uint64(params[constants::get_resource_id<T>()]);
                auto& manager = agent_framework::module::get_manager<typename LastOf<T>::type>();
                find_state.m_wanted_uuid = manager.rest_id_to_uuid(find_state.m_id, find_state.m_parent_uuid);
            }
            catch (const agent_framework::exceptions::NotFound&) {
                find_state.find_failed = true;
            }
        }
        return find_state;
    }
};


/*!
 * @brief Finds given resource based on the resources types in the template parameter list and does not throw exceptions.
 * Entry point to find API.
 *
 * @tparam args Subsequent types of resources being on the search path. Last type is target to find.
 * @param params Const reference to Parameters object container with endpoints to find within.
 * @return Object of TryFindStateObject type containing results.
 */
template<typename ... Args>
TryFindStateObject<typename LastOf<Args...>::type> try_find(const server::Parameters& params) {
    TryFindStateObject<typename LastOf<Args...>::type> find_state;
    return TryFindImpl<TryFindStateObject<typename LastOf<Args...>::type>, Args ...>
    ::try_find_impl_method(find_state, params);
}

}
}
}

