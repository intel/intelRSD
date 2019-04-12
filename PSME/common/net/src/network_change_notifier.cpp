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
 * @file network_change_notifier.cpp
 *
 * @brief NetworkChangeNotifier implementation.
 * */

#include "net/network_change_notifier_impl.hpp"
#include "logger/logger_factory.hpp"

#include <algorithm>

namespace net {

std::unique_ptr<NetworkChangeNotifier> NetworkChangeNotifier::create() {
    return std::unique_ptr<NetworkChangeNotifier>(new NetworkChangeNotifierImpl);
}

NetworkChangeNotifier::~NetworkChangeNotifier() {
}

void NetworkChangeNotifier::add_listener(NetworkChangeListener::WeakPtr listener) {
    std::lock_guard<std::mutex> lock(m_listeners_mutex);
    m_network_change_listeners.emplace_back(listener);
}

void NetworkChangeNotifier::remove_listener(NetworkChangeListener& listener) {
    std::lock_guard<std::mutex> lock(m_listeners_mutex);
    m_network_change_listeners.erase(std::remove_if(m_network_change_listeners.begin(),
            m_network_change_listeners.end(),
            [&listener](NetworkChangeListener::WeakPtr l) {
                auto exists = l.lock();
                return !exists || (exists && exists.get() == &listener);
            }),
    m_network_change_listeners.end());
}

NetworkChangeNotifier::NetworkChangeListener::~NetworkChangeListener() {}

void NetworkChangeNotifier::notify_listeners(unsigned iface_index,
                                             NetworkChangeNotifier::ChangeType change_type) {
    std::lock_guard<std::mutex> lock(m_listeners_mutex);
    for (const auto& listener : m_network_change_listeners) {
        if (auto l = listener.lock()) {
            try {
                l->on_network_change(iface_index, change_type);
            }
            catch(const std::exception& e) {
                log_error("net",
                        "NetworkChangeListener callback error: " << e.what());
            }
        }
    }
}

}
