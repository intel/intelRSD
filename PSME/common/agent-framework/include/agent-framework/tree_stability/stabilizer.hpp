/*!
 * @brief Provides class for one resource tree stabilization based on key generator.
 *
 * @copyright
 * Copyright (c) 2017-2019 Intel Corporation
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
 * @file stabilizer.hpp
 * */

#pragma once



#include "log_helpers.hpp"
#include "key_value_missing_error.hpp"
#include "agent-framework/module/model/resource.hpp"

#include "agent-framework/module/model/model_common.hpp"
#include "agent-framework/module/model/model_storage.hpp"

#include <string>
#include <utility>



namespace agent_framework {

/*!
 * @brief Stabilizer for single resource.
 * @tparam KeyGenerator Type of unique key generator depending on agent's type.
 * */
template<typename KeyGenerator>
class Stabilizer {
public:

    /*! @brief Destructor. */
    virtual ~Stabilizer();


    /*!
     * @brief Stabilize resource without changing model.
     *
     * This method runs dry stabilization to retrieve stable UUID of the resource.
     * Moreover, it operates on the resource copy so the model will be unchanged.
     *
     * @tparam T Resource type.
     * @tparam Args Additional arguments needed by KeyGenerator.
     *
     * @param resource Resource to dry stabilize.
     * @param args Additional arguments needed by KeyGenerator.
     *
     * @return Persistent UUID of the resource.
     * */
    template<class T, class ...Args>
    const Uuid dry_stabilize(T resource, const Args& ...args) const {
        resource.set_unique_key(KeyGenerator{}.generate_key(resource, args...));
        resource.make_persistent_uuid(); /* Persistent UUID for a copy */
        return resource.get_uuid();
    }


    /*!
     * @brief Stabilize resource object.
     *
     * @tparam T Resource type.
     * @tparam Args Additional arguments needed by KeyGenerator.
     *
     * @param resource Resource to stabilize.
     * @param args Additional arguments needed by KeyGenerator.
     *
     * @return Persistent UUID of the resource.
     * */
    template<class T, class ... Args>
    const Uuid stabilize(T& resource, const Args& ...args) const {

        Uuid resource_temporary_uuid = resource.get_uuid();
        try {
            resource.set_unique_key(KeyGenerator{}.generate_key(resource, args...));

            resource.make_persistent_uuid();
            Uuid resource_persistent_uuid = resource.get_uuid();
            log_persistent_uuid_generated(T::get_component().to_string(),
                                          resource_temporary_uuid,
                                          resource_persistent_uuid);

            return resource_persistent_uuid;
        }
        catch (const KeyValueMissingError& ex) {
            log_key_value_missing(T::get_component().to_string(), resource_temporary_uuid, ex);
            return resource_temporary_uuid;
        }
    }

};


template<typename KeyGenerator>
Stabilizer<KeyGenerator>::~Stabilizer() {}

}
