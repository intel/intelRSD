/*!
* @brief Interface for Aardvark adapter API usage.
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
* @file aardvark_interface.hpp
*/


#pragma once



#include <string>
#include <memory>
#include <vector>



class AardvarkInterfaceImpl;

/*! Agent namespace */
namespace agent {
/*! PNC namespace */
namespace pnc {
/*! Aardvark namespace */
namespace aardvark {

using AardvarkDevice = int;

/*! Interface class for Aardvark adapter API */
class AardvarkInterface {
public:
    /*! Default constructor */
    AardvarkInterface();


    /*! Copy constructor */
    AardvarkInterface(const AardvarkInterface&) = default;


    /*! Assignment operator */
    AardvarkInterface& operator=(const AardvarkInterface&) = default;


    /*! Default destructor */
    virtual ~AardvarkInterface();


    /*!
     * @brief Receives data from i2c master using aardvark device with given timeout
     * @param[in] device_handle handle to Aardvark adapter device
     * @param[in] timeout_ms time period in milliseconds to block while waiting for data. Timeout value < 0 will cause
     * this method to block until data is received. If the timeout is 0, a non-blocking check is performed.
     * @return vector of bytes received from master
     */
    std::vector<std::uint8_t> slave_receive(AardvarkDevice device_handle, int timeout_ms);


    /*!
     * @brief Finds and initializes single Aardvark device
     * @return handle to Aardvark adapter
     */
    AardvarkDevice init_device();


    /*!
     * @brief Enables slave mode for given adapter assigning desired i2c slave address
     * @param[in] device_handle handle to Aardvark adapter
     * @param[in] slave_address i2c slave address of Aardvark device to set
     */
    void enable_slave_mode(AardvarkDevice device_handle, std::uint8_t slave_address);


    /*!
     * @brief Disables slave mode for given Aardvark adapter
     * @param[in] device_handle handle to Aardvark adapter
     */
    void disable_slave_mode(AardvarkDevice device_handle);


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


    /*!
     * @brief Searches for single Aardvark device
     * @return port number which Aardvark adapter is connected to
     */
    int find_device();


private:
    std::shared_ptr<AardvarkInterfaceImpl> m_aadvark_interface_impl;
};

using AardvarkInterfacePtr = std::shared_ptr<AardvarkInterface>;

}
}
}
