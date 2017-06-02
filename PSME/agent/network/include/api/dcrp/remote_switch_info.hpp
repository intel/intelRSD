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
 */

#pragma once

#include "api/remote_switch_info.hpp"

#include <memory>
#include <string>
#include <vector>

using std::string;
using std::vector;

namespace agent {
namespace network {
namespace api {
namespace dcrp {

/*!
 * @brief dcrp switch Info.
 * */
class RemoteSwitchInfo : public api::RemoteSwitchInfo {
public:
    /*!
     * @brief Default constructor.
     * */
    RemoteSwitchInfo() {}

    /*!
     * @brief Default constructor.
     *
     * @param[in] uuid Switch UUID.
     * */
    RemoteSwitchInfo(const std::string& uuid);

    /*!
     * @brief Copy constructor.
     *
     * @param[in] orig object to be copied.
     * */
    RemoteSwitchInfo(const RemoteSwitchInfo& orig) = delete;

    /*!
     * @brief Copy operator.
     *
     * @param[in] orig object to be copied.
     * */
    RemoteSwitchInfo& operator=(const RemoteSwitchInfo& orig) = delete;

    /*!
     * @brief Destructor.
     * */
    virtual ~RemoteSwitchInfo();

    /*!
     * @brief Loads neighbor switches ids and store them in neighbor manager.
     *
     * @param[in] id Switch id
     * */
    void load_neighbor_switches_id(const string& id);

    /*!
     * @brief Fills response with switch info.
     *
     * @param[in] id Id of the switch.
     * @param[out] response Neighbor info
     * */
    void fill_response_with_switch_info(const string& id,
                                        agent_framework::model::
                                        RemoteEthernetSwitch& response);

    /*!
     * @brief Get remote switch identifier on a specified port.
     *
     * @param[in] switch_uuid local switch uuid.
     * @param[in] port_identifier port identifier on which to look for neighbor.
     *
     * @return remote switch identifier found on the port.
     * */
    virtual string get_neighbor_on_port(const string& switch_uuid,
                                      const string& port_identifier);

    /*!
     * @brief Get local DCRP node identifier.
     *
     * @return local node identifier.
     * */
    static std::string get_local_node_id();

private:
    /*!
     * @brief helper functions.
     * */
    class ScopedSocket {
    public:
        /*!
         * @brief constructor.
         * */
        ScopedSocket(int sock) : m_sock(sock) {}

        /*!
         * @brief Copy constructor.
         *
         * @param[in] orig object to be copied.
         * */
        ScopedSocket(const ScopedSocket& orig) = delete;

        /*!
         * @brief Copy operator.
         *
         * @param[in] orig object to be copied.
         * */
        ScopedSocket& operator=(const ScopedSocket& orig) = delete;

        /*!
         * @brief Destructor.
         * */
        virtual ~ScopedSocket();

    private:
        int m_sock;
    };

    using Pointer = std::unique_ptr<char[]>;

    /*!
     * @brief Send uint32_t value to the socket.
     *
     * @param[in] sock socket to use.
     * @param[in] value value to send.
     * */
    static void write_int(int sock, uint32_t value);

    /*!
     * @brief Read uint32_t value from the socket.
     *
     * @param[in] sock socket to use.
     * @param[out] value value to read into.
     * */
    static void read_int(int sock, uint32_t &value);

    /*!
     * @brief Send command and get response.
     *
     * @param[in] cmd Command to be sent.
     * @param[out] buf returned result.
     * @param[out] outsize the size of the result in bytes.
     * */
    static uint32_t process_command(const string& cmd, Pointer& buf,
                                    uint32_t& outsize);

    /*!
     * @brief Add switch identifier to Neighbor Manager.
     *
     * @param[in] parent_uuid Parent UUID.
     * @param[in] identifier Remote switch identifier.
     * */
    static void add_switch_to_neighbor_manager(const string& parent_uuid,
                                               const string& identifier);

    /*!
     * @brief Remove unknown neighbors from Neighbor Manager.
     *
     * @param[in] new_neighbors List of new neighbors.
     * */
    static void remove_unknown_neighbors(const std::vector<std::string>& new_neighbors);
};

}
}
}
}
