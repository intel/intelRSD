/*!
 * @copyright
 * Copyright (c) 2016-2019 Intel Corporation
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
 * @file nvme_secure_erase.hpp
 * @brief NVMe drive/namespace secure erase action
 * */

#pragma once



#include "nvme/nvme.hpp"

/*! Agent namespace */
namespace agent {
/*! PNC namespace */
namespace pnc {
/*! NVMe namespace */
namespace nvme {

/*!
 * @brief Secure erase of nvme drive
 *
 * @param device_path path to the device in the OS
 * */
void secure_erase(const std::string& device_path);


/*!
 * @brief Identify nvme controller or namespace
 *
 * @param file_descriptor file descriptor
 * @param ns namespace
 * @param ptr sth
 * @param cns sth
 * */
void identify(int file_descriptor, int ns, void* ptr, int cns);

}
}
}
