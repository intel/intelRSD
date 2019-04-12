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
 * @file message_queue.hpp
 *
 * @brief Notification declaration
 * */

#pragma once

#include "net/network_change_notifier.hpp"

#include <memory>
#include <vector>
#include <limits>

namespace ssdp {

/*!
 * Base Notification type for SsdpService.
 * Used to pass notifications to SsdpService thread, like shutdown request,
 * network change notification.
 */
class Notification {
public:
    using Bytes = std::vector<std::uint8_t>;
    static constexpr const Bytes::size_type MAX_SERIALIZED_SIZE = std::numeric_limits<unsigned char>::max();

    /*!
     * Enumeration of Notification types
     */
    enum class Type : std::uint8_t {
        NETWORK_CHANGE,
        SHUT_DOWN
    };

    /*!
     * Constructor
     * @param[in] type Notification type
     */
    explicit Notification(Type type);

    Notification(const Notification&) = default;
    Notification& operator=(const Notification&) = default;

    /*! Destructor */
    virtual ~Notification();

    /*!
     * Notification type getter
     * @return Notification type.
     */
    Type get_type() const;

    /*!
     * Serializes Notification to byte array.
     * @return Byte array containing serialized notification.
     */
    virtual Bytes serialize() const = 0;

    /*!
     * Deserializes Notification from byte array.
     * @param[in] bytes Array of bytes containing serialized notification.
     * @param[in] length Length of valid bytes in array.
     */
    virtual void deserialize(const Bytes& bytes, const Bytes::size_type length) = 0;

private:
    Type m_type;
};

/*! ShutDownNotification */
class ShutDownNotification : public Notification {
public:
    /*! Constructor */
    ShutDownNotification();

    ShutDownNotification(const ShutDownNotification&) = default;
    ShutDownNotification& operator=(const ShutDownNotification&) = default;

    /*! Destructor */
    ~ShutDownNotification() override;

    Bytes serialize() const override;
    void deserialize(const Bytes& bytes, const Bytes::size_type length) override;
};

/*! Notifies about network change.
 *  Carries information about change type and network interface index which changed. */
class NetworkChangeNotification: public Notification {
public:
    /*! Constructor */
    NetworkChangeNotification();

    NetworkChangeNotification(const NetworkChangeNotification&) = default;
    NetworkChangeNotification& operator=(const NetworkChangeNotification&) = default;

    /*!
     * Constructor
     * @param[in] iface_index NetworkInterface index
     * @param[in] change_type Type of NetworkInterface change.
     */
    NetworkChangeNotification(unsigned iface_index, net::NetworkChangeNotifier::ChangeType change_type);

    /*! Destructor */
    ~NetworkChangeNotification() override;

    /*!
     * NetworkInterface index getter
     * @return Index of changed NetworkInterface.
     */
    int get_iface_index() const;

    /*!
     * NetworkInterface change type getter
     * @return ChangeType which occurred on NetworkInterface
     */
    net::NetworkChangeNotifier::ChangeType get_change_type() const;

    Bytes serialize() const override;
    void deserialize(const Bytes& bytes, const Bytes::size_type length) override;

private:
    unsigned m_iface_index{0};
    net::NetworkChangeNotifier::ChangeType m_change_type{net::NetworkChangeNotifier::ChangeType::NO_CHANGE};
};

}
