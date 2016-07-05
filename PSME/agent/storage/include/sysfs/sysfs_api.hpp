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
 *
 *
 * @file sysfs_api.hpp
 * @brief C++ wrapper for libsysfs
 * */

#ifndef PSME_STORAGE_SYSFS_API_HPP
#define PSME_STORAGE_SYSFS_API_HPP

#include <vector>
#include <string>
#include <memory>

struct sysfs_device;

namespace agent {
namespace storage {
namespace sysfs {

static constexpr uint32_t SSD_RPM_VALUE = 1;

using std::vector;
using std::string;
using std::uint32_t;

/*!
 * @brief C++ wrapper for libsysfs
 */
class SysfsAPI {
public:

    /*! Block device */
    class HardDrive {
        string m_name{};
        string m_sysfs_path{};
        string m_device_path{};
        string m_manufacturer{};
        string m_model{};
        string m_serial_number{};
        double m_capacity_gb{};
        uint32_t m_rpm{};
        string m_type{};
        string m_interface{};
    public:

        /*! Default constructor */
        HardDrive() {}

        bool operator==(const agent::storage::sysfs::SysfsAPI::HardDrive& a) {
            return ((a.get_serial_number() == get_serial_number()) &&
                        (a.get_model() == get_model()) &&
                        (a.get_device_path() == get_device_path()));
        }

        /*! Default copy constructor */
        HardDrive(const HardDrive&) = default;

        /*! Default assignment operator */
        HardDrive& operator=(const HardDrive&) = default;

        /*!
         * @brief Get hard drive name
         * @return Hard drive name
         * */
        const string& get_name() const {
            return m_name;
        }

        /*!
         * @brief Set hard drive name
         * @param[in] name Hard drive name
         * */
        void set_name(const string& name) {
            m_name = name;
        }

        /*!
         * @brief Get hard drive SysFS path
         * @return Hard drive SysFS path
         * */
        const string& get_sysfs_path() const {
            return m_sysfs_path;
        }

        /*!
         * @brief Set hard drive SysFS path
         * @param[in] sysfs_path Hard drive SysFS path
         * */
        void set_sysfs_path(const string& sysfs_path) {
            m_sysfs_path = sysfs_path;
        }

        /*!
         * @brief Get device path (/dev/...)
         * @return Device path
         * */
        const string& get_device_path() const {
            return m_device_path;
        }

        /*!
         * @brief Set hard drive device path (/dev/...)
         * @param[in] device_path Hard drive device path
         * */
        void set_device_path(const string& device_path) {
            m_device_path = device_path;
        }

        /*!
         * @brief Get hard drive capacity in GB
         * @return Hard drive capacity in GB
         * */
        double get_capacity_gb() const {
            return m_capacity_gb;
        }

        /*!
         * @brief Set hard drive capacity in GB
         * @param[in] capacity_gb Hard drive capacity in GB
         * */
        void set_capacity_gb(double capacity_gb) {
            m_capacity_gb = capacity_gb;
        }

        /*!
         * @brief Get hard drive RPM
         * @return Hard drive RPM
         * */
        uint32_t get_rpm() const {
            return m_rpm;
        }

        /*!
         * @brief Set hard drive RPM
         * @param[in] rpm Hard drive RPM
         * */
        void set_rpm(uint32_t rpm) {
            m_rpm = rpm;
        }

        /*!
         * @brief Get hard drive manufacturer
         * @return Hard drive manufacturer
         * */
        const string& get_manufacturer() const {
            return m_manufacturer;
        }

        /*!
         * @brief Set hard drive manufacturer
         * @param[in] manufacturer Hard drive manufacturer
         * */
        void set_manufacturer(const string& manufacturer) {
            m_manufacturer = manufacturer;
        }

        /*!
         * @brief Get hard drive model
         * @return Hard drive model
         * */
        const string& get_model() const {
            return m_model;
        }

        /*!
         * @brief Set hard drive model
         * @param[in] model Hard drive model
         * */
        void set_model(const string& model) {
            m_model = model;
        }

        /*!
         * @brief Get drive serial number
         * @return Hard drive serial number
         * */
        const string& get_serial_number() const {
            return m_serial_number;
        }

        /*!
         * @brief Set hard drive serial number
         * @param[in] serial_number Hard drive serial number
         * */
        void set_serial_number(const string& serial_number) {
            m_serial_number = serial_number;
        }

        /*!
         * @brief Get hard drive type
         * @return Hard drive type
         * */
        const string& get_type() const {
            return m_type;
        }

        /*!
         * @brief Set hard drive type
         * @param[in] type Hard drive type
         * */
        void set_type(const string& type) {
            m_type = type;
        }

        /*!
         * @brief Get hard drive interface
         * @return Hard drive interface
         * */
        const string& get_interface() const {
            return m_interface;
        }

        /*!
         * @brief Set hard drive interface
         * @param[in] interface Hard drive interface
         * */
        void set_interface(const string& interface) {
            m_interface = interface;
        }

        static constexpr const char TYPE_SSD[] = "SSD";
        static constexpr const char TYPE_HDD[] = "HDD";
    };

    class Partition {
        std::string m_name{};
        std::string m_path{};
        double m_capacity_gb{};
    public:
        Partition(){}

        /*!
         * @brief Get partition name
         * @return partition name
         * */
        const string& get_name() const {
            return m_name;
        }

        /*!
         * @brief Set partition name
         * @param[in] name Partition name
         * */
        void set_name(const string& name) {
            m_name = name;
        }

        /*!
         * @brief Get partition path
         * @return Hard partition path
         * */
        const string& get_path() const {
            return m_path;
        }

        /*!
         * @brief Set partition path
         * @param[in] path Partition path
         * */
        void set_path(const string& path) {
            m_path = path;
        }

        /*!
         * @brief Get Partition capacity in GB
         * @return Partition capacity in GB
         * */
        double get_capacity_gb() const {
            return m_capacity_gb;
        }

        /*!
         * @brief Set partition capacity in GB
         * @param[in] capacity_gb Partition capacity in GB
         * */
        void set_capacity_gb(const double capacity_gb) {
            m_capacity_gb = capacity_gb;
        }


    };

    /*!
     * @brief Singleton pattern. Get SysfsAPI instance
     *
     * @return  SysfsAPI instance. Create if not exist otherwise
     *          return existing instance.
     * */
    static SysfsAPI* get_instance();

    /*!
     * @brief Get hard drives
     * @param[in] drives Vector of hard drives to be filled.
     * */
    void get_hard_drives(vector<HardDrive>& drives);

    /*!
     * @brief Gets list of partitions for given hard drive.
     * @param[in] drive Hard Drive.
     * @param[out] partitions List of partitions.
     * */
    void get_partitions(const HardDrive& drive, vector<Partition>& partitions);

    /*!
     * @brief Default destructor.
     * */
    virtual ~SysfsAPI();

private:
    SysfsAPI() {}

    string m_boot_device{};

    bool m_is_virtual_device(const struct sysfs_device* device);
    bool m_is_boot_device(const struct sysfs_device* device);

    void m_detect_boot_device();

    void m_read_block_device_attributes(struct sysfs_device* device,
                                        HardDrive& hard_drive);

    void m_read_attribute(unsigned long long& value,
                          struct sysfs_device* device,
                          const string& attribute_name);

    void m_read_attribute(string& value,
                          struct sysfs_device* device,
                          const string& attribute_name);

    void m_read_attribute(int& value,
                          struct sysfs_device* device,
                          const string& attribute_name);

    void m_read_ata_attributes(const struct sysfs_device* device,
                               HardDrive& hard_drive);
    void m_perform_sg_io(const struct sysfs_device* device, std::unique_ptr<uint16_t[]>& p_hdio_data);
    void m_perform_hdio_drive_cmd(const struct sysfs_device* device, std::unique_ptr<uint16_t[]>& p_hdio_data);
    void m_read_ata_string(string& value, size_t offset, size_t length, std::unique_ptr<uint16_t[]>& p_hdio_data);
    void m_read_rpm(uint32_t& rpm, std::unique_ptr<uint16_t[]>& p_hdio_data);
    void m_read_serial_number(string& serial_number, std::unique_ptr<uint16_t[]>& p_hdio_data);
    void m_read_manufacturer(string& manufacturer, std::unique_ptr<uint16_t[]>& p_hdio_data);

    string m_get_device_path(const struct sysfs_device* device);

    void m_trim(string& input_string);

    static constexpr double SECTORS_TO_GB = 512.0 / (1000.0 * 1000.0 * 1000.0); // a sector has 512 Bytes
    static constexpr const char ROOT_FS[] = "/";
    static constexpr const char BOOT_FS[] = "/boot";

    static constexpr uint32_t ATA_DATA_SIZE = 512+4;
};

}
}
}
#endif	/* PSME_STORAGE_SYSFS_API_HPP */

