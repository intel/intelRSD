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
 * */

#include "ssdp/notification.hpp"
#include "gtest/gtest.h"

#include <vector>

using namespace ssdp;

TEST(NotificationTest, SerializeDeserializeNetworkChangeNotification) {
    std::vector<NetworkChangeNotification> notifications;

    notifications.emplace_back(NetworkChangeNotification{});
    notifications.emplace_back(static_cast<unsigned>(-1),
                net::NetworkChangeNotifier::ChangeType::NEW_LINK);

    NetworkChangeNotification deserialized;
    for (const auto& notification : notifications) {
        auto bytes = notification.serialize();

        deserialized.deserialize(bytes, bytes.size());
        EXPECT_EQ(notification.get_type(), deserialized.get_type());
        EXPECT_EQ(notification.get_iface_index(), deserialized.get_iface_index());
        EXPECT_EQ(notification.get_change_type(), deserialized.get_change_type());
    }
}

TEST(NotificationTest, NetworkChangeNotificationDeserializeShouldFail) {
    Notification::Bytes bytes_wrong_type = {30};
    Notification::Bytes bytes_length_too_short = {std::uint8_t(Notification::Type::NETWORK_CHANGE)};
    NetworkChangeNotification deserialized;
    EXPECT_THROW(deserialized.deserialize(bytes_wrong_type, bytes_wrong_type.size()),
                 std::runtime_error);
    EXPECT_THROW(deserialized.deserialize(bytes_length_too_short, bytes_length_too_short.size()),
                 std::runtime_error);
}

TEST(NotificationTest, SerializeDeserializeShutDownNotification) {
    ShutDownNotification notification;
    auto bytes = notification.serialize();
    ShutDownNotification deserialized;
    deserialized.deserialize(bytes, bytes.size());
    EXPECT_EQ(notification.get_type(), deserialized.get_type());
}
