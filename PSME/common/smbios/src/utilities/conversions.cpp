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
 * @file conversions.cpp
 *
 * @brief Converting functions for smbios structures
 * */

#include "smbios/utilities/conversions.hpp"

#include <stdexcept>



namespace smbios {
namespace utilities {

const std::string get_system_uuid(const smbios::parser::SYSTEM_INFO_DATA& system_info_data) {
    // UUID is stored using 16 bytes; each byte is represented using 2 ASCII characters. Additionally,
    // there are 4 dashes and 1 place is needed for EOL character.
    constexpr size_t UUID_BUFFER_LENGTH = 2 * smbios::parser::SYSTEM_UUID_LENGTH + 4 + 1;
    char uuid_buffer[UUID_BUFFER_LENGTH] = {};

    const int written_chars_number = std::snprintf(uuid_buffer, UUID_BUFFER_LENGTH,
                                               "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
                                               system_info_data.uuid[3],
                                               system_info_data.uuid[2],
                                               system_info_data.uuid[1],
                                               system_info_data.uuid[0],
                                               system_info_data.uuid[5],
                                               system_info_data.uuid[4],
                                               system_info_data.uuid[7],
                                               system_info_data.uuid[6],
                                               system_info_data.uuid[8],
                                               system_info_data.uuid[9],
                                               system_info_data.uuid[10],
                                               system_info_data.uuid[11],
                                               system_info_data.uuid[12],
                                               system_info_data.uuid[13],
                                               system_info_data.uuid[14],
                                               system_info_data.uuid[15]);

    // Add EOL character
    uuid_buffer[UUID_BUFFER_LENGTH - 1] = '\0';

    if (written_chars_number != UUID_BUFFER_LENGTH - 1) {
        throw std::runtime_error("Could not create UUID string from provided data");
    }
    else {
        return std::string(uuid_buffer);
    }
}

}
}
