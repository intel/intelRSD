/*!
 * @brief Factory to create IPMI communication interfaces ("sessions").
 *
 * Sessions are built on interface type. All builders are to be added to the factory.
 * Private class to be used by IpmiController.
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
 * @file ipmi/ipmi_intf_factory.hpp
 */

#pragma once

#include "ipmi/ipmi_interface.hpp"

#include <functional>
#include <map>
#include <list>
#include <memory>
#include <mutex>

/*!
 * @brief Test "methods" checking number of interfaces "kept" in the factory
 */
namespace testing {

class TestManagementController;

}

/*!
 * @brief Owner of the <<private>> factory
 */
namespace ipmi {

class IpmiController;

}


namespace ipmi {

class IpmiInterfaceFactory {

friend class testing::TestManagementController;
friend class IpmiController;

public:
    using IpmiInterface_WeakPtr = std::weak_ptr<IpmiInterface>;
    using Collection = std::list<IpmiInterface_WeakPtr>;

    using Builder = std::function<IpmiInterface::Ptr(ConnectionData::ConstPtr)>;
    using Builders = std::map<std::string, Builder>;

protected:
    IpmiInterfaceFactory();
    virtual ~IpmiInterfaceFactory();

    /*!
     * @brief Get instance of the factory
     * @return Static IPMI interfaces factory
     */
    static IpmiInterfaceFactory& get_instance();

    /*!
     * @brief Get existing IPMI Interface for given connection data or create new one.
     *
     * Interface is properly configured and ready to action
     *
     * @param connection_data to be matched to the interface
     * @return pointer to (just created/already existing) IPMI interface. Empty
     *         pointer if
     */
    IpmiInterface::Ptr get_interface(ConnectionData::ConstPtr connection_data);

    /*!
     * @brief Release the interface
     *
     * Local pointer is removed. If it was the last controller which has used this
     * interface, delete the interface.
     *
     * @param intf interface to be released
     */
    void release_interface(IpmiInterface::Ptr&& intf);

    /*!
     * @brief Add builder to the factory.
     *
     * Builder is able to build IPMI interface for given type. Everything bases
     * on connection data interface type.
     *
     * @param interface_type type name of interface to be added
     * @param builder function to create new IPMI interface
     */
    void add_builder(const std::string& interface_type, Builder builder);


    /*!
     * @brief Template version of adding a builder
     * @param interface_type type name of interface to be added
     */
    template<class I>
    void add_builder(const std::string& interface_type) {
        add_builder(interface_type, [](ipmi::ConnectionData::ConstPtr connection_data) -> ipmi::IpmiInterface::Ptr {
            return std::make_shared<I>(connection_data);
        });
    }

private:
    /*!
     * @brief Collection of defined builders
     */
    Builders builders{};

    /*!
     * @brief Collection of built interfaces
     */
    Collection interfaces{};

    /*!
     * @brief Mutex to keep interfaces synchronized.
     *
     * Mutable for get_interfaces_number() const.
     */
    mutable std::mutex mutex{};
};

}
