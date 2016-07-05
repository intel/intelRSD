/*!
 * @section LICENSE
 *
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
 *
 * @section DESCRIPTION
 *
 * @file ipmb/service.hpp
 * @brief IPMI IPMB Service.
 * */
#ifndef AGENT_CHASSIS_IPMB_SERVICE_HPP
#define AGENT_CHASSIS_IPMB_SERVICE_HPP
#include <ipmb/ipmi_message.hpp>
#include <ipmb/worker_thread.hpp>
#include <ipmb/receiver_thread.hpp>
#include <ipmb/responder.hpp>

#include "generic/threadsafe_queue.hpp"
#include <agent-framework/generic/singleton.hpp>

/*! Agent namespace */
namespace agent {
/*! Chassis namespace */
namespace chassis {
/*! IPMB namespace */
namespace ipmb {

using std::uint16_t;
using std::uint32_t;

using namespace agent_framework::generic;

/*! IpmiService interface for IPMB. */
class Service : public Singleton<Service> {
public:

    /*!
     * @brief Message queue for incomming requests
     * */
    ::generic::ThreadSafeQueue <IpmiMessage> rcvd_msg_queue{};

    /*!
     * @brief MUX initialization
     * @param mux_port MUX port
     * @return initialization status
     * */
    bool init(::in_port_t mux_port);

    /*!
     * @brief MUX deinitialization
     * @return deinitialization status
     * */
    bool deinit();

    /*!
     * @brief Start IPMI IPMB Service
     * @return Starting status
     * */
    bool start();

    /*!
     * @brief Stop IPMI IPMB Serice
     * @return Stoping status
     * */
    bool stop();

    /*!
     * @brief Returns Slave address
     * @return Slave address
     * */
    uint8_t get_slave_address() const {
        return m_slave_addr;
    }

    /*!
     * @brief Slave read from MUX
     * @param msg IPMI message
     * @return read status
     * */
    bool slave_read(IpmiMessage& msg);

    /*!
     * @brief Master write to MUX
     * @param msg IPMI message
     * @return write status
     * */
    bool master_write(IpmiMessage& msg);

    /*!
     * @brief Return initialized flag
     * @return initialized flag
     * */
    bool is_initialized() {
        return m_is_initialized;
    }

    /*!
     * @brief Destructor
     * */
    virtual ~Service();

private:
    /*! Data processing thread */
    WorkerThreadUniquePtr m_worker_thread{};
    /*! Receiver thread. */
    ReceiverThreadUniquePtr m_receiver_thread{};

    /*! I2C slave address. 0x10 for communication between IPMB and PSME. */
    uint8_t m_slave_addr{I2C_SLAVE_ADDRESS};

    /*! Mutex for communications */
    std::mutex m_mutex{};
    std::mutex m_client_mutex{};

    /*! Set to true if library is already initialized */
    volatile bool m_is_initialized{};
    /*! Set to true if IPMI IPMB Service is started */
    volatile bool m_is_working{};

    /*! flags used to show if failures have already been logged and prevent
     *  logging again until the next success occurs (as attempts are retried
     *  until succeeded)
     */
    volatile bool m_server_failure_logged{};
    volatile bool m_client_failure_logged{};

    /*! Socket connection to cyMUX */
    Responder m_mux{};

    bool verify_chcksum(const IpmiMessage &msg, const byte_vec_t &read_buffer);
    void message_builder(IpmiMessage& msg, const byte_vec_t& read_buffer);
};

}
}
}

#endif  /* AGENT_CHASSIS_IPMB_SERVICE_HPP */
