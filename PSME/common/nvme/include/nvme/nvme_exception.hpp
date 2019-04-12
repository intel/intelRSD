/*!
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
 * @file nvme/nvme_exception.hpp
 */

#pragma once

#include "nvme/status_field.hpp"

namespace nvme {

/*!
 * Exception class representing NVMe error codes
 */
class NvmeException : public std::exception {
public:

    /*!
     * @brief Constructs exception based on the status field returned from ioctl
     * @param status_field Status field value
     */
    explicit NvmeException(uint16_t status_field);

    /*!
     * @brief Constructs exception based on the status field returned from ioctl
     * @param status_field Status field value
     */
    explicit NvmeException(StatusField status_field);

    /*!
     * @brief Constructs exception for 'non-NVMe' errors (i.e. during NVMe calls)
     * @param msg What message
     */
    explicit NvmeException(const std::string& msg);

    /*! Enable copy */
    NvmeException(const NvmeException&) = default;
    NvmeException& operator=(const NvmeException&) = default;

    /*! Enable move */
    NvmeException(NvmeException&&) = default;
    NvmeException& operator=(NvmeException&&) = default;

    virtual ~NvmeException();

    /*!
     * @brief Gets the exception 'what' message
     * @return What message
     */
    virtual const char* what() const noexcept override {
        return m_what.c_str();
    }

    /*
     * @brief Checks if the exception is an error code returned by the NVMe command
     * @param True for NVMe return codes, false for other errors
     * @return Status code type
     */
    bool is_nvme() const {
        return m_is_nvme;
    }

    /*
     * @brief Returns the type of the status code (should NOT be used if is_nvme() returns false)
     * @return Status code type
     */
    StatusCodeType get_nvme_status_type() const;

    /*
     * @brief Returns status code interpreted as a generic status code
     * @return Status code
     */
    GenericStatusCode get_code_as_generic() const;

    /*
     * @brief Returns status code interpreted as a command specific status code
     * @return Status code
     */
    CommandSpecificStatusCode get_code_as_command_specific() const;

    /*
     * @brief Returns status code interpreted as a media data integrity status code
     * @return Status code
     */
    MediaDataIntegrityStatusCode get_code_as_media_data_integrity() const;

private:

    /* Prepares m_what field returned on what() method call for nvme exceptions */
    void make_nvme_what();

    StatusField m_status_field{};
    std::string m_what{};

    bool m_is_nvme{false};
};

}
