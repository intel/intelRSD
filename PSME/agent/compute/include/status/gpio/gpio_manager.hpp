/*!
 * @brief GPIO manager class interface
 *
 * Generic "interface" class. It should be used as a reference to "real" controller.
 * Handle all passed requests. Connection data is to be passed as shared ptr,
 * no references are implemented (memory management actually is to be done in
 * the code)
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file gpio_manager.hpp
 */

#include "agent-framework/generic/singleton.hpp"
#include <ipmb/ipmi_message.hpp>
#include "net/stream_socket.hpp"
#include <chrono>
#include <mutex>



namespace agent {
namespace compute {
namespace status {

class GpioManager : public agent_framework::generic::Singleton<GpioManager> {
public:
    using byte_vec_t = std::vector<std::uint8_t>;
    using Clock = std::chrono::steady_clock;
    using TimePoint = Clock::time_point;

    /*!
     * @brief Get GPIO sled presence
     * @return Sled presence bitmask
     */
    std::uint8_t get_presence();

    /*!
     * @brief Returns presence flag of sled on given slot
     * @param slot Slot number
     * @return true if Sled is present, false otherwise.
     */
    bool is_present(std::uint8_t slot);

    /*!
     * @return Minimal update interval between presence readings.
     */
    std::chrono::milliseconds get_minimal_update_interval() const;

protected:

    /*!
     * @brief Update sled presence based on getSledPresence IPMI command.
     */
    void update_presence();


    /*!
     * @brief Send getSledPresence IPMI command to cyMUX and process the response.
     * @param msg Request message.
     */
    void send_request_process_response(agent::compute::ipmb::IpmiMessage& msg);


    /*!
     * @brief Verify message integrity by calculating its checksum.
     * @param msg IPMB message constructed from received buffer.
     * @param read_buffer Reseived byte buffer.
     * @return True if checksum is correct, false otherwise.
     */
    static bool verify_checksum(const agent::compute::ipmb::IpmiMessage& msg, const byte_vec_t& read_buffer);


    /*!
     * @brief Construct an IPMB message from a byte stream.
     * @param read_buffer IPMI message byte stream.
     * @return IPMB message object.
     */
    static agent::compute::ipmb::IpmiMessage message_builder(const byte_vec_t& read_buffer);

    GpioManager();

private:

    friend class agent_framework::generic::Singleton<GpioManager>;

    net::SocketAddress m_mux_socket_address{net::SocketAddress{"127.0.0.1", 5623}};
    std::chrono::milliseconds m_update_interval{5000};
    TimePoint m_last_update_time{};
    std::uint8_t m_presence{};
    std::mutex m_mutex{};
};

}
}
}
