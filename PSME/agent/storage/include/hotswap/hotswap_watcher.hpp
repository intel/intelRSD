
/*!
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
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
 * */

#include <thread>
#include <atomic>
#include "hotswap/hotswap_manager.hpp"

namespace agent {
    namespace storage {

        class HotswapWatcher {
        public:
            void start();
            void stop();
        private:
            std::thread m_thread{};
            std::atomic<bool> m_running{false};
            void watch();
            void handle_hotswap();
            bool detect_hotswap(int, int);
            const char* m_watch_disk_dir = "/dev/disk/by-id/";
        };

    }
}
