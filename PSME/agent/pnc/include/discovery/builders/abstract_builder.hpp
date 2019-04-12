/*!
 * @copyright
 * Copyright (c) 2016-2019 Intel Corporation
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
 * @file abstract_builder.hpp
 *
 * @brief AbstractBuilder interface
 * */

#pragma once

#include <string>
#include <stdexcept>

namespace agent {
namespace pnc {
namespace discovery {
namespace builders {

class BuilderFactory;

/*!
 * @brief Generic builder template
 * Several things have to be done in each builder classes - initializing with default object, initializing with
 * a provided object and checking if object was initialized (this ensures correct uuid creation). This class allows
 * us not to make unnecessary code copies...
 */
template <typename OBJ_TYPE>
class AbstractBuilder {
public:

    /*!
     * @brief Default destructor.
     * */
    virtual ~AbstractBuilder() {}

    /*!
     * @brief Creates default object
     * @param[in] parent Uuid of the parent of the object to be built
     * */
    void init(const std::string& parent) {
        m_initialized = true;
        m_obj = OBJ_TYPE{parent};
        build_default();
    }

    /*!
     * @brief Inits with an already existing objects
     * @param[in] new_obj New object
     * */
    void init(const OBJ_TYPE& new_obj) {
        m_initialized = true;
        m_obj = new_obj;
    }

    /*!
     * @brief Finishes construction and returns built objects
     * @return Constructed objects
     * */
    OBJ_TYPE build() const {
        if (!m_initialized) {
            throw std::runtime_error("Discovery builder was not initialized properly");
        }
        return m_obj;
    }

protected:
    /*!
     * @brief Performs construction of the default object
     * */
    virtual void build_default() = 0;

    /*! Currently built object - has to be correctly initialized, if not - parent uuid is incorrect */
    OBJ_TYPE m_obj{""};

    /*! True if object was correctly initialized */
    bool m_initialized {false};

};

}
}
}
}
