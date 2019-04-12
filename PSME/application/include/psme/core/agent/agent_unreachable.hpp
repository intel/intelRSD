/*!
 * @copyright
 * Copyright (c) 2015-2019 Intel Corporation
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
 * @file agent_unreachable.hpp
 *
 * @brief Declaration of Agent Unreachable Exception.
 * */

#pragma once



#include <string>
#include <exception>



namespace psme {
namespace core {
namespace agent {

/*!
 * @brief Agent Unreachable Exception.
 * */
class AgentUnreachable : public std::exception {
public:

    /*!
     * @brief Constructor
     *
     * @param gami_id               Unreachable agent's GAMI ID
     */
    explicit AgentUnreachable(const std::string& gami_id);


    /*! @brief Default copy constructor.  */
    AgentUnreachable(const AgentUnreachable&) = default;


    /*! @brief Assignment operator */
    AgentUnreachable& operator=(const AgentUnreachable&) = default;


    /*! @brief Destructor */
    virtual ~AgentUnreachable() noexcept;


    /*!
     * @brief Gets Agent's GAMI ID.
     *
     * @return GAMI ID of unreachable agent.
     * */
    const std::string& get_gami_id() const;


    /*!
     * @brief Returns a C-style character string describing the general cause of
     * the current error.
     *
     * @return Description of the current error
     * */
    virtual const char* what() const noexcept;


private:
    /* Prepares m_what field returned on what() method call */
    void make_what();


    const std::string m_gami_id;
    std::string m_what{};
};

}
}
}
