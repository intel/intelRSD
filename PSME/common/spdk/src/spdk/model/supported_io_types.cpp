/*!
 * @copyright Copyright (c) 2018-2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file supported_io_types.cpp
 */


#include "json-wrapper/json-wrapper.hpp"
#include "spdk/model/supported_io_types.hpp"



using namespace spdk;

namespace {

constexpr const char WRITE[] = "write";
constexpr const char READ[] = "read";
constexpr const char UNMAP[] = "unmap";
constexpr const char WRITE_ZEROES[] = "write_zeroes";
constexpr const char FLUSH[] = "flush";
constexpr const char RESET[] = "reset";
constexpr const char NVME_ADMIN[] = "nvme_admin";
constexpr const char NVME_IO[] = "nvme_io";

}


void model::to_json(json::Json& json, const SupportedIoTypes& supported_io_types) {
    json = json::Json{
        {::WRITE,        supported_io_types.is_write()},
        {::READ,         supported_io_types.is_read()},
        {::UNMAP,        supported_io_types.is_unmap()},
        {::WRITE_ZEROES, supported_io_types.is_write_zeroes()},
        {::FLUSH,        supported_io_types.is_flush()},
        {::RESET,        supported_io_types.is_reset()},
        {::NVME_ADMIN,   supported_io_types.is_nvme_admin()},
        {::NVME_IO,      supported_io_types.is_nvme_io()}
    };
}


void model::from_json(const json::Json& json, SupportedIoTypes& supported_io_types) {
    supported_io_types.set_write(json.at(::WRITE).get<bool>());
    supported_io_types.set_read(json.at(::READ).get<bool>());
    supported_io_types.set_unmap(json.at(::UNMAP).get<bool>());
    supported_io_types.set_write_zeroes(json.at(::WRITE_ZEROES).get<bool>());
    supported_io_types.set_flush(json.at(::FLUSH).get<bool>());
    supported_io_types.set_reset(json.at(::RESET).get<bool>());
    supported_io_types.set_nvme_admin(json.at(::NVME_ADMIN).get<bool>());
    supported_io_types.set_nvme_io(json.at(::NVME_IO).get<bool>());
}


std::string model::SupportedIoTypes::to_string() const {
    std::stringstream ss{};

    ss << " Read: " << std::boolalpha << m_read << std::endl;
    ss << " Write: " << std::boolalpha << m_write << std::endl;
    ss << " Unmap: " << std::boolalpha << m_unmap << std::endl;
    ss << " Write_zeros: " << std::boolalpha << m_write_zeroes << std::endl;
    ss << " Flush: " << std::boolalpha << m_flush << std::endl;
    ss << " Reset: " << std::boolalpha << m_reset << std::endl;
    ss << " Nvme_admin: " << std::boolalpha << m_nvme_admin << std::endl;
    ss << " Nvme_io: " << std::boolalpha << m_nvme_io;

    return ss.str();
}



