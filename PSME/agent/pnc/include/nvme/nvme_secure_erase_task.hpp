/*!
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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
 * @file nvme_secure_erase_task.hpp
 * @brief NVMe secure erase task
 * */

#pragma once



#include <string>



namespace agent {
namespace pnc {
namespace nvme {

/*! @brief  NVMe drive secure erase task class */
class NvmeSecureEraseTask {
public:
    /*!
     * @brief Constructor
     * @param[in] drive_uuid drive to be erased
     * */
    explicit NvmeSecureEraseTask(const std::string& drive_uuid);


    /*! @brief Starts erasing */
    void operator()();


private:
    std::string m_drive_uuid{};


    std::string get_drive_uuid() const;
};

}
}
}

