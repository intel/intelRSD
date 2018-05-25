/*!
 * @header{License}
 * @copyright Copyright (c) 2017-2018 Intel Corporation.
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
 * @header{Filesystem}
 * @file fabric_properties.hpp
 */

#pragma once

#include <string>
#include <sstream>

namespace nvmfpp {

enum class Transport : unsigned int  {
    RDMA,
    FC,
    LOOP
};

class FabricProperties final {
public:
    FabricProperties() = default;
    ~FabricProperties();

    /*!
     * @brief Constructor with mandatory fields for connect command.
     * @param nqn nqn
     * @param address ip address
     * @param port rdma port
     * @param transport transport protocol (rdma, fc, loop)
     */
    FabricProperties(
            const std::string& nqn,
            const std::string& address,
            std::uint16_t port,
            Transport transport
            );

    /*!
     * @brief Get nqn
     * @return nqn
     */
    const std::string& get_nqn() const {
        return m_nqn;
    }

    /*!
     * @brief Set nqn
     * @param[in] nqn nqn
     */
    void set_nqn(const std::string& nqn) {
        m_nqn = nqn;
    }

    /*!
     * @brief Get address ip
     * @return address address ip
     */
    const std::string& get_address() const {
        return m_address;
    }

    /*!
     * @brief Set address
     * @param[in] address address
     */
    void set_address(const std::string& address) {
        m_address = address;
    }

    /*!
     * @brief Get port
     * @return port
     */
    const std::string& get_port() const {
        return m_port;
    }

    /*!
     * @brief Set port
     * @param[in] port port
     */
    void set_port(std::uint16_t port) {
        m_port = uint16_to_string(port);
    }

    /*!
     * @brief Get transport
     * @return transport
     */
    const std::string& get_transport() const {
        return m_transport;
    }

    /*!
     * @brief Set transport
     * @param[in] transport transport
     */
    void set_transport(Transport transport) {
        m_transport = enum_to_string(transport);
    }

    /*!
     * @brief Get host address
     * @return host_traddr
     */
    const std::string& get_host_traddr() const {
        return m_host_traddr;
    }

    /*!
     * @brief Set host address
     * @param[in] host_traddr host_traddr
     */
    void set_host_traddr(const std::string& host_traddr) {
        m_host_traddr = host_traddr;
    }

    /*!
     * @brief Get hostnqn
     * @return hostnqn
     */
    const std::string& get_hostnqn() const {
        return m_hostnqn;
    }

    /*!
     * @brief Set hostnqn
     * @param[in] hostnqn hostnqn
     */
    void set_hostnqn(const std::string& hostnqn) {
        m_hostnqn = hostnqn;
    }

    /*!
     * @brief Get nr io queues, this property has to have value between <1,40>.
     * This field is mandatory for platforms that have nproc > 40.
     * @return nr_io_queues
     */
    const std::string& get_nr_io_queues() const {
        return m_nr_io_queues;
    }

    /*!
     * @brief Get nr io queues, this property has to have value between <1,40>.
     * This field is mandatory for platforms that have nproc > 40. Preferable value "1".
     * @param[in] nr_io_queues nr_io_queues
     */
    void set_nr_io_queues(std::uint16_t nr_io_queues) {
        m_nr_io_queues = uint16_to_string(nr_io_queues);
    }

    /*!
     * @brief Get queue size
     * @return queue_size
     */
    const std::string& get_queue_size() const {
        return m_queue_size;
    }

    /*!
     * @brief Set queue size
     * @param[in] queue_size queue_size
     */
    void set_queue_size(std::uint16_t queue_size) {
        m_queue_size = uint16_to_string(queue_size);
    }

    /*!
     * @brief Get keep alive tmo
     * @return keep_alive_tmo
     */
    const std::string& get_keep_alive_tmo() const {
        return m_keep_alive_tmo;
    }

    /*!
     * @brief Set keep alive tmo
     * @param[in] keep_alive_tmo keep_alive_tmo
     */
    void set_keep_alive_tmo(std::uint16_t keep_alive_tmo) {
        m_keep_alive_tmo = uint16_to_string(keep_alive_tmo);
    }

    /*!
     * @brief Get reconnect delay
     * @return reconnect_delay
     */
    const std::string& get_reconnect_delay() const {
        return m_reconnect_delay;
    }

    /*!
     * @brief Set reconnect delay
     * @param[in] reconnect_delay reconnect_delay
     */
    void set_reconnect_delay(std::uint16_t reconnect_delay) {
        m_reconnect_delay = uint16_to_string(reconnect_delay);
    }

    /*!
     * @brief Get device
     * @return device
     */
    const std::string& get_device() const {
        return m_device;
    }

    /*!
     * @brief Set device. Example: nvme5
     * @param[in] device device
     */
    void set_device(const std::string& device) {
        m_device = device;
    }

    /*!
     * @brief Checks values of fields required for connect action.
     * @return True if values are not empty
     */
    bool check_mandatory_connect_fields() const;

    /*!
     * @brief Checks values of fields required for disconnect action.
     * @return True if fields doesn't have the same value.
     */
    bool check_mandatory_disconnect_fields() const;

    /*!
     * @brief Packs fields into one string.
     * @return Packed values into one string.
     */
    std::string pack() const;

private:

    /*!
     * @brief Transport enum to string converter.
     * @param transport Transport as enum.
     * @return string Transport as string.
     */
    std::string enum_to_string(Transport transport) const ;

    /*!
     * @brief Property as uint16_t to string converter
     * @param property_uint16 Property as uint16_t.
     * @return string Property as string.
     */
    std::string uint16_to_string(uint16_t property_uint16) const ;

    /*! Fabric properties */
    std::string m_nqn{};
    std::string m_transport{};
    std::string m_address{};
    std::string m_port{};
    std::string m_host_traddr{};
    std::string m_hostnqn{};
    std::string m_nr_io_queues{"1"};
    std::string m_queue_size{};
    std::string m_keep_alive_tmo{};
    std::string m_reconnect_delay{};
    std::string m_device{};

};

}
