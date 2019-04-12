/*!
 * @brief Rmm certificate manager class implementation
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
 * @file certificate_manager.cpp
 */

#include "certificate_management/certificate_manager.hpp"
#include "certificate_management/certificate_utilities.hpp"
#include "ipmi/command/sdv/rmm/get_authorized_cert_hash.hpp"
#include "ipmi/command/sdv/rmm/set_authorized_cert.hpp"
#include "logger/logger.hpp"

#include <algorithm>
#include <exception>
#include <chrono>
#include <thread>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>


extern "C" {
#include <md5/md5.h>
}


using namespace agent::rmm;
using namespace agent::rmm::certificate_utilities;

namespace {
constexpr char MANAGER_LOGGER_NAME[] = "rmm-certificate-manager";

std::string certificate_type_to_string(CertificateManager::CertificateType certificate_type) {
    switch (certificate_type) {
        case CertificateManager::CertificateType::PODM:
            return "PODM";
        case CertificateManager::CertificateType::RMM:
            return "RMM";
        default:
            throw std::runtime_error("Unknown certificate type!");

    }
}

ipmi::command::sdv::CertificateType certificate_type_to_enum(CertificateManager::CertificateType certificate_type) {
    switch (certificate_type) {
        case CertificateManager::CertificateType::PODM:
            return ipmi::command::sdv::CertificateType::PODM;
        case CertificateManager::CertificateType::RMM:
            return ipmi::command::sdv::CertificateType::RMM;
        default:
            throw std::runtime_error("Unknown certificate type!");
    }
}


std::string print_buffer(const uint8_t* buffer, const size_t len) {
    std::string msg_body_string{};

    if (buffer && len > 0) {
        for (size_t i = 0; i < len; i++) {
            std::stringstream ss{};
            ss.width(2);
            ss.fill('0');
            ss << std::hex << static_cast<unsigned>(buffer[i]);
            msg_body_string += " " + ss.str();
        }
    }

    return msg_body_string;
}


const std::string bridge_info_to_string(const ipmi::BridgeInfo& bridge_info) {
    std::string bridge_info_serialized{};

    ipmi::BridgeInfo::Address address;
    ipmi::BridgeInfo::Channel channel;
    ipmi::BridgeInfo::Level level = bridge_info.get_level();

    if (bridge_info.get_target(address, channel)) {
        bridge_info_serialized += "target address: " + std::to_string(address) + ", target channel: " +
                                  std::to_string(static_cast<unsigned int>(channel));
    }

    if (bridge_info.get_transit(address, channel)) {
        bridge_info_serialized += ", transit address: " + std::to_string(address) + ", transit channel: " +
                                  std::to_string(static_cast<unsigned int>(channel));
    }

    bridge_info_serialized += ", bridging level: " + std::to_string(static_cast<unsigned int>(level));

    return bridge_info_serialized;
}

}


CertificateManager::CertificateManager() : certificates{{
    std::make_shared<CertificateData>(CertificateManager::CertificateType::PODM)
    /* no RMM certificate defined, it is not handled in the app */
}} { }

CertificateManager::~CertificateManager() { }

CertificateDataReference CertificateManager::get_certificate(CertificateManager::CertificateType certificate_type) const {
    std::lock_guard<std::mutex> lock(m_mutex);

    for (auto cert : certificates) {
        if (cert->get_certificate_type() == certificate_type) {
            return *cert;
        }
    }
    throw std::runtime_error("No certificate " + certificate_type_to_string(certificate_type) + " defined");
}

void CertificateManager::update_certificate(CertificateManager::CertificateType certificate_type,
                                            const ipmi::IpmiController::Ptr controller,
                                            const ipmi::BridgeInfo& bridge_info) {
    log_info(MANAGER_LOGGER_NAME,
             "Checking " << certificate_type_to_string(certificate_type) << " certificate for changes.");

    CertificateData::CertificateHashType hash;
    {
        CertificateDataReference reference = get_certificate(certificate_type);
        CertificateData& certificate_data = *reference;

        if (read_cert_file_and_update(certificate_data)) {
            log_info(MANAGER_LOGGER_NAME,
                certificate_type_to_string(certificate_type) << " changed. Updating.");
        }
        else {
            log_info(MANAGER_LOGGER_NAME,
                certificate_type_to_string(certificate_type) << " did not change since last check.");
        }
        hash = certificate_data.get_certificate_hash();
    }

    CertificateData::CertificateHashType agent_certificate_hash = get_certificate_hash_from_client(
        certificate_type, controller, bridge_info);

    if (agent_certificate_hash != hash) {
        log_info(MANAGER_LOGGER_NAME,
                 "Updating outdated " << certificate_type_to_string(certificate_type) << " certificate for client.");
        log_info(MANAGER_LOGGER_NAME,
                 "Updating outdated " << certificate_type_to_string(certificate_type) << " certificate for client with "
                                      << bridge_info_to_string(bridge_info) << ".");
        send_certificate_to_client(certificate_type, controller, bridge_info);
    }
}


CertificateData::CertificateHashType
CertificateManager::get_certificate_hash_from_client(CertificateManager::CertificateType certificate_type,
                                                     const ipmi::IpmiController::Ptr controller,
                                                     const ipmi::BridgeInfo& bridge_info) {
    log_info(MANAGER_LOGGER_NAME,
             "Getting " << certificate_type_to_string(certificate_type) << " certificate hash from client.");
    log_debug(MANAGER_LOGGER_NAME,
              "Getting " << certificate_type_to_string(certificate_type) << " certificate hash from client with "
                         << bridge_info_to_string(bridge_info) << ".");

    ipmi::command::sdv::request::GetAuthorizedCertHash get_hash_request{};
    ipmi::command::sdv::response::GetAuthorizedCertHash get_hash_response{};

    get_hash_request.set_certificate_type(certificate_type_to_enum(certificate_type));
    controller->send(get_hash_request, bridge_info, get_hash_response);

    const auto& certificate_hash = get_hash_response.get_certificate_hash();

    log_debug(MANAGER_LOGGER_NAME,
              certificate_type_to_string(certificate_type) << " certificate hash " <<
              print_buffer(certificate_hash.data(), certificate_hash.size()) <<
              " received from client at " << bridge_info_to_string(bridge_info));

    return certificate_hash;
}


void CertificateManager::send_certificate_to_client(CertificateManager::CertificateType certificate_type,
                                                    const ipmi::IpmiController::Ptr controller,
                                                    const ipmi::BridgeInfo& bridge_info) {
    log_info(MANAGER_LOGGER_NAME,
             "Sending " << certificate_type_to_string(certificate_type) << " certificate to client.");
    log_debug(MANAGER_LOGGER_NAME,
              "Sending " << certificate_type_to_string(certificate_type) << " certificate to client at "
                         << bridge_info_to_string(bridge_info) << ".");

    // send no more than 48 bytes at a time
    constexpr const size_t MAX_CHUNK_LENGTH = 48;
    constexpr const std::size_t MAX_RETRIES = 5;

    CertificateData::CertificateDataType certificate{};
    {
        CertificateDataReference reference = get_certificate(certificate_type);
        certificate = (*reference).get_certificate_data();
    }
    const size_t certificate_length = certificate.size();
    size_t remaining_bytes_count = certificate_length;
    unsigned int current_chunk_number = 0;

    ipmi::command::sdv::request::SetAuthorizedCert request{};
    ipmi::command::sdv::response::SetAuthorizedCert response{};
    ipmi::command::sdv::request::SetAuthorizedCert::CertificateChunk certificate_chunk{};

    request.set_certificate_type(certificate_type_to_enum(certificate_type));
    request.set_certificate_length(static_cast<std::uint16_t>(certificate_length));

    while (remaining_bytes_count > 0) {
        log_debug(MANAGER_LOGGER_NAME,
                  "Sending " << certificate_type_to_string(certificate_type) << " certificate (chunk "
                             << current_chunk_number << ") to client at " << bridge_info_to_string(bridge_info)
                             << ". Data: " <<
                             print_buffer(certificate_chunk.data(), certificate_chunk.size()) << "; "
                             << remaining_bytes_count << " bytes remaining.");

        const size_t bytes_count_to_transmit =
            remaining_bytes_count >= MAX_CHUNK_LENGTH ? MAX_CHUNK_LENGTH : remaining_bytes_count;

        certificate_chunk.resize(bytes_count_to_transmit);
        std::copy_n(certificate.end() - remaining_bytes_count, bytes_count_to_transmit, certificate_chunk.begin());

        request.set_current_chunk_number(static_cast<std::uint8_t>(current_chunk_number));
        request.set_certificate_chunk(certificate_chunk);

        for (size_t i = 1; i <= MAX_RETRIES; i++) {
            try {
                controller->send(request, bridge_info, response);
                break;
            }
            catch (const std::exception& e) {
                log_debug(MANAGER_LOGGER_NAME, "Failed to send certificate: " << e.what());
                if (i == MAX_RETRIES) {
                    throw std::runtime_error(std::string("Failed to send certificate: ") + e.what());
                }
                std::this_thread::sleep_for(std::chrono::seconds(2));
            }
        }

        current_chunk_number += 1;
        remaining_bytes_count -= bytes_count_to_transmit;
    }

    log_info(MANAGER_LOGGER_NAME,
             "Sending " << certificate_type_to_string(certificate_type) << " certificate to client finished.");
    log_debug(MANAGER_LOGGER_NAME,
              "Sending " << certificate_type_to_string(certificate_type) << " certificate to client at "
                         << bridge_info_to_string(bridge_info) << " finished.");
}


void CertificateManager::set_cert_file_path(CertificateManager::CertificateType cert_type, const std::string& file_path) {
    CertificateDataReference reference = get_certificate(cert_type);
    CertificateData& certificate_data = *reference;

    // Setting certificate file path is allowed only once to limit security vulnerabilities
    if (!certificate_data.get_certificate_file_path().empty()) {
        throw std::logic_error(
            "Error while setting " + certificate_type_to_string(cert_type) +
            " certificate file path: it can be set only once.");
    }

    certificate_data.set_certificate_file_path(file_path);
}

bool CertificateManager::read_cert_file_and_update(CertificateData& certificate_data) {
    const std::string& certificate_file_path = certificate_data.get_certificate_file_path();
    if (!is_path_absolute(certificate_file_path)) {
        throw std::logic_error("Certificate file path must be absolute.");
    }

    /* file is always binary */
    int fd = ::open(certificate_file_path.c_str(), O_RDONLY);
    if (fd < 0) {
        throw std::logic_error("Cannot open certificate file");
    }

    if (!is_regular_file(fd)) {
        ::close(fd);
        throw std::logic_error("Certificate file must be a regular file.");
    }

    /* check certificate file access time */
    const timespec& file_modification_time = get_file_last_modification_time(fd);
    if (!certificate_data.get_certificate_data().empty()) {
        const timespec& certificate_previous_time = certificate_data.get_last_modification_time();
        if (!file_timestamp_changed(certificate_previous_time, file_modification_time)) {
            ::close(fd);
            return false;
        }
        log_debug(MANAGER_LOGGER_NAME,
            certificate_type_to_string(certificate_data.get_certificate_type()) << " changed at " <<
            std::to_string(file_modification_time.tv_sec) << "." << std::to_string(file_modification_time.tv_nsec));
    }

    /* get file size */
    off_t cert_file_length = ::lseek(fd, 0, SEEK_END);
    if (cert_file_length < 0) {
        ::close(fd);
        throw std::logic_error("Cannot get certificate file size");
    }
    if (cert_file_length == 0) {
        ::close(fd);
        throw std::logic_error("Empty certificate file");
    }
    ::lseek(fd, 0, SEEK_SET);

    CertificateData::CertificateDataType certificate_content{};
    certificate_content.resize(cert_file_length);

    ssize_t cert_bytes = ::read(fd, certificate_content.data(), cert_file_length);
    uint8_t eof; ssize_t end_of_file = ::read(fd, &eof, sizeof(eof));
    ::close(fd);

    if ((cert_bytes != cert_file_length) || (end_of_file != 0)) {
        throw std::logic_error("File modified concurently");
    }

    certificate_data.set_certificate_data(certificate_content);
    certificate_data.set_last_modification_time(file_modification_time);
    calculate_certificate_hash(certificate_data);
    return true;
}


void CertificateManager::calculate_certificate_hash(CertificateData& certificate_data) {
    log_debug(MANAGER_LOGGER_NAME,
              "Calculating " << certificate_type_to_string(certificate_data.get_certificate_type())
                             << " certificate hash.");

    std::vector<uint8_t> result;
    constexpr size_t MD5_HASH_LENGTH = 16;
    result.resize(MD5_HASH_LENGTH);

    MD5_CTX context;
    MD5_Init(&context);
    MD5_Update(&context, certificate_data.get_certificate_data().data(),
               certificate_data.get_certificate_data().size());
    MD5_Final(result.data(), &context);

    log_debug(MANAGER_LOGGER_NAME,
              certificate_type_to_string(certificate_data.get_certificate_type()) << " certificate hash: "
                                                                                  << print_buffer(result.data(),
                                                                                                  result.size()));

    certificate_data.set_certificate_hash(result);
}
