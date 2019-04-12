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
 * @file nvme/nvme_exception.cpp
 */

#include "nvme/nvme_exception.hpp"

#include <map>
#include <sstream>

using namespace nvme;

namespace {

static const std::map<GenericStatusCode, std::string> generic_status_codes {
    {GenericStatusCode::Success, "Command completed successfully"},
    {GenericStatusCode::InvalidOpcode, "Invalid command opcode"},
    {GenericStatusCode::InvalidField, "Invalid field value"},
    {GenericStatusCode::IdConflict, "Command Id already in use"},
    {GenericStatusCode::DataTransferError, "Data/metadata transfer error"},
    {GenericStatusCode::PowerLoss, "Aborted due to power loss"},
    {GenericStatusCode::InternalError, "Internal error"},
    {GenericStatusCode::AbortRequested, "Aborted due to Abort command request"},
    {GenericStatusCode::SqDeleted, "Aborted due to submission queue deletion"},
    {GenericStatusCode::FailedFusedOperation, "Aborted due to failed fused operation"},
    {GenericStatusCode::MissingFusedCommand, "Aborted due to missing fused operation"},
    {GenericStatusCode::InvalidNamespaceOrFormat, "Invalid namespace or namespace format"},
    {GenericStatusCode::SequenceError, "Aborted due to protocol violation in multi command sequence"},
    {GenericStatusCode::InvalidSglDescriptor, "Invalid SGL last segment or SGL segment descriptor"},
    {GenericStatusCode::InvalidSglDescriptorsNumber, "Invalid number of SGL segments"},
    {GenericStatusCode::InvalidSglDataLength, "Data SGL is too short"},
    {GenericStatusCode::InvalidSglMetadataLength, "Metadata SGL is too short"},
    {GenericStatusCode::InvalidSglDescriptorType, "SGL descriptor type not supported by the controller"},
    {GenericStatusCode::InvalidControllerMemoryBufferUse, "Not supported use of Controller Memory Buffer"},
    {GenericStatusCode::InvalidPrpOffset, "Invalid offset in PRP entry"},
    {GenericStatusCode::AtomicWriteUnitExceeded, "Length exceeds atomic write unit size"},
    {GenericStatusCode::OperationDenied, "Operation denied"},
    {GenericStatusCode::InvalidSglOffset, "Invalid offset in SGL descriptor"},
    {GenericStatusCode::HostIdInconsistentFormat, "Simultaneous use of 64 and 128 bit host id on different controllers"},
    {GenericStatusCode::ExpiredKeepAliveTimeout, "Keep alive timeout has expired"},
    {GenericStatusCode::InvalidKeepAliveTimeout, "Invalid Keep alive timeout"},
    {GenericStatusCode::PreemptAbort, "Aborted due to Preempt and Abort Reservation Acquire command"},
    {GenericStatusCode::SanitizeFailed, "Sanitize failed"},
    {GenericStatusCode::SanitizeInProgress, "Sanitize in progress"},
    {GenericStatusCode::InvalidSglDataBlockGranularity, "Invalid address alignment or length granularity for SGL data block descriptor"},
    {GenericStatusCode::NotSupportedCommandInCmbQueue, "Submission to Controller Memory Buffer is not supported"},
    {GenericStatusCode::LbaOutOfRange, "Referenced LBA exceeds the size of the namespace"},
    {GenericStatusCode::CapacityExceeded, "Namespace utilization exceeds namespace capacity"},
    {GenericStatusCode::NamespaceNotReady, "Namespace is not ready to be addressed"},
    {GenericStatusCode::ReservationConflict, "Aborted due to conflict with a reservation held on the namespace"},
    {GenericStatusCode::FormatInProgress, "NVM format in progress"}
};

static const std::map<CommandSpecificStatusCode, std::string> command_specific_status_codes {
    {CommandSpecificStatusCode::InvalidCompletionQueue, "Invalid completion queue id"},
    {CommandSpecificStatusCode::InvalidQueueId, "Invalid queue identifier value"},
    {CommandSpecificStatusCode::InvalidQueueSize, "Invalid number of entries in the requested queue"},
    {CommandSpecificStatusCode::AbortCommadLimitExceeded, "Number of Abort requests exceeded"},
    {CommandSpecificStatusCode::AsynchronousEventRequestLimitExceeded, "Number of Asynchronous Event Requests exceeded"},
    {CommandSpecificStatusCode::InvalidFirmwareSlot, "Specified firmware slot is invalid or read only"},
    {CommandSpecificStatusCode::InvalidFirmwareImage, "Specified firmware slot is invalid and not loaded"},
    {CommandSpecificStatusCode::InvalidInterruptVector, "Invalid interrupt vector value"},
    {CommandSpecificStatusCode::InvalidLogPage, "Invalid log page id"},
    {CommandSpecificStatusCode::InvalidFormat, "Provided LBA format is not supported"},
    {CommandSpecificStatusCode::FirmwareActivationRequiresConventionalReset, "Firmware commit is successful, but activation requires a conventional reset"},
    {CommandSpecificStatusCode::InvalidQueueDeletion, "Specified completion queue cannot be deleted"},
    {CommandSpecificStatusCode::FeatureIdentifierNotSaveable, "Feature identifier does not support a saveable value"},
    {CommandSpecificStatusCode::FetaureNotChangeable, "Feature identifier is not able to be changed"},
    {CommandSpecificStatusCode::FetaureNotNamespaceSpecific, "Feature identifier is not namespace specific"},
    {CommandSpecificStatusCode::FirmwareActivationRequiresNvmSubsytemReset, "Firmware commit is successful, but activation requires a NVM subsystem reset"},
    {CommandSpecificStatusCode::FrimwareActivationRequiresReset, "Firmware commit is successful, but activation requires a reset"},
    {CommandSpecificStatusCode::FirmwareActivationRequiresMaximumTimeViolation, "Image activation would exceed maximum time for activation, to activate it Firmware commit must be reissued"},
    {CommandSpecificStatusCode::FirmwareActivationProhibited, "Firmware image is prohibited from activation"},
    {CommandSpecificStatusCode::OverlappingRange, "Firmware image has overlapping ranges"},
    {CommandSpecificStatusCode::NamespaceInsufficientCapacity, "Insufficient capacity to create a namespace"},
    {CommandSpecificStatusCode::NamespaceIdentifierUnavailable, "Number of supported namespaces has been exceeded"},
    {CommandSpecificStatusCode::NamespaceAlreadyAttached, "Namespace is already attached to the controller"},
    {CommandSpecificStatusCode::NamespaceIsPrivate, "Namespace is private and already attached to a controller"},
    {CommandSpecificStatusCode::NamespaceNotAttached, "Namespace is not attached to the controller"},
    {CommandSpecificStatusCode::ThinProvisioningNotSupported, "Provisioning is not supported"},
    {CommandSpecificStatusCode::InvalidControllerList, "Provided controller list is invalid"},
    {CommandSpecificStatusCode::DeviceSelfTestInProgress, "Self test is already in progress"},
    {CommandSpecificStatusCode::BootPartitionWriteProhibited, "Attempted to modify a locked boot partition"},
    {CommandSpecificStatusCode::InvalidControllerId, "Specified controller id is invalid"},
    {CommandSpecificStatusCode::InvalidSecondaryControllerState, "Action is invalid for the controller in the current state"},
    {CommandSpecificStatusCode::InvalidNumberOfControllerResources, "Specified number of flexible resources is invalid"},
    {CommandSpecificStatusCode::InvalidResourceIndentifier, "At least one of the specified resource ids was invalid"},
    {CommandSpecificStatusCode::ConflictingAttributes, "Attributes specified in the command are conflicting"},
    {CommandSpecificStatusCode::InvalidProtectionInformation, "Specified protection information settings are invalid"},
    {CommandSpecificStatusCode::AttemptedWriteToReadOnlyRange, "Specified LBA range is read only"},
};

static const std::map<MediaDataIntegrityStatusCode, std::string> media_data_integrity_status_codes {
    {MediaDataIntegrityStatusCode::WriteFault, "Write error"},
    {MediaDataIntegrityStatusCode::UnrecoveredReadError, "Read error"},
    {MediaDataIntegrityStatusCode::EndToEndGuardCheckError, "Aborted due to end-to-end guard check failure"},
    {MediaDataIntegrityStatusCode::EndToEndApplicationTagCheckError, "Aborted due to end-to-end application tag check failure"},
    {MediaDataIntegrityStatusCode::EndToEndReferenceTagCheckError, "Aborted due to end-to-end reference tag check failure"},
    {MediaDataIntegrityStatusCode::CompareFailure, "Miscompare during the Compare command"},
    {MediaDataIntegrityStatusCode::AccessDenied, "Access denied due to lack of access rights"},
    {MediaDataIntegrityStatusCode::DeallocatedOrUnwrittenLogicalBlock, "Failed due to read attempt from deallocated or unwritten blocks"}
};

template <typename T>
std::string error_code_to_string(unsigned status_code, const std::map<T, std::string>& code_to_string_map) {
    T typed_status_code = static_cast<T>(status_code);
    auto it = code_to_string_map.find(typed_status_code);
    if (it != code_to_string_map.end()) {
        return it->second;
    }
    else {
        std::stringstream str{};
        str << std::string{"Unknown error (0x"} << status_code << ")";
        return str.str();
    }
}

std::string get_error_message(const StatusField& status_field) {
    std::stringstream str{};
    str << std::hex;

    auto status_code_type = static_cast<StatusCodeType>(status_field.get_type());
    unsigned status_code = status_field.get_code();
    switch (status_code_type) {
        case StatusCodeType::Generic: {
            str << "Generic error (0x" << status_code << "): ";
            str << error_code_to_string(status_code, generic_status_codes);
            break;
        }
        case StatusCodeType::CommandSpecific: {
            str << "Command specific error (0x" << status_code << "): ";
            str << error_code_to_string(status_code, command_specific_status_codes);
            break;
        }
        case StatusCodeType::MediaDataIntegrity: {
            str << "Media data integrity error (0x"  << status_code << "): ";
            str << error_code_to_string(status_code, media_data_integrity_status_codes);
            break;
        }
        case StatusCodeType::VendorSpecific:
            str << "Vendor specific error (0x" << status_code << "): 0x" << status_field.get_raw_value();
            break;
        default:
            str << "Unknown status code type (status field = 0x" << status_field.get_raw_value() << ")";
            break;
    }
    return str.str();
}

}

NvmeException::NvmeException(uint16_t status_field): m_status_field(status_field), m_is_nvme{true} { make_nvme_what(); }

NvmeException::NvmeException(StatusField status_field): m_status_field(status_field), m_is_nvme{true} { make_nvme_what(); }

NvmeException::NvmeException(const std::string& msg): m_is_nvme{false} { m_what = msg; }


NvmeException::~NvmeException() {}

StatusCodeType NvmeException::get_nvme_status_type() const {
    return static_cast<StatusCodeType>(m_status_field.get_type());
}

GenericStatusCode NvmeException::get_code_as_generic() const {
    return static_cast<GenericStatusCode>(m_status_field.get_code());
}

CommandSpecificStatusCode NvmeException::get_code_as_command_specific() const {
    return static_cast<CommandSpecificStatusCode>(m_status_field.get_code());
}

MediaDataIntegrityStatusCode NvmeException::get_code_as_media_data_integrity() const {
    return static_cast<MediaDataIntegrityStatusCode>(m_status_field.get_code());
}

void NvmeException::make_nvme_what() {
    m_what = get_error_message(m_status_field);
}
