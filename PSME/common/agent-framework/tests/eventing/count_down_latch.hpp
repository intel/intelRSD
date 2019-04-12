/*!
 * @brief CountDownLatch.
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
 * @file count_down_latch.hpp
 */

#include <mutex>
#include <condition_variable>

template<int N>
class CountDownLatch {
public:
    void count_down() {
        std::unique_lock<std::mutex> lk(mutex);
        if (--counter <= 0) {
            cv.notify_all();
        }
    }

    int get_count() const {
        std::unique_lock<std::mutex> lk(mutex);
        return counter;
    }

    void wait() const {
        std::unique_lock<std::mutex> lk(mutex);
        cv.wait(lk, [this](){return counter <= 0;});
    }

    template<typename R, typename P>
    bool wait_for(const std::chrono::duration<R, P>& timeout) const {
        std::unique_lock<std::mutex> lk(mutex);
        return cv.wait_for(lk, timeout, [this](){return counter <= 0;});
    }

private:
    mutable std::mutex mutex{};
    mutable std::condition_variable cv{};
    int counter{N};
};
