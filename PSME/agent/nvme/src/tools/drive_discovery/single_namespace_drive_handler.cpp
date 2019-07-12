/*!
 * @copyright Copyright (c) 2017-2019 Intel Corporation
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
 * @file single_namespace_drive_handler.cpp
 */

#include "tools/drive_discovery/single_namespace_drive_handler.hpp"
#include "nvme/nvme_exception.hpp"
#include "nvme/utils.hpp"
#include "partition/gpt_partition_table.hpp"
#include "logger/logger_factory.hpp"
#include "agent-framework/module/utils/to_hex_string.hpp"

#include <regex>



using namespace agent::nvme::tools;
using namespace nvme;
using namespace partition;
using namespace agent_framework::model::utils;

namespace {

constexpr char NAMESPACE_AFFIX[] = "n";
constexpr char PARTITION_AFFIX[] = "p";

constexpr unsigned ERASE_BYTES_PER_SINGLE_WRITE = 10 * 1024 * 1024; // bytes to write during single write operation

constexpr size_t MODEL_NUMBER_LENGTH = 40;
constexpr size_t SERIAL_NUMBER_LENGTH = 20;
constexpr size_t FIRMWARE_REVISION_LENGTH = 8;


std::string convert_uint8_to_string(const uint8_t* array, size_t size) {
    std::stringstream ss{};
    for (size_t i = 0; i < size; ++i) {
        if (array[i] == '\0') {
            break;
        }
        ss << array[i];
    }
    std::string s = ss.str();
    std::size_t first = s.find_first_not_of(' ');
    std::size_t last = s.find_last_not_of(' ');
    if (std::string::npos == first || std::string::npos == last || first > last) {
        return "";
    }
    return s.substr(first, last - first + 1);

}

std::string partition_id_to_volume_name(const std::string& name, unsigned partition_id) {
    // on the hardware, partition ids start from 0; in the system numbering starts from 1
    // for a drive 'nvme3' and partition id 4, we want to get 'nvme3n1p5'
    std::stringstream str{};
    str << name << PARTITION_AFFIX << (partition_id + 1);
    return str.str();
}


unsigned
volume_name_to_partition_id(const std::string& drive_name, const std::string& volume_name) {

    std::regex expr(drive_name + PARTITION_AFFIX + "([0-9]*)");
    static constexpr unsigned EXPECTED_MATCHES = 2;
    std::smatch match{};
    if (!std::regex_match(volume_name, match, expr) || match.size() != EXPECTED_MATCHES) {
        throw std::runtime_error(std::string{"Invalid volume name format: "} + volume_name);
    }
    long id = std::stol(match[1].str(), nullptr);
    if (id <= 0) {
        throw std::runtime_error(std::string{"Invalid volume name format: "} + volume_name);
    }
    return unsigned(id - 1);
}


long double uint128_to_long_double(const uint64_t value[2]) {
    long double ret{};
    static constexpr int size = 16;
    const uint8_t* bytes = reinterpret_cast<const uint8_t*>(value);
    for (int i = 0; i < size; ++i) {
        ret *= 256;
        ret += bytes[size - 1 - i];
    }

    return ret;
}


uint64_t integer_divide_and_ceil(uint64_t a, uint64_t b) {
    if (b == 0) {
        throw std::runtime_error("Division by zero!");
    }
    // this is a trick to make integer division with ceil operation without using floating point numbers
    return (a + b - 1) / b;
}


void force_os_update_partition_table(const std::string& device_name) {
    // TODO: implement it without system
    std::string command = "partprobe /dev/" + device_name;
    if (::system(command.c_str()) != 0) {
        throw std::runtime_error("Unable to force OS partition table update.");
    }
}


void set_latency_data(BaseDriveHandler::LatencyData::HistogramGroup& data,
                      const OptionalField<std::uint32_t>& min_range_us,
                      const OptionalField<std::uint32_t>& max_range_us,
                      const OptionalField<std::uint32_t>& step,
                      std::uint32_t buckets[],
                      std::size_t buckets_count) {
    data.min_range_us = min_range_us;
    data.max_range_us = max_range_us;
    data.step_us = step;
    data.buckets = std::vector<std::uint32_t>(buckets, buckets + buckets_count);
}

}


SingleNamespaceDriveHandler::~SingleNamespaceDriveHandler() {}


void SingleNamespaceDriveHandler::load() {
    std::lock_guard<std::mutex> lock{m_mutex};

    static constexpr uint32_t MAX_LBA = 0xFFFFFFFF;

    try {
        log_debug("drive-handler", "Loading drive data: " << m_name);

        // get NVMe drive data
        m_namespace_id = get_primary_namespace_id();
        m_controller_data = m_nvme_interface->get_controller_info(m_name);
        m_namespace_data = m_nvme_interface->get_namespace_info(m_name, m_namespace_id);
        uint64_t lba_size = get_formatted_lba_data_size(m_namespace_data);
        if (lba_size > MAX_LBA) {
            throw std::runtime_error(std::string{"Unsupported LBA size: "} + std::to_string(lba_size));
        }
        m_lba_size = uint32_t(lba_size);

        try {
            auto result = m_nvme_interface->get_latency_tracking_feature(m_name, m_namespace_id);
            m_latency_tracking_feature_enabled = bool(result);
            log_debug("drive-handler", "Latency tracking for " << m_name
                << " is enabled: " << std::boolalpha << m_latency_tracking_feature_enabled.value());
        }
        catch (const std::exception& e) {
            log_warning("drive-handler", "Failed to get-feature of '" << m_name
                << "'. Extended telemetry may not be supported by this drive. " << e.what());
            m_latency_tracking_feature_enabled.reset();
        }

        // read partition table
        m_partition_table = std::make_shared<GptPartitionTable>(m_lba_size);
        try {
            // try reading the table GPT
            m_partition_table->load(m_name, m_drive_interface);
        }
        catch (const std::exception& e) {
            log_warning("drive-handler", "Unable to read GPT on drive " << m_name << ", resetting GPT.");
            m_partition_table->reset(m_namespace_data.size);
            log_debug("drive-handler", "\tReloading...");
            m_partition_table->save(m_name, m_drive_interface);
            m_partition_table->load(m_name, m_drive_interface);
            force_os_update_partition_table(m_name);
        }
    }
    catch (const NvmeException& e) {
        log_error("drive-handler", "NVMe exception while reading drive data (" << m_name << "): " << e.what());
    }
    catch (const std::exception& e) {
        log_error("drive-handler", "Exception while reading drive data (" << m_name << "): " << e.what());
    }
}


SingleNamespaceDriveHandler::DriveData SingleNamespaceDriveHandler::get_drive_data() const {
    std::lock_guard<std::mutex> lock{m_mutex};
    DriveData dd{};
    dd.block_size_bytes = m_lba_size;
    dd.size_lba = m_namespace_data.size;
    dd.available_size_lba = m_partition_table->get_max_usable_blocks();
    dd.model_number = convert_uint8_to_string(m_controller_data.model_number, ::MODEL_NUMBER_LENGTH);
    dd.serial_number = convert_uint8_to_string(m_controller_data.serial_number, ::SERIAL_NUMBER_LENGTH);
    dd.firmware_revision = convert_uint8_to_string(m_controller_data.firmware_revision, ::FIRMWARE_REVISION_LENGTH);
    dd.manufacturer_number = to_hex_string<2>(m_controller_data.pci_vendor_id);
    dd.features.latency_tracking_enabled = m_latency_tracking_feature_enabled;
    dd.namespaces = {m_namespace_id};
    return dd;
}


std::vector<std::string> SingleNamespaceDriveHandler::get_volumes() const {
    std::lock_guard<std::mutex> lock{m_mutex};
    if (!m_partition_table) {
        return {};
    }
    std::vector<std::string> ret{};
    for (const auto& id : m_partition_table->get_partition_ids()) {
        ret.emplace_back(partition_id_to_volume_name(m_name, id));
    }
    return ret;
}


std::string SingleNamespaceDriveHandler::create_volume(const uint64_t size_bytes) const {
    std::lock_guard<std::mutex> lock{m_mutex};

    try {
        uint64_t size_lba = integer_divide_and_ceil(size_bytes, m_lba_size);
        unsigned id = m_partition_table->get_available_partition_id();
        std::string partition_name = partition_id_to_volume_name(m_name, id);

        log_debug("drive-handler", "Creating volume on drive " << m_name);
        log_debug("drive-handler", "\tRequested size (bytes): " << size_bytes);
        log_debug("drive-handler", "\tAllocated size (bytes): " << size_lba * m_lba_size);
        log_debug("drive-handler", "\tAllocated size (LBA): " << size_lba);
        log_debug("drive-handler", "\tPartition slot: " << id);

        PartitionData pd{};
        pd.first_lba = m_partition_table->get_empty_region(size_lba);
        log_debug("drive-handler", "\tStart location (LBA): " << pd.first_lba);
        pd.size_lba = size_lba;
        pd.type = PartitionType::LINUX;
        m_partition_table->set_partition(id, pd);
        m_partition_table->update();
        log_debug("drive-handler", "\tReloading...");
        m_partition_table->save(m_name, m_drive_interface);
        m_partition_table->load(m_name, m_drive_interface);
        force_os_update_partition_table(m_name);
        log_debug("drive-handler", "Volume " << partition_name << " created successfully");
        return partition_name;
    }
    catch (const std::exception& e) {
        try {
            // Try to reload information about partitions if something failed...
            m_partition_table->load(m_name, m_drive_interface);
        }
        catch (...) { }

        log_error("drive-handler", "Exception while creating volume (on drive " << m_name << "): " << e.what());
        throw;
    }

}


void SingleNamespaceDriveHandler::remove_volume(const std::string& id) const {
    std::lock_guard<std::mutex> lock{m_mutex};

    try {
        log_debug("drive-handler", "Removing volume on drive " << m_name);
        unsigned partition_id = volume_name_to_partition_id(m_name, id);
        log_debug("drive-handler", "\tPartition slot " << partition_id);

        auto ids = m_partition_table->get_partition_ids();
        if (std::find(ids.begin(), ids.end(), partition_id) == ids.end()) {
            log_warning("drive-handler", "Trying to remove volume that does not exist (" << id << ")");
            return;
        }
        m_partition_table->remove_partition(partition_id);
        m_partition_table->update();
        log_debug("drive-handler", "\tReloading...");
        m_partition_table->save(m_name, m_drive_interface);
        m_partition_table->load(m_name, m_drive_interface);
        force_os_update_partition_table(m_name);
        log_debug("drive-handler", "Volume " << id << " removed successfully");
    }
    catch (const std::exception& e) {
        try {
            // Try to reload information about partitions if something failed...
            m_partition_table->load(m_name, m_drive_interface);
        }
        catch (...) { }

        log_error("drive-handler", "Exception while removing volume (on drive " << m_name << "): " << e.what());
        throw;
    }
}


SingleNamespaceDriveHandler::VolumeData SingleNamespaceDriveHandler::get_volume_data(const std::string& id) const {
    std::lock_guard<std::mutex> lock{m_mutex};

    VolumeData vd{};
    try {
        vd.block_size_bytes = m_lba_size;
        if (!m_partition_table) {
            throw std::runtime_error("Partition table not initialized.");
        }
        auto partition_data = m_partition_table->get_partition(volume_name_to_partition_id(m_name, id));
        vd.size_lba = partition_data.size_lba;
        vd.uuid = partition_data.uuid;
    }
    catch (const std::exception& e) {
        log_error("drive-handler",
                  "Unable to read volume (id = " << id << "), for drive " << m_name << ": " << e.what());
    }
    return vd;
}


uint32_t SingleNamespaceDriveHandler::get_primary_namespace_id() const {
    return find_first_namespace(m_nvme_interface->get_active_namespaces(m_name));
}


SingleNamespaceDriveHandler::SmartData SingleNamespaceDriveHandler::load_smart_info() const {
    std::lock_guard<std::mutex> lock{m_mutex};
    SingleNamespaceDriveHandler::SmartData ret{};

    try {
        auto smart_log = m_nvme_interface->get_smart_log(m_name, global_log_page_id);

        ret.available_spare = smart_log.available_spare;
        ret.available_spare_threshold = smart_log.available_spare_threshold;
        ret.composite_temperature = smart_log.composite_temperature;
        ret.controller_busy_time = uint128_to_long_double((smart_log.controller_busy_time));
        ret.critical_warnings = smart_log.critical_warnings;
        ret.data_unit_read = uint128_to_long_double((smart_log.data_unit_read));
        ret.data_unit_written = uint128_to_long_double((smart_log.data_unit_written));
        ret.host_read_commands = uint128_to_long_double((smart_log.host_read_commands));
        ret.host_write_commands = uint128_to_long_double((smart_log.host_write_commands));
        ret.media_data_integrity_errors = uint128_to_long_double((smart_log.media_data_integrity_errors));
        ret.number_of_error_log_entries = uint128_to_long_double((smart_log.number_of_error_log_entries));
        ret.percentage_used = smart_log.percentage_used;
        ret.power_cycles = uint128_to_long_double((smart_log.power_cycles));
        ret.power_on_hours = uint128_to_long_double((smart_log.power_on_hours));
        ret.unsafe_shutdowns = uint128_to_long_double((smart_log.unsafe_shutdowns));
    }
    catch (const NvmeException& e) {
        log_error("drive-handler", "NVMe exception while reading drive data (" << m_name << "): " << e.what());
    }
    catch (const std::exception& e) {
        log_error("drive-handler", "Exception while reading drive data (" << m_name << "): " << e.what());
    }

    return ret;
}


BaseDriveHandler::LatencyData SingleNamespaceDriveHandler::load_latency_histogram() const {
    std::lock_guard<std::mutex> lock{m_mutex};
    SingleNamespaceDriveHandler::LatencyData data{};

    try {
        auto read_latency = m_nvme_interface->get_read_latency_histogram(m_name, m_namespace_id);
        log_debug("drive-handler", "Read latency log page version: "
            << read_latency.major_version << "." << read_latency.minor_version);

        // Steps and ranges defined in Intel NVMe-oF Arch Spec
        set_latency_data(data.read_histogram.group_1st, 0, 1024, 32, read_latency.buckets_1st_group, 32);
        set_latency_data(data.read_histogram.group_2nd, 1000, 32000, 1000, read_latency.buckets_2nd_group, 31);
        set_latency_data(data.read_histogram.group_3rd, 32000, 1023000, 32000, read_latency.buckets_3rd_group, 31);
        set_latency_data(data.read_histogram.group_4th, 1024000, 2047000, 1023000, read_latency.buckets_4th_group, 1);
        set_latency_data(data.read_histogram.group_5th, 2048000, 4095000, 2047000, read_latency.buckets_5th_group, 1);
        set_latency_data(data.read_histogram.group_6th, 4096000, {}, {}, read_latency.buckets_6th_group, 1);

        auto write_latency = m_nvme_interface->get_write_latency_histogram(m_name, m_namespace_id);
        log_debug("drive-handler", "Write latency log page version: "
            << write_latency.major_version << "." << write_latency.minor_version);

        set_latency_data(data.write_histogram.group_1st, 0, 1024, 32, write_latency.buckets_1st_group, 32);
        set_latency_data(data.write_histogram.group_2nd, 1000, 32000, 1000, write_latency.buckets_2nd_group, 31);
        set_latency_data(data.write_histogram.group_3rd, 32000, 1023000, 32000, write_latency.buckets_3rd_group, 31);
        set_latency_data(data.write_histogram.group_4th, 1024000, 2047000, 1023000, write_latency.buckets_4th_group, 1);
        set_latency_data(data.write_histogram.group_5th, 2048000, 4095000, 2047000, write_latency.buckets_5th_group, 1);
        set_latency_data(data.write_histogram.group_6th, 4096000, {}, {}, write_latency.buckets_6th_group, 1);

    }
    catch (const NvmeException& e) {
        log_warning("drive-handler", "NVMe exception while reading histogram data (" << m_name << ")." <<
        " The feature may not be supported by this drive. Message: " << e.what());
    }
    catch (const std::exception& e) {
        log_warning("drive-handler", "Exception while reading histogram data (" << m_name << "): " << e.what());
    }

    return data;
}


void SingleNamespaceDriveHandler::initialize_drive() {
    std::lock_guard<std::mutex> lock{m_mutex};
    try {
        if (!m_partition_table->is_valid()) {
            log_info("drive-handler", "Initializing drive " << m_name);
            m_partition_table->reset(m_namespace_data.size);
            log_debug("drive-handler", "\tReloading...");
            m_partition_table->save(m_name, m_drive_interface);
            m_partition_table->load(m_name, m_drive_interface);
            force_os_update_partition_table(m_name);
            log_debug("drive-handler", "Drive initialized successfully");
        }
    }
    catch (const std::runtime_error& e) {
        log_error("drive-handler", "Exception while initializing drive (" << m_name << "): " << e.what());
        throw std::runtime_error("Drive initialization failed!");
    }
}


void SingleNamespaceDriveHandler::erase_volume(const std::string& id, bool fast_mode) {
    std::lock_guard<std::mutex> lock{m_mutex};
    try {
        log_info("drive-handler", "Erasing (" << (fast_mode ? "fast" : "slow") << ") volume " << id);

        utils::ByteBuffer data(::ERASE_BYTES_PER_SINGLE_WRITE);

        // fill data or leave it empty for fast mode
        if (!fast_mode) {
            std::default_random_engine generator(std::chrono::system_clock::now().time_since_epoch().count());
            std::uniform_int_distribution<uint8_t> distribution(0, 255);
            std::generate(data.begin(), data.end(), [&distribution, &generator]() { return distribution(generator); });
        }

        auto partition_data = m_partition_table->get_partition(volume_name_to_partition_id(m_name, id));

        m_drive_interface->write(m_name, partition_data.first_lba * m_lba_size,
                                 partition_data.size_lba * m_lba_size,
                                 [&data]() -> const utils::ByteBuffer& { return data; });
        log_debug("drive-handler", "Volume erased successfully");
    }
    catch (const std::runtime_error& e) {
        log_error("drive-handler", "Exception while erasing volume (" << id << "): " << e.what());
        throw std::runtime_error("Volume erase failed!");
    }
}
