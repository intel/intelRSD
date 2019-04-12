/*!
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
 * @file partition/thread_safe_drive_interface_proxy.hpp
 */

#pragma once

#include "partition/abstract_drive_interface.hpp"

#include <string>
#include <mutex>
#include <condition_variable>
#include <set>

namespace partition {

/*!
 * @brief Proxy for drive interfaces that adds thread-safety to the interface.
 */
class ThreadSafeDriveInterfaceProxy final : public AbstractDriveInterface {
public:

    /*!
     * @brief Creates a valid proxy object
     * @param impl Drive interface to used by the proxy
     */
    ThreadSafeDriveInterfaceProxy(std::shared_ptr<AbstractDriveInterface> impl): m_impl(impl) {}

    ThreadSafeDriveInterfaceProxy(const ThreadSafeDriveInterfaceProxy&) = default;
    ThreadSafeDriveInterfaceProxy(ThreadSafeDriveInterfaceProxy&&) = default;

    ThreadSafeDriveInterfaceProxy& operator=(const ThreadSafeDriveInterfaceProxy&) = default;
    ThreadSafeDriveInterfaceProxy& operator=(ThreadSafeDriveInterfaceProxy&&) = default;

    virtual ~ThreadSafeDriveInterfaceProxy();

    /*! Implementations of pure virtual functions */
    virtual utils::ByteBuffer read(const std::string& target, uint64_t location, std::size_t num_of_bytes) const override;
    virtual void write(const std::string& target, uint64_t location, const utils::ByteBuffer& data) const override;
    virtual void write(const std::string& target, uint64_t location, uint64_t size, DataGenerator generator) const override;

private:

    void lock_target(const std::string& target) const;
    void unlock_target(const std::string& target) const;

    std::shared_ptr<AbstractDriveInterface> m_impl{};
    mutable std::set<std::string> m_locked_targets{};
    mutable std::mutex m_mutex{};
    mutable std::condition_variable m_cv{};

};

}
