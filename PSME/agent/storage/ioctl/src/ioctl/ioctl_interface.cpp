/*!
 * @copyright Copyright (c) 2018-2019 Intel Corporation
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
 * @file ioctl_interface.cpp
 */

#include "agent-framework/module/utils/utils.hpp"
#include "agent-framework/module/utils/is_ascii_string.hpp"

#include "ioctl/ioctl_interface.hpp"
#include "logger/logger.hpp"
#include "utils/string_utils.hpp"

#include <safe-string/safe_lib.hpp>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/hdreg.h>
#include <scsi/sg.h>


using namespace agent::storage::ioctl;

namespace {

const constexpr char DEV_FS[] = "/dev/";
const constexpr std::uint32_t ATA_DATA_SIZE = 512 + 4;

}

void IoctlInterface::perform_sg_io_cmd(HdioDataPtr& hdio_data) {
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
    static constexpr const std::uint8_t NUMBER_OF_SECTIONS = 1;
    static constexpr const char INTERFACE_ID = 'S';

    std::string dev_path = DEV_FS + m_device_name;

    if (nullptr == hdio_data.get()) {
        hdio_data.reset(new std::uint16_t[ATA_DATA_SIZE / 2]);
    }

    std::uint8_t command_buffer[COMMAND_BUFFER_SIZE];
    std::uint8_t sense_buffer[SENSE_BUFFER_SIZE];
    std::uint8_t* data_buffer = reinterpret_cast<std::uint8_t*>(hdio_data.get() + 2);
    sg_io_hdr_t io_hdr{};

    int fd = open(dev_path.c_str(), O_RDONLY | O_NONBLOCK);
    if (fd < 0) {
        throw std::runtime_error("Could not open " + dev_path);
    }

    memset_s(&io_hdr, sizeof(io_hdr), 0);
    memset_s(command_buffer, COMMAND_BUFFER_SIZE, 0);
    memset_s(sense_buffer, SENSE_BUFFER_SIZE, 0);
    memset_s(hdio_data.get(), ATA_DATA_SIZE, 0);

    command_buffer[0] = SG_ATA_16;
    command_buffer[1] = SG_ATA_PROTO_PIO_IN;
    command_buffer[2] = SG_CDB2_TLEN_NSECT | SG_CDB2_TLEN_SECTORS | SG_CDB2_TDIR_FROM_DEV;
    command_buffer[6] = NUMBER_OF_SECTIONS;
    command_buffer[13] = ATA_USING_LBA;
    command_buffer[14] = ATA_OP_IDENTIFY;

    io_hdr.interface_id = INTERFACE_ID;
    io_hdr.cmd_len = COMMAND_BUFFER_SIZE;
    io_hdr.mx_sb_len = SENSE_BUFFER_SIZE;
    io_hdr.dxfer_direction = SG_DXFER_FROM_DEV;
    io_hdr.dxfer_len = ATA_DATA_SIZE;
    io_hdr.dxferp = data_buffer;
    io_hdr.cmdp = command_buffer;
    io_hdr.sbp = sense_buffer;
    io_hdr.timeout = SG_IO_TIMEOUT;

    if (0 != ::ioctl(fd, SG_IO, &io_hdr)) {
        close(fd);
        throw std::runtime_error("Could not run ioctl 'SG_IO' command.");
    }

    close(fd);
}

void IoctlInterface::perform_hdio_drive_cmd(HdioDataPtr& hdio_data) {
    static constexpr std::uint8_t ATA_OP_IDENTIFY = 0xec;
    static constexpr std::uint8_t ATA_OP_PIDENTIFY = 0xa1;
    static constexpr std::uint8_t SECTOR = 1;

    std::string dev_path = DEV_FS + m_device_name;

    if (nullptr == hdio_data.get()) {
        hdio_data.reset(new std::uint16_t[ATA_DATA_SIZE / 2]);
    }

    int fd = open(dev_path.c_str(), O_RDONLY | O_NONBLOCK);
    if (fd < 0) {
        throw std::runtime_error("Could not open " + dev_path);
    }

    memset_s(hdio_data.get(), ATA_DATA_SIZE, 0);

    std::uint8_t* hdio_data_raw = reinterpret_cast<std::uint8_t*>(hdio_data.get());
    hdio_data_raw[0] = ATA_OP_IDENTIFY;
    hdio_data_raw[3] = SECTOR;

    if (0 != ::ioctl(fd, HDIO_DRIVE_CMD, hdio_data_raw)) {
        memset_s(hdio_data.get(), ATA_DATA_SIZE, 0);
        hdio_data_raw[0] = ATA_OP_PIDENTIFY;
        hdio_data_raw[3] = SECTOR;
        if (0 != ::ioctl(fd, HDIO_DRIVE_CMD, hdio_data_raw)) {
            close(fd);
            throw std::runtime_error("Could not run ioctl 'HDIO_DRIVE_CMD' command.");
        }
    }

    close(fd);
}


IoctlInterface::HdioDataPtr IoctlInterface::read_ata_attributes() {
    HdioDataPtr hdio_data{};

    try {
        perform_sg_io_cmd(hdio_data);
    }
    catch (const std::exception& exception) {
        log_warning("ioctl", "Cannot perform SG_IO ioctl. Falling back to HDIO_DRIVE_CMD. " << exception.what());

        try {
            perform_hdio_drive_cmd(hdio_data);
        }
        catch (const std::exception& hdio_exception) {
            log_error("ioctl", "HDIO_DRIVE_CMD failed: " << hdio_exception.what());
            return nullptr;
        }
    }

    return hdio_data;
}

OptionalField<std::string> IoctlInterface::get_serial_number(const HdioDataPtr& hdio_data) {
    static constexpr const std::uint32_t SERIAL_OFFSET = 12;
    static constexpr const std::uint32_t SERIAL_LENGTH = 10;

    return read_ata_string(SERIAL_OFFSET, SERIAL_LENGTH, hdio_data);
}

OptionalField<std::string> IoctlInterface::get_model_number(const HdioDataPtr& hdio_data) {
    static constexpr const std::uint32_t MODEL_OFFSET = 29;
    static constexpr const std::uint32_t MODEL_LENGTH = 20;

    return read_ata_string(MODEL_OFFSET, MODEL_LENGTH, hdio_data);
}

OptionalField<std::uint32_t> IoctlInterface::get_rpm(const HdioDataPtr& hdio_data) {
    static const constexpr std::uint32_t NMRR = 219; /* Nominal Media Rotation Rate */

    if (hdio_data == nullptr) {
        return {};
    }
    else {
        return hdio_data[NMRR];
    }
}

OptionalField<std::string> IoctlInterface::get_manufacturer(const HdioDataPtr& hdio_data) {
    static constexpr const std::uint32_t MANUFACTURER_OFFSET = 198;
    static constexpr const std::uint32_t MANUFACTURER_LENGTH = 10;

    return read_ata_string(MANUFACTURER_OFFSET, MANUFACTURER_LENGTH, hdio_data);
}

OptionalField<std::string> IoctlInterface::read_ata_string(std::size_t offset,
                                                           std::size_t length,
                                                           const HdioDataPtr& hdio_data) {
    if (hdio_data == nullptr || 0 == hdio_data[offset]) {
        return {};
    }

    std::string value{};
    value.resize(length * 2);
    for (std::size_t i = 0; i < length; ++i) {
        std::uint16_t data_word = hdio_data[offset + i];
        value[i * 2] = static_cast<char>((data_word >> 8) & 0xff);
        value[i * 2 + 1] = static_cast<char>(data_word & 0xff);
    }
    value = utils::StringUtils::trim(value);

    if (agent_framework::model::utils::is_ascii_string(value)) {
        return value;
    }
    else {
        log_warning("storage-agent", "HDIO data for " <<
            m_device_name << " drive contains invalid characters: " << value);
        return {};
    }
}

