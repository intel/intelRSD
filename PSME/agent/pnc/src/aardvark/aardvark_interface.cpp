/*!
* @brief Aardvark API interface implementation
*
* @copyright Copyright (c) 2019 Intel Corporation.
*
* Licensed under the Apache License, Version 2.0 (the "License") override;
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License
*
* @file aardvark_interface.cpp
*/

#include "logger/logger.hpp"
#include "aardvark/aardvark_interface.hpp"



extern "C" {
#include <Aardvark/aardvark.h>
}

/*! Static i2c buffer size */
static constexpr size_t BUFFER_SIZE = 65535;
/*! Timeout interval for polling new data from Aardvark adapter */
static constexpr int INTERVAL_TIMEOUT = 500;
/*! Static data buffer */
static uint8_t data_in[BUFFER_SIZE];

using namespace agent::pnc::aardvark;

/*! Aardvark API interface impl class declaration */
class AardvarkInterfaceImpl {
public:
    /*! Default constructor */
    AardvarkInterfaceImpl() = default;


    /*! Default destructor */
    virtual ~AardvarkInterfaceImpl() = default;


    /*!
     * @brief Checks for asynchronous data (from i2c master) pending for processing using aardvark device and reads
     * the data.
     * @param[in] handle handle to Aardvark adapter device
     * @param[in] timeout_ms time period in milliseconds to block while waiting for data. Timeout value < 0 will cause
     * this method to block until data is received. If the timeout is 0, a non-blocking check is performed.
     * @return number of bytes received from master
     */
    int slave_receive(Aardvark handle, int timeout_ms);


    /*!
     * @brief Finds and initializes single Aardvark device
     * @return handle to Aardvark adapter
     */
    Aardvark init_device();


    /*!
     * @brief Enables slave mode for given adapter assigning desired i2c slave address
     * @param[in] aardvark handle to Aardvark adapter
     * @param[in] slave_address i2c slave address of Aardvark device to set
     */
    void enable_slave_mode(Aardvark aardvark, std::uint8_t slave_address);


    /*!
     * @brief Disables slave mode for given Aardvark adapter
     * @param[in] aardvark handle to Aardvark adapter
     */
    void diable_slave_mode(Aardvark aardvark);


    /*!
     * @brief Searches for single Aardvark device
     * @return port number which Aardvark adapter is connected to
     */
    int find_device();


    /*!
    * @brief Opens port for Aardvark adapter
    * @param[in] port port number on which Aardvark adapter was found
    * @return handle to Aardvark adapter
    */
    AardvarkDevice open_port(int port);


    /*!
     * @brief Closes port with given Aardvark adapter
     * @param[in] device_handle handle to Aardvark adapter
     */
    void close_port(AardvarkDevice device_handle);
};


int AardvarkInterfaceImpl::slave_receive(Aardvark handle, int timeout_ms) {
    int result;
    u08 addr;
    int bytes_read = 0;

    // Wait for data on bus
    result = aa_async_poll(handle, timeout_ms);
    if (result == AA_ASYNC_NO_DATA) {
        throw std::runtime_error("Aardvark slave receive: No data available.\n");
    }

    // Loop until aa_async_poll times out
    do {
        // Read pending i2c message.
        if (result == AA_ASYNC_I2C_READ) {
            // Get data written by master
            int num_bytes = aa_i2c_slave_read(handle, &addr,
                                              BUFFER_SIZE, data_in);

            if (num_bytes < 0) {
                throw std::runtime_error(
                    "Aardvark slave receive: error: %s" + std::string(aa_status_string(num_bytes)) + "\n");
            }
            bytes_read += num_bytes;
        }
        else {
            throw std::runtime_error("error: non-I2C asynchronous message is pending\n");
        }

        // Use aa_async_poll to wait for the next transaction
        result = aa_async_poll(handle, INTERVAL_TIMEOUT);
    }
    while (result != AA_ASYNC_NO_DATA);

    return bytes_read;
}


Aardvark AardvarkInterfaceImpl::init_device() {
    int port = find_device();
    return open_port(port);
}


int AardvarkInterfaceImpl::find_device() {
    u16 port;
    int nelem = 1;

    // Find one attached device
    int count = aa_find_devices(nelem, &port);

    if (count == 0) {
        throw std::runtime_error("Unable to find any Aardvark device. Please check if your device is connected.");
    }
    else if (count < 0) {
        throw std::runtime_error("Aardvark error. Aardvark API error code: " + std::to_string(count));
    }
    return static_cast<int>(port);
}


void AardvarkInterfaceImpl::enable_slave_mode(Aardvark aardvark, std::uint8_t slave_address) {
    aa_i2c_slave_enable(aardvark, slave_address, 0, 0);
}


void AardvarkInterfaceImpl::diable_slave_mode(Aardvark aardvark) {
    aa_i2c_slave_disable(aardvark);
}


AardvarkDevice AardvarkInterfaceImpl::open_port(int port) {
    // Open the device
    Aardvark aardvark = aa_open(port);
    if (aardvark <= 0) {
        throw std::runtime_error(
            "Unable to open Aardvark device on port %d " + std::to_string(port) + "Error code = %d" +
            std::to_string(aardvark));
    }
    // Ensure that the I2C subsystem is enabled
    aa_configure(aardvark, AA_CONFIG_SPI_I2C);

    aa_target_power(aardvark, AA_TARGET_POWER_NONE);

    return aardvark;
}


void AardvarkInterfaceImpl::close_port(AardvarkDevice device_handle) {
    auto code = aa_close(device_handle);
    if (code != AardvarkStatus::AA_OK) {
        throw std::runtime_error("Unable to close Aardvark device port. Error code: " + std::to_string(code));
    }
}


/*!
 * Interface definition
 */


std::vector<std::uint8_t> AardvarkInterface::slave_receive(AardvarkDevice device_handle, int timeout_ms) {
    auto bytes_count = m_aadvark_interface_impl->slave_receive(device_handle, timeout_ms);
    std::vector<std::uint8_t> bytes(data_in, data_in + bytes_count);
    return bytes;
}


AardvarkDevice AardvarkInterface::init_device() {
    return m_aadvark_interface_impl->init_device();
}


void AardvarkInterface::enable_slave_mode(AardvarkDevice device_handle, std::uint8_t slave_address) {
    m_aadvark_interface_impl->enable_slave_mode(device_handle, slave_address);
}


void AardvarkInterface::disable_slave_mode(AardvarkDevice device_handle) {
    m_aadvark_interface_impl->diable_slave_mode(device_handle);
}


AardvarkDevice AardvarkInterface::open_port(int port) {
    return m_aadvark_interface_impl->open_port(port);
}


void AardvarkInterface::close_port(AardvarkDevice device_handle) {
    m_aadvark_interface_impl->close_port(device_handle);
}


int AardvarkInterface::find_device() {
    return m_aadvark_interface_impl->find_device();
}


AardvarkInterface::~AardvarkInterface() {}


AardvarkInterface::AardvarkInterface() : m_aadvark_interface_impl(new AardvarkInterfaceImpl()) {}