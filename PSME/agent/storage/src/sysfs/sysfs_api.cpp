/*!
 * @section LICENSE
 *
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
 *
 * @section DESCRIPTION
 *
 * @file sysfs_api.cpp
 *
 * @brief C++ wrapper for Libsysfs
 * */
#include "sysfs/sysfs_api.hpp"
#include "agent-framework/logger_ext.hpp"

#include "libsysfs.h"

#include <mntent.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/vfs.h>
#include <sys/ioctl.h>
#include <scsi/sg.h>
#include <linux/hdreg.h>

#include <mutex>
#include <thread>
#include <algorithm>

using namespace agent::storage::sysfs;

constexpr const char SysfsAPI::ROOT_FS[];
constexpr const char SysfsAPI::BOOT_FS[];

constexpr const char SysfsAPI::HardDrive::TYPE_SSD[];
constexpr const char SysfsAPI::HardDrive::TYPE_HDD[];

static SysfsAPI* g_sysfs_api = nullptr;

SysfsAPI* SysfsAPI::get_instance() {
    if (nullptr == g_sysfs_api) {
        g_sysfs_api = new SysfsAPI();
    }

    return g_sysfs_api;
}

SysfsAPI::~SysfsAPI() {}

void SysfsAPI::get_hard_drives(vector<HardDrive>& hard_drives) {
    struct sysfs_class* block = nullptr;
    struct sysfs_device* device = nullptr;
    struct dlist* devices_list = nullptr;
    std::list<std::thread> threads{};
    std::mutex push_back_mutex{};

    auto discover_device = [&](struct sysfs_device* dev) {
        HardDrive hard_drive{};

        m_read_block_device_attributes(dev, hard_drive);
        if (!hard_drive.get_serial_number().empty()){

            std::lock_guard<std::mutex> guard(push_back_mutex);

            hard_drives.push_back(hard_drive);

            log_debug(GET_LOGGER("storage-agent"), "Found block device "
                << hard_drive.get_device_path());
            log_debug(GET_LOGGER("storage-agent"), "Model: "
                << hard_drive.get_model());
            log_debug(GET_LOGGER("storage-agent"), "Capacity: "
                << hard_drive.get_capacity_gb());
        } else {
            log_debug(GET_LOGGER("storage-agent"), "Cannot read parameters directly from device: "
                    <<hard_drive.get_device_path()<<", it might have been recently removed or is not responding.");
        }
    };

    block = sysfs_open_class(SYSFS_BLOCK_NAME);
    if (nullptr == block) {
        return;
    }

    devices_list = sysfs_get_class_devices(block);
    dlist_for_each_data(devices_list, device, struct sysfs_device) {
        if (nullptr != device && !m_is_virtual_device(device)
                && !m_is_boot_device(device)) {

            // discover hard drives simultaneously
            threads.emplace_back(discover_device, device);
        }
    }

    // join all threads
    std::for_each(std::begin(threads), std::end(threads),
            std::mem_fun_ref(&std::thread::join));

    sysfs_close_class(block);

}

void SysfsAPI::get_partitions(const HardDrive& drive,
        vector<Partition>& partitions) {
    struct sysfs_device* device = nullptr;
    struct sysfs_device* child_device = nullptr;
    int partition_nr = 0;
    unsigned long long capacity_bytes = 0;

    log_debug(GET_LOGGER("storage-agent"),
            "Getting partition list of block device /dev/" << drive.get_name());

    device = sysfs_open_device_tree(drive.get_sysfs_path().c_str());
    if (nullptr != device) {
        if (nullptr != device->children) {
            dlist_for_each_data(device->children, child_device, struct sysfs_device) {
                if (nullptr != child_device) {
                    Partition partition;
                    m_read_attribute(partition_nr, child_device, "partition");
                    if (partition_nr) {
                        partition.set_name(child_device->name);
                        partition.set_path(child_device->path);

                        m_read_attribute(capacity_bytes, child_device, "size");
                        partition.set_capacity_gb(
                                double(capacity_bytes) * SECTORS_TO_GB);
                        log_debug(GET_LOGGER("storage-agent"), "Partition /dev/"
                                << child_device->name << " found. Size: "
                                << partition.get_capacity_gb() << " GB");

                        partitions.push_back(partition);
                    }
                    partition_nr = 0;
                }
            }
        }
    }
    sysfs_close_device_tree(device);
}

bool SysfsAPI::m_is_virtual_device(const struct sysfs_device* device) {
    string path{device->path};
    return string::npos != path.find("/virtual/");
}

bool SysfsAPI::m_is_boot_device(const struct sysfs_device* device) {
    if (m_boot_device.empty()) {
        m_detect_boot_device();
    }

    if (!m_boot_device.empty()) {
        string dev_path = m_get_device_path(device);
        if (m_boot_device.find(dev_path) == 0) {
            return true;
        }
    }

    return false;
}

void SysfsAPI::m_detect_boot_device() {
    try {
        struct mntent mount_point;
        char strings[4096];

        FILE* mtab = setmntent("/etc/mtab", "r");

        if (nullptr == mtab) {
            throw std::runtime_error("setmntent");
        }

        m_boot_device = "";
        while (nullptr !=
               getmntent_r(mtab, &mount_point, strings, sizeof(strings))) {

            struct statfs fs;

            if (nullptr == mount_point.mnt_dir ||
                0 != statfs(mount_point.mnt_dir, &fs)) {
                continue;
            }

            string mnt_dir = mount_point.mnt_dir;

            if (BOOT_FS == mnt_dir ||
                (ROOT_FS == mnt_dir && m_boot_device.empty())) {
                m_boot_device = mount_point.mnt_fsname;
            }
        }

        endmntent(mtab);

        if (m_boot_device.empty()) {
            log_warning(GET_LOGGER("storage-agent"),
                        "Cannot detect boot device.");
        }
        else {
            log_debug(GET_LOGGER("storage-agent"), "Detected boot device: "
                      << m_boot_device);
        }
    }
    catch (std::exception& error) {
        log_error(GET_LOGGER("storage-agent"),
                  "Cannot detect boot device: " << error.what());
    }
}

void SysfsAPI::m_read_block_device_attributes(struct sysfs_device* device,
                                              HardDrive& hard_drive) {
    unsigned long long capacity_bytes = 0;
    string model = "";

    m_read_attribute(capacity_bytes, device, "size");
    m_read_attribute(model, device, "device/model");

    m_trim(model);

    hard_drive.set_name(device->name);
    hard_drive.set_sysfs_path(device->path);
    hard_drive.set_device_path(m_get_device_path(device));
    hard_drive.set_capacity_gb(
        double(capacity_bytes) * SECTORS_TO_GB);
    hard_drive.set_model(model);

    m_read_ata_attributes(device, hard_drive);
}

void SysfsAPI::m_read_attribute(string& value,
                                struct sysfs_device* device,
                                const string& attribute_name) {

    try {
        struct sysfs_attribute* attribute =
            sysfs_get_device_attr(device, attribute_name.c_str());

        if (nullptr == attribute) {
            throw std::runtime_error("sysfs_get_device_attr");
        }

        log_debug(GET_LOGGER("storage-agent"),
                  "Sysfs " << attribute_name << " attribute value: "
                  << attribute->value);

        value = attribute->value;
    }
    catch (std::exception& error) {
        log_warning(GET_LOGGER("storage-agent"),
                  "Cannot get sysfs attribute: " << attribute_name << ". "
                  << error.what());
    }
}

void SysfsAPI::m_read_attribute(unsigned long long& value,
                                struct sysfs_device* device,
                                const string& attribute_name) {
    try {
        string string_value = "";

        m_read_attribute(string_value, device, attribute_name);

        if (!string_value.empty()) {
            value = std::stoull(string_value);
        }
    }
    catch (const std::exception& error) {
        log_warning(GET_LOGGER("storage-agent"),
                  "Cannot get sysfs attribute: " << attribute_name << ". "
                  << error.what());
    }
}

void SysfsAPI::m_read_attribute(int& value,
                                struct sysfs_device* device,
                                const string& attribute_name) {
    try {
        string string_value = "";

        m_read_attribute(string_value, device, attribute_name);

        if (!string_value.empty()) {
            value = std::stoi(string_value);
        }

    } catch (const std::exception& error) {
        log_warning(GET_LOGGER("storage-agent"),
                  "Cannot get sysfs attribute: " << attribute_name << ". "
                  << error.what());
    }
}

void SysfsAPI::m_read_ata_attributes(const struct sysfs_device* device,
                                     HardDrive& hard_drive) {

    std::unique_ptr<uint16_t[]> p_hdio_data{};
    try {
        uint32_t rpm{};
        string serial_number{};
        string manufacturer{};

        try {
            m_perform_sg_io(device, p_hdio_data);
        }
        catch (const std::exception& error) {
            log_warning(GET_LOGGER("storage-agent"),
                        "Cannot perform SG_IO ioctl. " <<
                        "Falling back to HDIO_DRIVE_CMD. " << error.what());
            m_perform_hdio_drive_cmd(device, p_hdio_data);
        }

        m_read_rpm(rpm, p_hdio_data);
        m_read_serial_number(serial_number, p_hdio_data);
        m_read_manufacturer(manufacturer, p_hdio_data);

        if (SSD_RPM_VALUE == rpm) {
            hard_drive.set_type(HardDrive::TYPE_SSD);
        }
        else {
            hard_drive.set_rpm(rpm);
            hard_drive.set_type(HardDrive::TYPE_HDD);
        }

        hard_drive.set_serial_number(serial_number);
        hard_drive.set_manufacturer(manufacturer);
        hard_drive.set_interface("");
    }
    catch (const std::exception& error) {
        log_error(GET_LOGGER("storage-agent"),
                  "Cannot read drive attributes: " << error.what());
    }
}

void SysfsAPI::m_perform_hdio_drive_cmd(const struct sysfs_device* device, std::unique_ptr<uint16_t[]>& p_hdio_data) {

    static constexpr uint8_t ATA_OP_IDENTIFY = 0xec;
    static constexpr uint8_t ATA_OP_PIDENTIFY = 0xa1;

    string dev_path = m_get_device_path(device);

    if (nullptr == p_hdio_data.get()) {
        p_hdio_data.reset(new uint16_t[ATA_DATA_SIZE/2]);
    }

    int fd = open(dev_path.c_str(), O_RDONLY | O_NONBLOCK);
    if (0 > fd) {
        throw std::runtime_error("open");
    }

    memset(p_hdio_data.get(), 0, ATA_DATA_SIZE);

    uint8_t* hdio_data = reinterpret_cast<uint8_t*>(p_hdio_data.get());
    hdio_data[0] = ATA_OP_IDENTIFY;
    hdio_data[3] = 1;
    if (0 != ioctl(fd, HDIO_DRIVE_CMD, hdio_data)) {
        memset(p_hdio_data.get(), 0, ATA_DATA_SIZE);
        hdio_data[0] = ATA_OP_PIDENTIFY;
        hdio_data[3] = 1;
        if (0 != ioctl(fd, HDIO_DRIVE_CMD, hdio_data)) {
            close(fd);
            throw std::runtime_error("ioctl(HDIO_DRIVE_CMD)");
        }
    }

    close(fd);
}

void SysfsAPI::m_perform_sg_io(const struct sysfs_device* device, std::unique_ptr<uint16_t[]>& p_hdio_data) {

    static constexpr const size_t COMMAND_BUFFER_SIZE = 16;
    static constexpr const size_t SENSE_BUFFER_SIZE = 32;
    static constexpr const uint32_t SG_IO_TIMEOUT = 1000;
    static constexpr const uint8_t ATA_OP_IDENTIFY = 0xec;
    static constexpr const uint8_t SG_ATA_16 = 0x85;
    static constexpr const uint8_t SG_ATA_PROTO_PIO_IN = 0x08;
    static constexpr const uint8_t SG_CDB2_TLEN_NSECT = 0x02;
    static constexpr const uint8_t SG_CDB2_TLEN_SECTORS = 0x04;
    static constexpr const uint8_t SG_CDB2_TDIR_FROM_DEV = 0x08;
    static constexpr const uint8_t ATA_USING_LBA = 0x40;

    string dev_path = m_get_device_path(device);

    if (nullptr == p_hdio_data.get()) {
        p_hdio_data.reset(new uint16_t[ATA_DATA_SIZE/2]);
    }

    uint8_t command_buffer[COMMAND_BUFFER_SIZE];
    uint8_t sense_buffer[SENSE_BUFFER_SIZE];
    uint8_t* data_buffer = reinterpret_cast<uint8_t*>(p_hdio_data.get() + 2);
    sg_io_hdr_t io_hdr;

    int fd = open(dev_path.c_str(), O_RDONLY | O_NONBLOCK);
    if (0 > fd) {
        throw std::runtime_error("open");
    }

    memset(&io_hdr, 0, sizeof(io_hdr));
    memset(command_buffer, 0, COMMAND_BUFFER_SIZE);
    memset(sense_buffer, 0, SENSE_BUFFER_SIZE);
    memset(p_hdio_data.get(), 0, ATA_DATA_SIZE);

    command_buffer[0] = SG_ATA_16;
    command_buffer[1] = SG_ATA_PROTO_PIO_IN;
    command_buffer[2] =
        SG_CDB2_TLEN_NSECT | SG_CDB2_TLEN_SECTORS | SG_CDB2_TDIR_FROM_DEV;
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

void SysfsAPI::m_read_ata_string(string& value, size_t offset, size_t length, std::unique_ptr<uint16_t[]>& p_hdio_data) {
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

void SysfsAPI::m_read_serial_number(string& serial_number, std::unique_ptr<uint16_t[]>& p_hdio_data) {
    static constexpr const uint32_t SERIAL_OFFSET = 12;
    static constexpr const uint32_t SERIAL_LENGTH = 10;

    m_read_ata_string(serial_number, SERIAL_OFFSET, SERIAL_LENGTH, p_hdio_data);
    log_debug(GET_LOGGER("storage-agent"), "Serial number: " <<
              serial_number);
}

void SysfsAPI::m_read_manufacturer(string& manufacturer, std::unique_ptr<uint16_t[]>& p_hdio_data) {
    static constexpr const uint32_t MANUFACTURER_OFFSET = 198;
    static constexpr const uint32_t MANUFACTURER_LENGTH = 10;

    m_read_ata_string(manufacturer, MANUFACTURER_OFFSET, MANUFACTURER_LENGTH,  p_hdio_data);
}

void SysfsAPI::m_read_rpm(uint32_t& rpm, std::unique_ptr<uint16_t[]>& p_hdio_data) {
    /* Nominal Media Rotation Rate */
    static constexpr uint32_t NMRR = 219;

    rpm = p_hdio_data[NMRR];
    log_debug(GET_LOGGER("storage-agent"), "Nominal Media Rotation Rate: " <<
              rpm);
}

string SysfsAPI::m_get_device_path(const struct sysfs_device* device) {
    return string("/dev/") + device->name;
}

void SysfsAPI::m_trim(string& input_string) {
    static constexpr char WHITE_CHARS[] = " \t\n\r";

    input_string.erase(0, input_string.find_first_not_of(WHITE_CHARS));
    if (!input_string.empty()) {
        input_string.erase(input_string.find_last_not_of(WHITE_CHARS) + 1);
    }
}
