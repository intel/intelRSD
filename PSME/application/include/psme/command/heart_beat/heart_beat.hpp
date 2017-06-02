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
 * @file command/heart_beat/heart_beat.hpp
 *
 * @brief Heart beat command.
 * */

#pragma once
#include "psme/command/heart_beat/tag.hpp"
#include "psme/command/command.hpp"

#include <string>
#include <chrono>

namespace psme {
namespace command {
namespace heart_beat {

/* Forward declaration */
namespace json { class HeartBeat; }

using std::string;

/*! Command HeartBeat */
class HeartBeat : public Command {
public:
    class Request;
    class Response;

    /*! Time live in seconds */
    using TimeLive = std::uint64_t;
    using Seconds = std::chrono::seconds;

    /*! Tag string for identify HeartBeat command */
    static const char* TAG;

    /*!
     * @brief Create generic command HeartBeat for monitoring agents/server
     *
     * @param[in]   implementation  Set user tag string to identify particular
     *                              command implementation
     * */
    HeartBeat(const string& implementation)
        : Command(implementation, heart_beat::TAG, HeartBeat::TAG) { }

    /*!
     * @brief Execute command with given request and response argument
     *
     * @param[in]   request     Input request argument
     * @param[out]  response    Output response argument
     * */
    virtual void execute(const Request& request, Response& response) = 0;

    /*! Command destructor */
    virtual ~HeartBeat();
protected:
    /*!
     * @brief Execute command with givent command arguments
     *
     * @param[in]   in      Input command argument
     * @param[out]  out     Output command argument
     * */
    void execute(const Argument& in, Argument& out) override final {
        execute(static_cast<const Request&>(in), static_cast<Response&>(out));
    }
public:
    /*! Argument request to execute */
    class Request : public Argument {
    public:
        ~Request();

        /*!
         * @brief Get agent identifier from request
         * @return agent identifier
         * */
        const std::string& get_gami_id() const {
            return m_gami_id;
        }

    private:
        std::string m_gami_id{};
        friend class json::HeartBeat;
    };

    /*! Argument response from execute */
    class Response : public Argument {
    public:
        void set_time_live(const TimeLive& time_live) {
            m_time_live = time_live;
        }

        void set_minimum_delay(const Seconds& seconds) {
            m_minimum_delay = seconds;
        }

        ~Response();
    private:
        friend class json::HeartBeat;
        TimeLive m_time_live{0};
        Seconds m_minimum_delay{0};
    };

};

}
}
}

