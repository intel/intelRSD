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
 * @file network_change_notifier.hpp
 *
 * @brief NetworkChangeNotifier interface
 * */

#pragma once

#include <memory>
#include <mutex>
#include <vector>

namespace net {

/*!
 * Monitors system for NetworkInterface changes
 * and notifies listeners when network interface change occurs.
 */
class NetworkChangeNotifier {
public:
    /*!
     * Change type enumeration
     */
    enum ChangeType : std::uint8_t {
        NO_CHANGE,
        NEW_IP,
        DEL_IP,
        NEW_LINK,
        DEL_LINK
    };

    /*!
     * Factory method.
     * @return Implementation of NetworkChangeNotifier.
     */
    static std::unique_ptr<NetworkChangeNotifier> create();

    /*! Destructor */
    virtual ~NetworkChangeNotifier();

    /*!
     * NetworkChangeListener interface.
     */
    class NetworkChangeListener {
    public:
        using WeakPtr = std::weak_ptr<NetworkChangeListener>;

        /*!
         * Callback invoked on NetworkInterface change.
         * @param[in] iface_index Index of changed NetworkInterface.
         * @param[in] change_type Type of change.
         */
        virtual void on_network_change(unsigned iface_index, ChangeType change_type) = 0;

    protected:
        /*! Constructor */
        NetworkChangeListener() {}
        /*! Destructor */
        virtual ~NetworkChangeListener();
    };

    /*!
     * Adds Listener to observers list.
     * @param[in] listener NetworkChangeListener object.
     */
    void add_listener(NetworkChangeListener::WeakPtr listener);

    /*!
     * Removes Listener from observers list.
     * @param[in] listener NetworkChangeListener object.
     */
    void remove_listener(NetworkChangeListener& listener);

    /*!
     * @brief Starts network interface monitoring.
     */
    virtual void start() = 0;

    /*!
     * @brief Stops network interface monitoring.
     */
    virtual void stop() = 0;

protected:
    /*!
     * Notifies listeners on change.
     * @param[in] iface_index NetworkInterface index.
     * @param[in] change_type Type of change.
     */
    void notify_listeners(unsigned iface_index, NetworkChangeNotifier::ChangeType change_type);

    /*! Constructor */
    NetworkChangeNotifier() {}

private:
    friend class NetworkChangeNotifierTest;

    std::mutex m_listeners_mutex{};
    std::vector<NetworkChangeListener::WeakPtr> m_network_change_listeners{};
};

}
