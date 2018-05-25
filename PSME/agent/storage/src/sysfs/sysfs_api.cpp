/*!
 * @copyright
 * Copyright (c) 2015-2018 Intel Corporation
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
 * @file sysfs_api.cpp
 *
 * @brief C++ wrapper for sysfs operations
 * */

#include "sysfs/sysfs_api.hpp"
#include "agent-framework/logger_ext.hpp"
#include "agent-framework/module/utils/is_ascii_string.hpp"

#include "sysfs/sysfs_interface.hpp"

#include <safe-string/safe_lib.hpp>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <scsi/sg.h>
#include <linux/hdreg.h>

#include <mutex>
#include <thread>
#include <algorithm>

using namespace agent::storage;
using namespace agent_framework::model::utils;

constexpr const char SysfsApi::DEV_FS[];
constexpr const char SysfsApi::MTAB_FILE[];
constexpr const char SysfsApi::VIRTUAL[];
constexpr const char SysfsApi::SIZE[];
constexpr const char SysfsApi::DEVICE_MODEL[];
constexpr const char SysfsApi::DEVICE_TYPE[];
constexpr const char SysfsApi::SYSFS_BLOCK_CLASS[];

constexpr const char SysfsApi::HardDrive::TYPE_SSD[];
constexpr const char SysfsApi::HardDrive::TYPE_HDD[];

static SysfsApi* g_sysfs_api = nullptr;

SysfsApi* SysfsApi::get_instance() {
    if (nullptr == g_sysfs_api) {
        g_sysfs_api = new SysfsApi(std::make_shared<sysfs::SysfsInterface>());
    }

    return g_sysfs_api;
}

SysfsApi::SysfsApi(std::shared_ptr<sysfs::AbstractSysfsInterface> sysfs_interface)
    : m_sysfs(sysfs_interface) {

    m_mount_points = m_sysfs->get_mounts(MTAB_FILE);
}


SysfsApi::~SysfsApi() {}


std::vector<SysfsApi::HardDrive> SysfsApi::get_hard_drives() {
    std::vector<HardDrive> hard_drives{};
    std::list<std::thread> threads{};
    std::mutex push_back_mutex{};

    auto discover_device = [&](const sysfs::Path& device_path) {
        try {
            auto link = m_sysfs->get_link(device_path);
            auto path = device_path;
            path.pop_right();
            auto device = m_sysfs->get_dir(path / link.value);
            if (!m_is_virtual_device(device) && m_is_direct_access_device(device) && !m_is_boot_device(device)) {
                std::this_thread::sleep_for(std::chrono::seconds(3)); // waiting for devices to stabilize
                HardDrive hard_drive{};
                m_read_block_device_attributes(device, hard_drive);
                if (!hard_drive.get_serial_number().empty()){
                    log_debug("storage-agent", "Found block device " << hard_drive.get_device_path());
                    log_debug("storage-agent", "Model: " << hard_drive.get_model());
                    log_debug("storage-agent", "Capacity: " << hard_drive.get_capacity_gb());

                    std::lock_guard<std::mutex> guard(push_back_mutex);
                    hard_drives.push_back(hard_drive);
                } else {
                    log_info("storage-agent", "Cannot read parameters directly from device: "
                        << hard_drive.get_device_path() << ", it might have been recently removed or is not responding.");
                }
            }
        }
        catch (const std::exception& ex) {
            log_info("storage-agent", "Could not read block device " << device_path << ": " << ex.what());
        }
    };

    sysfs::SysfsDir block_devices;
    try {
        block_devices = m_sysfs->get_dir(SYSFS_BLOCK_CLASS);
    }
    catch (const std::exception& ex) {
        log_info("storage-agent", "Could not read block devices: " << ex.what());
        return {};
    }

    for (const auto& device_path : block_devices.links) {
        // discover hard drives simultaneously
        threads.emplace_back(discover_device, device_path);
    }

    // join all threads
    std::for_each(std::begin(threads), std::end(threads), std::mem_fun_ref(&std::thread::join));
    return hard_drives;
}



bool SysfsApi::m_is_virtual_device(const sysfs::SysfsDir& device) {
    if (device.path.to_string().find(VIRTUAL) != std::string::npos) {
        log_info("storage-agent", "Device " << m_get_devfs_path(device) << " is a virtual device.");
        return true;
    }
    return false;
}


bool SysfsApi::m_is_direct_access_device(const sysfs::SysfsDir& device) {
    try {
        auto type_string = m_sysfs->get_file(device.path / DEVICE_TYPE).value;
        unsigned long long type = std::stoull(type_string);

        if (type != DIRECT_ACCESS_TYPE) {
            log_info("storage-agent", "Device " << m_get_devfs_path(device) << " has type: " << std::hex << type);
            return false;
        }
        return true;
    }
    catch (const std::exception& ex) {
        log_info("storage-agent", "Could not read the type of device "
            << m_get_devfs_path(device) << ": " << ex.what());
        return false;
    }
}


bool SysfsApi::m_is_boot_device(const sysfs::SysfsDir& device) {
    for (const auto& mount : m_mount_points) {
        if (mount.name == m_get_devfs_path(device)) {
            log_info("storage-agent", "Device " << m_get_devfs_path(device) << " is a boot device.");
            return true;
        }
    }
    return false;
}


void SysfsApi::m_read_block_device_attributes(const sysfs::SysfsDir& device, HardDrive& hard_drive) {
    hard_drive.set_name(device.name);
    hard_drive.set_device_path(m_get_devfs_path(device));

    try {
        auto capacity_string = m_sysfs->get_file(device.path / SIZE).value;
        unsigned long long capacity_bytes = std::stoull(capacity_string);
        hard_drive.set_capacity_gb(double(capacity_bytes) * SECTORS_TO_GB);
    }
    catch (const std::exception& ex) {
        log_debug("storage-agent", "Encountered an error while reading " << m_get_devfs_path(device)
                                                                         << " attributes: " << ex.what());
    }

    m_read_ata_attributes(device, hard_drive);
}


void SysfsApi::m_read_ata_attributes(const sysfs::SysfsDir& device, HardDrive& hard_drive) {

    std::unique_ptr<std::uint16_t[]> p_hdio_data{};
    try {
        std::uint32_t rpm{};
        std::string serial_number{};
        std::string manufacturer{};
        std::string model{};

        try {
            m_perform_sg_io(device, p_hdio_data);
        }
        catch (const std::exception& error) {
            log_warning("storage-agent", "Cannot perform SG_IO ioctl. "
                << "Falling back to HDIO_DRIVE_CMD. " << error.what());
            m_perform_hdio_drive_cmd(device, p_hdio_data);
        }

        m_read_rpm(rpm, p_hdio_data);
        m_read_serial_number(serial_number, p_hdio_data);
        m_read_model_number(model, p_hdio_data);
        m_read_manufacturer(manufacturer, p_hdio_data);

        if (SSD_RPM_VALUE == rpm) {
            hard_drive.set_type(HardDrive::TYPE_SSD);
        }
        else {
            hard_drive.set_rpm(rpm);
            hard_drive.set_type(HardDrive::TYPE_HDD);
        }

        if (is_ascii_string(serial_number)) {
            hard_drive.set_serial_number(serial_number);
        }
        else {
            log_warning("storage-agent", "Serial Number for " << m_get_devfs_path(device)
                                                            << " drive contains invalid characters: " << serial_number);
        }
        if (is_ascii_string(model)) {
            hard_drive.set_model(model);
        }
        else {
            log_warning("storage-agent", "Model Number for " << m_get_devfs_path(device)
                                                              << " drive contains invalid characters: " << model);
        }
        if (is_ascii_string(manufacturer)) {
            hard_drive.set_manufacturer(manufacturer);
        }
        else {
            log_warning("storage-agent", "Manufacturer for " << m_get_devfs_path(device)
                                                             << " drive contains invalid characters: " << manufacturer);
        }
        hard_drive.set_interface("");
    }
    catch (const std::exception& error) {
        log_error("storage-agent", "Cannot read drive attributes: " << error.what());
    }
}


void SysfsApi::m_perform_hdio_drive_cmd(const sysfs::SysfsDir& device, std::unique_ptr<uint16_t[]>& p_hdio_data) {

    static constexpr std::uint8_t ATA_OP_IDENTIFY = 0xec;
    static constexpr std::uint8_t ATA_OP_PIDENTIFY = 0xa1;

    std::string dev_path = m_get_devfs_path(device);

    if (nullptr == p_hdio_data.get()) {
        p_hdio_data.reset(new uint16_t[ATA_DATA_SIZE/2]);
    }

    int fd = open(dev_path.c_str(), O_RDONLY | O_NONBLOCK);
    if (0 > fd) {
        throw std::runtime_error("open");
    }

    memset_s(p_hdio_data.get(), ATA_DATA_SIZE, 0);

    std::uint8_t* hdio_data = reinterpret_cast<std::uint8_t*>(p_hdio_data.get());
    hdio_data[0] = ATA_OP_IDENTIFY;
    hdio_data[3] = 1;
    if (0 != ioctl(fd, HDIO_DRIVE_CMD, hdio_data)) {
        memset_s(p_hdio_data.get(), ATA_DATA_SIZE, 0);
        hdio_data[0] = ATA_OP_PIDENTIFY;
        hdio_data[3] = 1;
        if (0 != ioctl(fd, HDIO_DRIVE_CMD, hdio_data)) {
            close(fd);
            throw std::runtime_error("ioctl(HDIO_DRIVE_CMD)");
        }
    }

    close(fd);
}


void SysfsApi::m_perform_sg_io(const sysfs::SysfsDir& device, std::unique_ptr<uint16_t[]>& p_hdio_data) {

    static constexpr const std::size_t COMMAND_BUFFER_SIZE = 16;
    static constexpr const std::size_t SENSE_BUFFER_SIZE = 32;
    static constexpr const std::uint32_t SG_IO_TIMEOUT = 1000;
    static constexpr const std::uint8_t ATA_OP_IDENTIFY = 0xec;
    static constexpr const std::uint8_t SG_ATA_16 = 0x85;
    static constexpr const std::uint8_t SG_ATA_PROTO_PIO_IN = 0x08;
    static constexpr const std::uint8_t SG_CDB2_TLEN_NSECT = 0x02;
    static constexpr const std::uint8_t SG_CDB2_TLEN_SECTORS = 0x04;
    static constexpr const std::uint8_t SG_CDB2_TDIR_FROM_DEV = 0x08;
    static constexpr const std::uint8_t ATA_USING_LBA = 0x40;

    std::string dev_path = m_get_devfs_path(device);

    if (nullptr == p_hdio_data.get()) {
        p_hdio_data.reset(new uint16_t[ATA_DATA_SIZE/2]);
    }

    std::uint8_t command_buffer[COMMAND_BUFFER_SIZE];
    std::uint8_t sense_buffer[SENSE_BUFFER_SIZE];
    std::uint8_t* data_buffer = reinterpret_cast<uint8_t*>(p_hdio_data.get() + 2);
    sg_io_hdr_t io_hdr;

    int fd = open(dev_path.c_str(), O_RDONLY | O_NONBLOCK);
    if (0 > fd) {
        throw std::runtime_error("open");
    }

    memset_s(&io_hdr, sizeof(io_hdr), 0);
    memset_s(command_buffer, COMMAND_BUFFER_SIZE, 0);
    memset_s(sense_buffer, SENSE_BUFFER_SIZE, 0);
    memset_s(p_hdio_data.get(), ATA_DATA_SIZE, 0);

    command_buffer[0] = SG_ATA_16;
    command_buffer[1] = SG_ATA_PROTO_PIO_IN;
    command_buffer[2] = SG_CDB2_TLEN_NSECT | SG_CDB2_TLEN_SECTORS | SG_CDB2_TDIR_FROM_DEV;
    command_buffer[6] = 1;     // number of sectors
    command_buffer[13] = ATA_USING_LBA;
    command_buffer[14] = ATA_OP_IDENTIFY;

    io_hdr.interface_id = 'S';
    io_hdr.cmd_len = COMMAND_BUFFER_SIZE;
    io_hdr.mx_sb_len = SENSE_BUFFER_SIZE;
    io_hdr.dxfer_direction = SG_DXFER_FROM_DEV;
    io_hdr.dxfer_len = ATA_DATA_SIZE;
    io_hdr.dxferp = data_buffer;
    io_hdr.cmdp = command_buffer;
    io_hdr.sbp = sense_buffer;
    io_hdr.timeout = SG_IO_TIMEOUT;

    if (0 != ioctl(fd, SG_IO, &io_hdr)) {
        close(fd);
        throw std::runtime_error("ioctl(SG_IO)");
    }

    close(fd);
}


void SysfsApi::m_read_ata_string(std::string& value, std::size_t offset, std::size_t length,
                                 std::unique_ptr<std::uint16_t[]>& p_hdio_data) {
    if (0 == p_hdio_data[offset]) {
        return;
    }
    value.resize(length * 2);
    for (size_t pos = 0; pos < length; ++pos) {
        uint16_t data_word = p_hdio_data[offset + pos];
        value[pos * 2] = static_cast<char>((data_word >> 8) & 0xff);
        value[pos * 2 + 1] = static_cast<char>(data_word & 0xff);
    }
    m_trim(value);
}


void SysfsApi::m_read_serial_number(std::string& serial_number, std::unique_ptr<std::uint16_t[]>& p_hdio_data) {
    static constexpr const std::uint32_t SERIAL_OFFSET = 12;
    static constexpr const std::uint32_t SERIAL_LENGTH = 10;

    m_read_ata_string(serial_number, SERIAL_OFFSET, SERIAL_LENGTH, p_hdio_data);
    log_debug("storage-agent", "Serial number: " << serial_number);
}

void SysfsApi::m_read_model_number(std::string& model_number, std::unique_ptr<std::uint16_t[]>& p_hdio_data) {
    static constexpr const std::uint32_t MODEL_OFFSET = 29;
    static constexpr const std::uint32_t MODEL_LENGTH = 20;

    m_read_ata_string(model_number, MODEL_OFFSET, MODEL_LENGTH, p_hdio_data);
    log_debug("storage-agent", "Model: " << model_number);
}


void SysfsApi::m_read_manufacturer(std::string& manufacturer, std::unique_ptr<std::uint16_t[]>& p_hdio_data) {
    static constexpr const std::uint32_t MANUFACTURER_OFFSET = 198;
    static constexpr const std::uint32_t MANUFACTURER_LENGTH = 10;

    m_read_ata_string(manufacturer, MANUFACTURER_OFFSET, MANUFACTURER_LENGTH,  p_hdio_data);
}


void SysfsApi::m_read_rpm(std::uint32_t& rpm, std::unique_ptr<std::uint16_t[]>& p_hdio_data) {
    /* Nominal Media Rotation Rate */
    static constexpr std::uint32_t NMRR = 219;

    rpm = p_hdio_data[NMRR];
    log_debug("storage-agent", "Nominal Media Rotation Rate: " << rpm);
}


sysfs::Path SysfsApi::m_get_devfs_path(const sysfs::SysfsDir& device) {
    return sysfs::Path(DEV_FS) / device.name;
}

void SysfsApi::m_trim(std::string& input_string) {
    static constexpr char WHITE_CHARS[] = " \t\n\r";

    input_string.erase(0, input_string.find_first_not_of(WHITE_CHARS));
    if (!input_string.empty()) {
        input_string.erase(input_string.find_last_not_of(WHITE_CHARS) + 1);
    }
}
