/*!
 * @brief iSCSI MDR attempt structure builder.
 *
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
 * @file iscsi_mdr_attempt_builder.hpp
 */

#pragma once

#include "ipmi/ipmi_interface.hpp"
#include "iscsi/iscsi_mdr_builder_base.hpp"

#include <cstddef>

namespace iscsi {
namespace builder {

/*!
 * @brief The iSCSI MDR attempt structure builder.
 */
class IscsiAttemptBuilder final : public IscsiMdrBuilderBase {
public:

    /*!
     * @brief Deleted constructor.
     */
    IscsiAttemptBuilder() = delete;

    /*!
     * @brief Append an attempt structure to the IPMI ByteBuffer.
     *
     * @param buffer the buffer to append the structure to.
     * @param initiator the initiator handle.
     * @param target the target handle.
     * @param timeout the attempt timeout in milliseconds.
     * @param retries the attempt retry count.
     * @param handle the handle distribution function.
     */
    static void append_structure(ipmi::IpmiInterface::ByteBuffer& buffer, std::uint16_t initiator, std::uint16_t target,
        std::uint16_t timeout, std::uint8_t retries, std::function<uint16_t()> handle);
};

}  // namespace builder
}  // namespace iscsi
