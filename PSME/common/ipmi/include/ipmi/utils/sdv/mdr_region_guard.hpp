/*!
 * @brief Opportunistic MDR region lock - mutex style.
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file mdr_region_lock.hpp
 */

#pragma once



#include "logger/logger_factory.hpp"
#include "ipmi/command/sdv/enums.hpp"
#include "ipmi/response_error.hpp"

#include <functional>
#include <chrono>
#include <thread>



namespace ipmi {
namespace sdv {

/*!
 * @brief An opportunistic MDR region lock implementation.
 */
template<typename MdrLockResp>
class MdrRegionGuard final {
public:

    /*! MDR region lock function typedef */
    using LockFunc = std::function<MdrLockResp(uint16_t, uint8_t)>;

    /*! MDR region unlock function typedef */
    using UnlockFunc = std::function<void(uint8_t)>;


    /*!
     * @brief Constructor for the MDR region guard.
     *
     * @param lock_func the function to call, to send a lock request.
     * @param unlock_func the function to call, to send an unlock request.
     * @param duration_ms the duration for both the lock and backoff period.
     * @param retries the number of retires to acquire the lock.
     */
    MdrRegionGuard(LockFunc lock_func, UnlockFunc unlock_func, uint16_t duration_ms, int retries = 0) :
        m_lock{lock_func}, m_unlock{unlock_func}, m_handle{0}, m_duration{duration_ms}, m_retries{retries}{
    }


    /*!
     * @brief Acquire the lock.
     */
    void lock();


    /*!
     *  @brief Release the lock.
     *
     *  Does not throw an exception.
     */
    void unlock() noexcept;

    /*!
	 *  @brief Release the lock.
	 *
	 *  May throw an exception.
	 */
    void unlock_except() noexcept(false);

    /*!
     * @brief Deleted copy constructor.
     */
    MdrRegionGuard(const MdrRegionGuard&) = delete;


    /*!
     * @brief Defaulted move constructor.
     */
    MdrRegionGuard(MdrRegionGuard&&) = default;


    /*!
     * @brief Deleted assignment operator.
     */
    MdrRegionGuard& operator=(const MdrRegionGuard&) = delete;


    /*!
     * @brief Defaulted move assignment.
     */
    MdrRegionGuard& operator=(MdrRegionGuard&&) = default;


    /*!
     * @brief Defaulted destructor.
     */
    ~MdrRegionGuard() = default;


    /*!
     * @brief Getter for the lock handle.
     *
     * @return The handle for the lock.
     */
    uint8_t get_handle() const {
        return m_handle;
    }


private:
    std::function<MdrLockResp(uint16_t, uint8_t)> m_lock;
    std::function<void(uint8_t)> m_unlock;
    uint8_t m_handle;
    uint16_t m_duration;
    int m_retries;
};


template<typename MdrLockResp>
void MdrRegionGuard<MdrLockResp>::lock() {
    do {
        try {
            // request new handle
            m_handle = m_lock(m_duration, m_handle).get_session_lock_handle();
            return;
        }
        catch (const MdrRegionInUse&) {
            // this is normal behavior, try again after sleeping
        }
        catch (const MdrRegionLocked&) {
            // this is normal behavior, try again after sleeping
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(m_duration));
    } while (m_retries-- > 0);

    throw std::runtime_error("Unable to obtain MDR lock");
}


template<typename MdrLockResp>
void MdrRegionGuard<MdrLockResp>::unlock_except() noexcept(false) {
    // unlock only with valid handle
    if (m_handle != 0) {
		m_unlock(m_handle);
	}
}


template<typename MdrLockResp>
void MdrRegionGuard<MdrLockResp>::unlock() noexcept {
    try {
        unlock_except();
    }
    catch(const std::exception& e) {
        log_error("mdr-region-accessor", "An error occurred while unlocking the MDR region" << e.what());
    }
    catch(...) {
        log_error("mdr-region-accessor", "An unknown error occurred while unlocking the MDR region");
    }
}

} // namespace sdv
} // namespace ipmi
