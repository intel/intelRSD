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
 * @file partition/thread_safe_drive_interface_proxy.cpp
 */

#include "partition/thread_safe_drive_interface_proxy.hpp"



using namespace partition;


ThreadSafeDriveInterfaceProxy::~ThreadSafeDriveInterfaceProxy() {
}


utils::ByteBuffer
ThreadSafeDriveInterfaceProxy::read(const std::string& target, uint64_t location, std::size_t num_of_bytes) const {
    utils::ByteBuffer ret;
    lock_target(target);
    try {
        ret = m_impl->read(target, location, num_of_bytes);
    }
    catch (const std::exception& e) {
        unlock_target(target);
        throw std::runtime_error(std::string{"Unable to read target: " + target + ". " + e.what()});
    }
    unlock_target(target);
    return ret;
}


void ThreadSafeDriveInterfaceProxy::write(const std::string& target, uint64_t location,
                                          const utils::ByteBuffer& data) const {
    lock_target(target);
    try {
        m_impl->write(target, location, data);
    }
    catch (const std::exception& e) {
        unlock_target(target);
        throw std::runtime_error(std::string{"Unable to write target: " + target + ". " + e.what()});
    }
    unlock_target(target);
}


void ThreadSafeDriveInterfaceProxy::write(const std::string& target, uint64_t location, uint64_t size,
                                          DataGenerator generator) const {
    lock_target(target);
    try {
        m_impl->write(target, location, size, generator);
    }
    catch (const std::exception& e) {
        unlock_target(target);
        throw std::runtime_error(std::string{"Unable to write target: " + target + ". " + e.what()});
    }
    unlock_target(target);
}


void ThreadSafeDriveInterfaceProxy::lock_target(const std::string& target) const {
    std::unique_lock<std::mutex> lock(m_mutex);
    m_cv.wait(lock, [this, &target] { return (this->m_locked_targets.count(target) == 0); });
    m_locked_targets.insert(target);
}


void ThreadSafeDriveInterfaceProxy::unlock_target(const std::string& target) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_locked_targets.erase(target);
    m_cv.notify_all();
}
