/*!
 * @brief Unit tests for generation of UUIDv5
 *
 * @copyright
 * Copyright (c) 2017-2019 Intel Corporation
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
 * @file compute_tree_stabilizer_test.cpp
 * */


#include "certificate_management/certificate_manager.hpp"
#include "logger/logger.hpp"

#include "gtest/gtest.h"

#include <sys/stat.h>
#include <fcntl.h>

using namespace agent::rmm;

TEST(CertificateManagerTest, NoDoubleCertificateFilepathSettingTest) {
    CertificateManager certificate_manager{};

    const CertificateManager::CertificateType podm_cert_type{CertificateManager::CertificateType::PODM};
    const std::string podm_certificate_file_path{"/etc/psme/certs/podm.crt"};

    ASSERT_NO_THROW(certificate_manager.set_cert_file_path(podm_cert_type, podm_certificate_file_path));
    ASSERT_THROW(certificate_manager.set_cert_file_path(podm_cert_type, podm_certificate_file_path), std::logic_error);
}

namespace {

class CertificateManagerTester : public CertificateManager {
public:
    CertificateManagerTester(const std::string& file) {
        CertificateDataReference reference = get_certificate(CertificateManager::CertificateType::PODM);
        CertificateData& certificate_data = *reference;

        certificate_data.set_certificate_file_path(file);
        ::unlink(file.c_str());
    }

    virtual ~CertificateManagerTester() { }

    void create_cert_file(size_t size, mode_t mode) {
        CertificateDataReference reference = get_certificate(CertificateManager::CertificateType::PODM);
        CertificateData& certificate_data = *reference;

        ::unlink(certificate_data.get_certificate_file_path().c_str());
        int fd = ::open(certificate_data.get_certificate_file_path().c_str(), O_WRONLY | O_CREAT, mode);
        if (fd < 0) {
            throw std::runtime_error("Cannot create file");
        }

        char c = 'a';
        for (size_t b = 0; b + 1 < size; b++) {
            if (b % 21 == 20) {
                putc(fd, '\n');
            }
            else {
                putc(fd, c);
                c++;
                if (c > 'z') {
                    c = 'a';
                }
            }
        }
        if (size != 0) {
            putc(fd, '\n');
        }
        ::close(fd);
        /* files in the test are created faster than resolution of the clock.. */
        usleep(10000);
    }

    bool check_cert_file_changed() {
        CertificateDataReference reference = get_certificate(CertificateManager::CertificateType::PODM);
        CertificateData& certificate_data = *reference;

        return read_cert_file_and_update(certificate_data);
    }

    size_t get_cert_size() const {
        CertificateDataReference reference = get_certificate(CertificateManager::CertificateType::PODM);
        CertificateData& certificate_data = *reference;

        return certificate_data.get_certificate_data().size();
    }

private:
    void putc(int fd, char c) {
        if (write(fd, &c, 1) != 1) {
            ::close(fd);
            throw std::runtime_error("Cannot write data");
        }
    }
};

}

TEST(CertificateManagerTest, ReadingWrongCertificateFile) {
    CertificateManagerTester certificate_manager{"./cert"};

    ASSERT_THROW(certificate_manager.check_cert_file_changed(), std::logic_error) << "not fully qualified path";
}

TEST(CertificateManagerTest, ReadingCertificateFile) {
    CertificateManagerTester certificate_manager{"/tmp/cert_reading"};

    ASSERT_THROW(certificate_manager.check_cert_file_changed(), std::logic_error) << "file exist";

    certificate_manager.create_cert_file(0, S_IWUSR | S_IRUSR);
    ASSERT_THROW(certificate_manager.check_cert_file_changed(), std::logic_error) << "empty file read";

    certificate_manager.create_cert_file(40, S_IWUSR | S_IRUSR);
    ASSERT_TRUE(certificate_manager.check_cert_file_changed()) << "cannot read file";
    ASSERT_EQ(40, certificate_manager.get_cert_size());
    ASSERT_FALSE(certificate_manager.check_cert_file_changed()) << "file timestamp modified";

    certificate_manager.create_cert_file(50, S_IWUSR | S_IRUSR);
    ASSERT_TRUE(certificate_manager.check_cert_file_changed()) << "file not modified";
    ASSERT_EQ(50, certificate_manager.get_cert_size());

    certificate_manager.create_cert_file(60, 0);
    ASSERT_THROW(certificate_manager.check_cert_file_changed(), std::logic_error) << "inaccessible file read";
}
