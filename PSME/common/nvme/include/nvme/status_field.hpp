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
 * @file nvme/status_field.hpp
 */

#pragma once

#include <cstdint>
#include <string>

namespace nvme {

/*! Status code type */
enum class StatusCodeType : uint8_t {
    Generic = 0x00,
    CommandSpecific = 0x01,
    MediaDataIntegrity = 0x02,
    VendorSpecific = 0x07
};

/*! Status codes for generic statuses */
enum class GenericStatusCode : uint8_t {
    // admin commands
    Success = 0x00,
    InvalidOpcode = 0x01,
    InvalidField = 0x02,
    IdConflict = 0x03,
    DataTransferError = 0x04,
    PowerLoss = 0x05,
    InternalError = 0x06,
    AbortRequested = 0x07,
    SqDeleted = 0x08,
    FailedFusedOperation = 0x09,
    MissingFusedCommand = 0x0A,
    InvalidNamespaceOrFormat = 0x0B,
    SequenceError = 0x0C,
    InvalidSglDescriptor = 0x0D,
    InvalidSglDescriptorsNumber = 0x0E,
    InvalidSglDataLength = 0x0F,
    InvalidSglMetadataLength = 0x10,
    InvalidSglDescriptorType = 0x11,
    InvalidControllerMemoryBufferUse = 0x12,
    InvalidPrpOffset = 0x13,
    AtomicWriteUnitExceeded = 0x14,
    OperationDenied = 0x15,
    InvalidSglOffset = 0x16,
    HostIdInconsistentFormat = 0x18,
    ExpiredKeepAliveTimeout = 0x19,
    InvalidKeepAliveTimeout = 0x1A,
    PreemptAbort = 0x1B,
    SanitizeFailed = 0x1C,
    SanitizeInProgress = 0x1D,
    InvalidSglDataBlockGranularity = 0x1E,
    NotSupportedCommandInCmbQueue = 0x1F,
    // nvm commands
    LbaOutOfRange = 0x80,
    CapacityExceeded = 0x81,
    NamespaceNotReady = 0x82,
    ReservationConflict = 0x83,
    FormatInProgress = 0x84
};

/*! Status codes for command specific statuses */
enum class CommandSpecificStatusCode : uint8_t {
    // admin commands
    InvalidCompletionQueue = 0x00,
    InvalidQueueId = 0x01,
    InvalidQueueSize = 0x02,
    AbortCommadLimitExceeded = 0x03,
    AsynchronousEventRequestLimitExceeded = 0x05,
    InvalidFirmwareSlot = 0x06,
    InvalidFirmwareImage = 0x07,
    InvalidInterruptVector = 0x08,
    InvalidLogPage = 0x09,
    InvalidFormat = 0x0A,
    FirmwareActivationRequiresConventionalReset = 0x0B,
    InvalidQueueDeletion = 0x0C,
    FeatureIdentifierNotSaveable = 0x0D,
    FetaureNotChangeable = 0x0E,
    FetaureNotNamespaceSpecific = 0x0F,
    FirmwareActivationRequiresNvmSubsytemReset = 0x10,
    FrimwareActivationRequiresReset = 0x11,
    FirmwareActivationRequiresMaximumTimeViolation = 0x12,
    FirmwareActivationProhibited = 0x13,
    OverlappingRange = 0x14,
    NamespaceInsufficientCapacity = 0x15,
    NamespaceIdentifierUnavailable = 0x16,
    NamespaceAlreadyAttached = 0x18,
    NamespaceIsPrivate = 0x19,
    NamespaceNotAttached = 0x1A,
    ThinProvisioningNotSupported = 0x1B,
    InvalidControllerList = 0x1C,
    DeviceSelfTestInProgress = 0x1D,
    BootPartitionWriteProhibited = 0x1E,
    InvalidControllerId = 0x1F,
    InvalidSecondaryControllerState = 0x20,
    InvalidNumberOfControllerResources = 0x21,
    InvalidResourceIndentifier = 0x22,
    // Nvm commands
    ConflictingAttributes = 0x80,
    InvalidProtectionInformation = 0x81,
    AttemptedWriteToReadOnlyRange = 0x82
};

/*! Status codes for media or data integrity statuses */
enum class MediaDataIntegrityStatusCode : uint8_t {
    // Nvm commands
    WriteFault = 0x80,
    UnrecoveredReadError = 0x81,
    EndToEndGuardCheckError = 0x82,
    EndToEndApplicationTagCheckError = 0x83,
    EndToEndReferenceTagCheckError = 0x84,
    CompareFailure = 0x85,
    AccessDenied = 0x86,
    DeallocatedOrUnwrittenLogicalBlock = 0x87
};

/*! Wrapper class for status fields that includes utility methods */
class StatusField final {
public:

    StatusField() {}
    StatusField(uint16_t status_field): m_status_field(status_field) {}
    ~StatusField() {}

    StatusField(const StatusField&) = default;
    StatusField(StatusField&&) = default;

    StatusField& operator=(const StatusField&) = default;
    StatusField& operator=(StatusField&&) = default;

    /*!
     * @brief Extracts status code from the status field value
     * @return Status code
     */
    uint8_t get_code() const {
        // 8 status code bits are located in bits 8:1
        return uint8_t((m_status_field >> 1) & 0x00FF);
    }


    /*!
     * @brief Extracts status code type from the status field value
     * @return Status code type
     */
    uint8_t get_type() const {
        // 3 status code type bits are located in bits 11:9
        return uint8_t((m_status_field >> 9) & 0x0007);
    }

    /*!
     * @brief Checks if status code is a success return code
     * @return True for success status codes
     */
    bool is_successful() const {
        if (get_type() == static_cast<uint8_t>(StatusCodeType::Generic)
            && get_code() == static_cast<uint8_t>(GenericStatusCode::Success)) {

            return true;
        }
        return false;
    }

    /*!
     * @brief Returns stored raw status field value
     * @return Status field value
     */
    uint16_t get_raw_value() const {
        return m_status_field;
    }

private:

    uint16_t m_status_field{};

};

}
