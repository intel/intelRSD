/*!
 * @copyright Copyright (c) 2018-2019 Intel Corporation.
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
 * @file nvmf-discovery/fabric_exception.hpp
 */

#include "nvme/definitions.hpp"
#include <stdexcept>

#pragma once

namespace nvmf_discovery {

/*!
 * Base class for fabric exceptions.
 */
class FabricException : public std::runtime_error {
public:
    /*!
     * Constructor.
     * @param[in] msg Error message
     * @param[in] code Error code
     */
    explicit FabricException(const std::string& msg, ssize_t code);

    /*! Destructor */
    ~FabricException() override;

    FabricException(const FabricException&) = default;
    FabricException(FabricException&&) = default;
    FabricException& operator=(const FabricException&) = default;
    FabricException& operator=(FabricException&&) = default;

    /*
     * Return error code (usually errno) related to exception.
     * @return Error code of given exception.
     */
    ssize_t get_code() const {
        return m_code;
    }

private:
    ssize_t m_code{0};
};


/*!
 * Nvmf exception containing nvme status.
 */
class NvmfException: public FabricException {
public:
    /*! Constructor */
    NvmfException(const std::string& msg, ssize_t code = 0, nvme::NvmeStatus status = {});

    NvmfException(const std::string& msg, nvme::NvmeStatus status);

    /*! Destructor */
    ~NvmfException() override;

    NvmfException(const NvmfException&) = default;
    NvmfException(NvmfException&&) = default;
    NvmfException& operator=(const NvmfException&) = default;
    NvmfException& operator=(NvmfException&&) = default;

    nvme::NvmeStatus get_status() const {
        return m_status;
    }

private:
    nvme::NvmeStatus m_status{};
};

}
