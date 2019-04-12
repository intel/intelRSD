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
 * @file nvme/definitions.hpp
 */

#pragma once



#include <cstdint>



namespace nvme {

/*! Admin command opcodes */
enum class AdminCommandOpcode : uint8_t {
    DeleteSubmissionQueue = 0x00,
    CreateSubmissionQueue = 0x01,
    GetLogPage = 0x02,
    DeleteCompletionQueue = 0x04,
    CreateCompletionQueue = 0x05,
    Identify = 0x06,
    Abort = 0x08,
    SetFeatures = 0x09,
    GetFeatures = 0x0A,
    AsynchronousEventRequest = 0x0C,
    NamespaceManagement = 0x0D,
    FirmwareCommit = 0x10,
    FirmwareImageDownload = 0x11,
    NamespaceAttachment = 0x15,
    KeepAlive = 0x18,
    DirectiveSend = 0x19,
    DirectiveReceive = 0x1A,
    VirtualizationManagement = 0x1C,
    NvmeMiSend = 0x1D,
    NvmeMiReceive = 0x1E,
    DoorbellBufferConfig = 0x7C,
    FormatNvm = 0x80,
    SecuritySend = 0x81,
    SecurityReceive = 0x82,
    Sanitize = 0x84
};

/*! Nvm command opcodes */
enum class NvmCommandOpcode : uint8_t {
    Flush = 0x00,
    Write = 0x01,
    Read = 0x02,
    WriteUncorrectable = 0x04,
    Compare = 0x05,
    WriteZeroes = 0x08,
    DatasetManagement = 0x09,
    ReservationRegister = 0x0D,
    ReservationReport = 0x0E,
    ReservationAcquire = 0x11,
    ReservationRelease = 0x15
};

/*! Cns field values for Identify commands */
enum class IdentifyCns : uint8_t {
    Namespace = 0x00,                     // active namespace info
    Controller = 0x01,                    // controller info
    NamespaceIdList = 0x02,               // list of active namespaces
    NamespaceDescriptorList = 0x03,
    AllocatedNamespaceIdList = 0x10,      // list of allocated namespaces
    AllocatedNamespace = 0x11,            // allocated namespace info
    NamespaceControllerIdList = 0x12,     // list of controllers attached to the namespace
    ControllerIdList = 0x13,              // list of all controllers
    PrimaryControllerCapability = 0x14,
    SecondaryControllerList = 0x15
};

/*! Sel field values for NamespaceManagement commands */
enum class NamespaceManagementSel : uint8_t {
    Create = 0x00,
    Delete = 0x01
};

/*! Sel field values for NamespaceAttachment commands */
enum class NamespaceAttachmentSel : uint8_t {
    Attach = 0x00,
    Detach = 0x01
};

enum class GetFeaturesSel : std::uint8_t {
    Current = 0x00,
    Default = 0x01,
    Saved = 0x02,
    SupportedCapabilities = 0x03
};

/*! Log page id for GetLogPage commands */
enum class LogPageId : uint8_t {
    Error = 0x01,
    Smart = 0x02,
    Firmware = 0x03,
    ChangedNamespaceList = 0x04,
    CommandsAndEffects = 0x05,
    SelfTest = 0x06,
    TelemetryHostInit = 0x07,
    TelemetryControllerInit = 0x08,
    Discover = 0x70,
    ReservationNotification = 0x80,
    SanitizeStatus = 0x81,

    // OEM
    IntelVendorLatencyHistogramRead = 0xC0,
    IntelVendorLatencyHistogramWrite = 0xC1,
    IntelVendorIOQsMetrics = 0xCB
};

/*! Ses field values for FormatNvm commands*/
enum class FormatNvmSes : uint8_t {
    NoSecureErase = 0x00,
    UserDataErase = 0x01,
    CryptographicErase = 0x02
};

/*! NVMe controller register space offsets */
enum class NvmeCtrlRegisterOffset : uint32_t {
    Capabilities = 0x0000,
    Version = 0x0008,
    InterruptMaskSet = 0x000C,
    InterruptMaskClear = 0x0010,
    ControllerConfiguration = 0x0014,
    ControllerStatus = 0x001C,
    NvmSubsystemReset = 0x0020,
    AdminQueueAttributes = 0x0024,
    AdminSQBaseAddress = 0x0028,
    AdminCQBaseAddress = 0x0030,
    ControllerMemoryBufferLocation = 0x0038,
    ControllerMemoryBufferSize = 0x003C,
    SQ0TailDoorbell = 0x1000
};

/*! Value used in some commands to represent all namespaces */
static constexpr uint32_t ALL_NAMESPACES = 0xFFFFFFFF;

#pragma pack(push, 1)

/*! Struct representing a single entry in the error log */
struct LogPageError final {
    uint64_t error_id{};
    uint16_t submission_queue_id{};
    uint16_t command_id{};
    uint16_t status_field{};
    uint16_t parameter_error_location{};
    uint64_t lba{};
    uint32_t namespace_id{};
    uint8_t vendor_specific_log_id{};
    uint8_t reserved_29[3]{};
    uint64_t command_specific_info{};
    uint8_t reserved_40[24]{};
};
static_assert(sizeof(LogPageError) == 64, "Incorrect size");

/*! Struct representing smart log page */
struct LogPageSmart final {
    uint8_t critical_warnings{};
    uint16_t composite_temperature{};
    uint8_t available_spare{};
    uint8_t available_spare_threshold{};
    uint8_t percentage_used{};
    uint8_t reserved_6[26]{};
    uint64_t data_unit_read[2]{};
    uint64_t data_unit_written[2]{};
    uint64_t host_read_commands[2]{};
    uint64_t host_write_commands[2]{};
    uint64_t controller_busy_time[2]{};
    uint64_t power_cycles[2]{};
    uint64_t power_on_hours[2]{};
    uint64_t unsafe_shutdowns[2]{};
    uint64_t media_data_integrity_errors[2]{};
    uint64_t number_of_error_log_entries[2]{};
    uint32_t warning_composite_temperature_time{};
    uint32_t critical_composite_temperature_time{};
    uint16_t temperature_sensor[8]{};
    uint32_t thermal_management_temperature_transition_count[2]{};
    uint32_t total_time_for_thermal_transition_count[2]{};
    uint8_t reserved_232[280]{};
    uint8_t reserved_512[512]{};
};
static_assert(sizeof(LogPageSmart) == 1024, "Incorrect size");

/*! Size of single firmware revision field (in bytes) */
constexpr uint16_t FIRMWARE_REVISION_SIZE = 8;
/*! Maximum number of firmware revisions in the firmware log page */
constexpr uint16_t FIRMWARE_REVISION_COUNT = 7;

/*! Struct representing firmware log page */
struct LogPageFirmware final {
    uint8_t afi{};
    uint8_t reserved_1[7]{};
    uint8_t firmware_revision_for_slot[FIRMWARE_REVISION_COUNT][FIRMWARE_REVISION_SIZE]{};
    uint8_t reserved_64[448]{};
    uint8_t reserved_512[512]{};
};
static_assert(sizeof(LogPageFirmware) == 1024, "Incorrect size");

/*! Intel OEM latency tracking histogram log page*/
struct LogPageLatencyStats final {
    std::uint16_t major_version{};
    std::uint16_t minor_version{};
    std::uint32_t buckets_1st_group[32]{}; // range 0-1ms, step 32us
    std::uint32_t buckets_2nd_group[31]{}; // range 1-32ms, step 1ms
    std::uint32_t buckets_3rd_group[31]{}; // range 32ms-1s, step 32ms
    std::uint32_t buckets_4th_group[1]{}; // range 1024ms-2047ms, step 1s
    std::uint32_t buckets_5th_group[1]{}; // range 2048ms-4095ms, step 2s
    std::uint32_t buckets_6th_group[1]{}; // range 4s+
    std::uint8_t reserved_392[120]{};
    std::uint8_t reserved_512[512]{};
};
static_assert(sizeof(LogPageLatencyStats) == 1024, "Incorrect size");

/*! Intel OEM IO Queues log page*/
struct LogPageIOQueues final {
    std::uint16_t log_version{};
    std::uint16_t iosq_count{};
    std::uint16_t iocq_count{};

    struct {
        std::uint16_t iosq_id{};
        std::uint16_t associated_iocq_id{};
        std::uint16_t head_pointer{};
        std::uint16_t tail_pointer{};
        std::uint16_t outstanding_commands{};
        std::uint16_t queue_size{}; // Max depth
    } iosq_structs[32]{};

    struct {
        std::uint16_t iocq_id{};
        std::uint16_t head_pointer{};
        std::uint16_t tail_pointer{};
        std::uint16_t outstanding_commands{};
        std::uint16_t queue_size{}; // Max depth
    } iocq_structs[32]{};

    std::uint8_t reserved_710[314]{};
};
static_assert(sizeof(LogPageIOQueues) == 1024, "Incorrect size");

/*! Nvme SGL descriptor types */
enum class SglDescriptorType : uint8_t {
    DataBlock = 0x0,
    BitBucket = 0x1,
    Segment = 0x2,
    LastSegment = 0x3,
    KeyedDataBlock = 0x4,
    /* 0x5 - 0xE reserved */
    VendorSpecific = 0xF,
};

/*! Nvme SGL descriptor subtypes */
enum class SglDescriptorSubtype : uint8_t {
    Address = 0x0,
    Offset = 0x1,
    /* NVMEoF RDMA specific */
    InvalidateKey = 0xF
};

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnested-anon-types"
#endif

/*! Structure representing SGL descriptor */
struct SglDescriptor {
    uint64_t address;
    union {
        struct {
            uint8_t reserved[7];
            uint8_t subtype : 4;
            uint8_t type : 4;
        } generic;
        struct {
            uint32_t length;
            uint8_t reserved[3];
            uint8_t subtype : 4;
            uint8_t type : 4;
        } unkeyed;
        struct {
            uint64_t length : 24;
            uint64_t key : 32;
            uint64_t subtype : 4;
            uint64_t type : 4;
        } keyed;
    };
};
static_assert(sizeof(SglDescriptor) == 16, "Incorrect size");

/*!
 * @brief 64-byte (16 dwords) structure representing Nvme Command
 */
struct NvmeCmd {
    uint8_t opcode{};
    uint8_t fused_psdt{};
    uint16_t command_id{};
    uint32_t namespace_id{};
    uint64_t reserved_8{};
    uint64_t metadata_pointer{};

    union {

        struct {
            uint64_t address;
            uint32_t metadata_length;
            uint32_t data_length;
        } data;

        struct {
            uint64_t prp1;
            uint64_t prp2;
        } prp;

        SglDescriptor sgl1;
    } data_pointer{};

    union {
        // generic field representation - 24 bytes
        struct {
            uint32_t cdw10{};
            uint32_t cdw11{};
            uint32_t cdw12{};
            uint32_t cdw13{};
            uint32_t cdw14{};
            uint32_t cdw15{};
        } common{};

        // identify command fields
        struct {
            uint8_t cns{};
            uint8_t reserved_1{};
            uint16_t controller_id{};
            uint8_t reserved_4[20]{};
        } identify;

        // namespace management command fields
        struct {
            uint8_t sel{};
            uint8_t reserved_1[23]{};
        } namespace_management;

        // namespace attachment command fields
        struct {
            uint8_t sel{};
            uint8_t reserved_1[23]{};
        } namespace_attachment;

        // get log pages command fields
        struct {
            uint8_t log_page_id{};
            uint8_t lsp_rae{};
            uint32_t num_of_dwords{};
            uint8_t reserved_6[2]{};
            uint64_t log_page_offset{};
        } get_log_page;

        // format nvm command fields
        struct {
            uint8_t lbaf_mset_pi{};
            uint8_t pil_ses{};
            uint8_t reserved_2[22]{};
        } format_nvm;

        // get-features command fields
        struct {
            std::uint8_t fid{};
            std::uint8_t sel{};
            std::uint8_t reserved_2[2]{};
            std::uint8_t reserved_4[20]{};
        } get_features;

        // set-features command fields
        struct {
            std::uint8_t fid{};
            std::uint8_t reserved_1[2]{};
            struct {
                std::uint8_t reserved : 7;
                std::uint8_t sv : 1;
            } save{};
            std::uint32_t value{};
            std::uint8_t reserved_8[16]{};
        } set_features;
    };
};
static_assert(sizeof(NvmeCmd) == 64, "Incorrect size");

#ifdef __clang__
#pragma clang diagnostic pop
#endif

/*! Status code types */
enum class NvmeStatusCodeType {
    Generic = 0x0,
    CommandSpecific = 0x1,
    MediaError = 0x2,
    /* 0x3-0x6 - reserved */
    VendorSpecific = 0x7,
};

/*! Generic command status codes */
enum class NvmeGenericCommandStatusCode : uint8_t {
    Success = 0x00,
    InvalidOpcode = 0x01,
    InvalidField = 0x02,
    CommandIdConflict = 0x03,
    DataTransferError = 0x04,
    AbortedPowerLoss = 0x05,
    InternalDeviceError = 0x06,
    AbortedByRequest = 0x07,
    AbortedSQDeletion = 0x08,
    AbortedFailedFused = 0x09,
    AbortedMissingFused = 0x0a,
    InvalidNamespaceOrFormat = 0x0b,
    CommandSequenceError = 0x0c,
    InvalidSglSegDescriptor = 0x0d,
    InvalidNumSglDescriptors = 0x0e,
    DataSglLengthInvalid = 0x0f,
    MetadataSglLengthInvalid = 0x10,
    SglDescriptorTypeInvalid = 0x11,
    InvalidControllerMemBuf = 0x12,
    InvalidPrpOffset = 0x13,
    AtomicWriteUnitExceeded = 0x14,
    OperationDenied = 0x15,
    InvalidSglOffset = 0x16,
    /* 0x17 reserved */
    HostIdInconsistentFormat = 0x18,
    KeepAliveExpired = 0x19,
    KeepAliveInvalid = 0x1a,
    AbortedPreempt = 0x1b,
    SanitizeFailed = 0x1c,
    SanitizeInProgress = 0x1d,
    SglDataBlockGranularityInvalid = 0x1e,
    CommandInvalidInCmb = 0x1f,
    LbaOutOfRange = 0x80,
    CapacityExceeded = 0x81,
    NamespaceNotReady = 0x82,
    ReservationConflict = 0x83,
    FormatInProgress = 0x84,
};

/*! Nvme Status */
struct NvmeStatus {
    uint16_t phase_tag : 1;
    uint16_t status_code : 8;
    uint16_t status_code_type : 3;
    uint16_t reserved_2 : 2;
    uint16_t more : 1;
    uint16_t do_not_retry : 1;
};
static_assert(sizeof(NvmeStatus) == 2, "Incorrect size");

/*! Completion queue entry */
struct NvmeCQEntry {
    uint32_t cdw0; /* command-specific */
    uint32_t rsvd1;
    uint16_t submission_queue_head_pointer;
    uint16_t submission_qeue_id;
    uint16_t command_id;
    struct NvmeStatus status;
};
static_assert(sizeof(NvmeCQEntry) == 16, "Incorrect size");

/*!
 * @brief Structure containing Nvme Command
 * and two additional fields (timeout and result) for ioctls
 */
struct CommandData final {
    NvmeCmd cmd{};
    uint32_t timeout_ms{};
    uint32_t result{};
};
static_assert(sizeof(CommandData) == 72, "Incorrect size");

/*! Power State Descriptor returned by Identify command (32 bytes) */
struct PowerStateDescriptorData final {
    uint16_t max_power{};
    uint8_t reserved_2{};
    uint8_t mxps_nops{};
    uint32_t enlat{};
    uint32_t exlat{};
    uint8_t rrt{};
    uint8_t rrl{};
    uint8_t rwt{};
    uint8_t rwl{};
    uint16_t idle_power{};
    uint8_t idle_power_scale{};
    uint8_t reserved_19{};
    uint16_t active_power{};
    uint8_t active_power_workload_scale{};
    uint8_t reserved_23[9]{};
};
static_assert(sizeof(PowerStateDescriptorData) == 32, "Incorrect size");

constexpr uint16_t CONTROLLER_ID_DYNAMIC = 0xffff;

/*! Controller descriptor returned by Identify command (4096 bytes) */
struct ControllerData final {
    uint16_t pci_vendor_id{};
    uint16_t pci_subsystem_vendor_id{};
    uint8_t serial_number[20]{};
    uint8_t model_number[40]{};
    uint8_t firmware_revision[8]{};
    uint8_t rab{};
    uint8_t ieee[3]{};
    uint8_t cmic{};
    uint8_t mdts{};
    uint16_t controller_id{};
    uint32_t version{};
    uint32_t rtd3r{};
    uint32_t rtd3e{};
    uint32_t oaes{};
    uint32_t ctratt{};
    uint8_t reserved_100[12]{};
    uint8_t fru_guid[16]{};
    uint8_t reserved_128[128]{};
    uint16_t oacs{};
    uint8_t acl{};
    uint8_t aerl{};
    uint8_t frmw{};
    uint8_t lpa{};
    uint8_t elpe{};
    uint8_t npss{};
    uint8_t avscc{};
    uint8_t apsta{};
    uint16_t wctemp{};
    uint16_t cctemp{};
    uint16_t mtfa{};
    uint32_t hmpre{};
    uint32_t hmmin{};
    uint8_t tnvmcap[16]{};
    uint8_t unvmcap[16]{};
    uint32_t rpmbs{};
    uint16_t edstt{};
    uint8_t dsto{};
    uint8_t fwug{};
    uint16_t kas{};
    uint16_t hctma{};
    uint16_t mntmt{};
    uint16_t mxtmt{};
    uint32_t sanicap{};
    uint8_t reserved_332[180]{};
    uint8_t sqes{};
    uint8_t cqes{};
    uint16_t maxcmd{};
    uint32_t nn{};
    uint16_t oncs{};
    uint16_t fuses{};
    uint8_t fna{};
    uint8_t vwc{};
    uint16_t awun{};
    uint16_t awupf{};
    uint8_t nvscc{};
    uint8_t reserved_531{};
    uint16_t acwu{};
    uint8_t reserved_534[2]{};
    uint32_t sgls{};
    uint8_t reserved_540[228]{};
    uint8_t subsystem_nqn[256]{};
    uint8_t reserved_1024[768]{};
    uint8_t nvmeof[256]{};
    PowerStateDescriptorData psd[32]{};
    uint8_t vendor_specific[1024]{};
};
static_assert(sizeof(ControllerData) == 4096, "Incorrect size");

/*! LBA Format Data structure by Identify command (4 bytes) */
struct LbaFormatData final {
    uint16_t metadata_size{};
    uint8_t lba_data_size{};
    uint8_t relative_performance{};
};
static_assert(sizeof(LbaFormatData) == 4, "Incorrect size");

/*! Maximum number of Controller ids returned by Identify command */
constexpr uint16_t LBA_FORMAT_COUNT = 16;

/*! Namespace descriptor returned by Identify command (4096 bytes) */
struct NamespaceData final {
    uint64_t size{};
    uint64_t capacity{};
    uint64_t utilization{};
    uint8_t features{};
    uint8_t nlbaf{};
    uint8_t flbas{};
    uint8_t mc{};
    uint8_t dpc{};
    uint8_t dps{};
    uint8_t nmic{};
    uint8_t rescap{};
    uint8_t fpi{};
    uint8_t dlfeat{};
    uint16_t nawun{};
    uint16_t nawupf{};
    uint16_t nacwu{};
    uint16_t nabsn{};
    uint16_t nabo{};
    uint16_t nabspf{};
    uint16_t noiob{};
    uint64_t nvm_capacity[2]{};
    uint8_t reserved_64[40]{};
    uint8_t guid[16]{};
    uint8_t ieee_euid[8]{};
    LbaFormatData lbaf[LBA_FORMAT_COUNT]{};
    uint8_t reserved_192[192]{};
    uint8_t vendor_specific[3712]{};
};
static_assert(sizeof(NamespaceData) == 4096, "Incorrect size");

/*! Maximum number of Controller ids returned by Identify command */
constexpr uint16_t CONTROLLER_ID_LIST_MAX_SIZE = 2047;

/*! Controller Id list returned by Identify command */
struct ControllerIdList final {
    uint16_t size{};
    uint16_t controller_id[CONTROLLER_ID_LIST_MAX_SIZE]{};
};
static_assert(sizeof(ControllerIdList) == 4096, "Incorrect size");

/*! Maximum number of Namespace ids returned by Identify command */
constexpr uint16_t NAMESPACE_ID_LIST_MAX_SIZE = 1024;

/*! Namespace Id list returned by Identify command */
struct NamespaceIdList final {
    uint32_t namespace_id[NAMESPACE_ID_LIST_MAX_SIZE]{};
};
static_assert(sizeof(NamespaceIdList) == 4096, "Incorrect size");

/*! Valid NVMe controller versions */
enum class NvmeVersion : uint32_t {
    UNKNOWN = 0x00000000,
    NVME_1_0_0 = 0x00010000,
    NVME_1_1_0 = 0x00010100,
    NVME_1_2_0 = 0x00010200,
    NVME_1_2_1 = 0x00010201,
    NVME_1_3_0 = 0x00010300,
    INVALID = 0xFFFFFFFF
};

//
// NVMEoF
//

constexpr const char DiscoverySubsystemNqn[] = "nqn.2014-08.org.nvmexpress.discovery";

constexpr uint8_t NvmeFabricsCommandOpcode = 0x7f;

enum class NvmfCommandType : uint8_t {
    PropertySet = 0x00,
    Connect = 0x01,
    PropertyGet = 0x04,
    AuthenticationSend = 0x05,
    AuthenticationRecv = 0x06,
    StartVendorSpecific = 0xC0
};

enum class NvmfCommandStatusCode : uint8_t {
    IncompatibleFormat = 0x80,
    ControllerBusy = 0x81,
    InvalidParam = 0x82,
    RestartDiscovery = 0x83,
    InvalidHost = 0x84,
    LogRestartDiscovery = 0x90,
    AuthRequired = 0x91
};

/*! Size of transport service id field (in bytes) */
constexpr uint16_t TRANSPORT_SERVICE_ID_SIZE = 32;

/*! Size of transport address field (in bytes) */
constexpr uint16_t TRANSPORT_ADDRESS_SIZE = 256;

/*! Size of subsystem NQN field (in bytes) */
constexpr uint16_t SUBSYSTEM_NQN_SIZE = 256;

/*! Size of transport address subtype field (in bytes) */
constexpr uint16_t TRANSPORT_ADDRESS_SUBTYPE_SIZE = 256;

/*! Structure representing discovery log page header */
struct LogPageDiscoveryHeader final {
    uint64_t generation_counter{};
    uint64_t number_of_records{};
    uint16_t record_format{};
    uint8_t reserved_18[1006]{};
};
static_assert(sizeof(LogPageDiscoveryHeader) == 1024, "Incorrect size");

/* Transport Type of Discovery Log Page entry */
enum class TransportType : uint8_t {
    Rdma = 1,
    FibreChannel = 2,
    IntraHost = 254 /* Reserved for host usage */
};

/* Address Family of Discovery Log Page entry */
enum class AddressFamily : uint8_t {
    Ipv4 = 1,
    Ipv6 = 2,
    InfiniBand = 3,
    FibreChannel = 4,
    IntraHost = 254
};

/* Specifies the type of NVM subsystem of Discovery Log Page entry */
enum class SubsystemType : uint8_t {
    DiscoveryService = 1,
    Nvm = 2
};

/*
 * Indicates requirements for the NVMe Transport,
 * whether connections shall be made over a fabric secure channel.
 */
enum class TReqSecureChannel : uint8_t {
    NotSpecified = 0,
    Required = 1,
    NotRequired = 2
};

/* Rdma queue pair types */
enum class RdmaQPType : uint8_t {
    ReliableConnected = 1,
    ReliableDatagram = 2
};

/* Rdma provider types */
enum class RdmaProviderType : uint8_t {
    None = 1,
    InfiniBand = 2,
    RoCEv1 = 3,
    RoCEv2 = 4,
    Iwarp = 5
};

/* Rdma connection management service */
enum class RdmaCMS : uint8_t {
    RdmaCm = 1 /* Sockets based endpoint addressing */
};

constexpr uint16_t MIN_ADMIN_MAX_SQ_SIZE = 32;

/*! Structure representing discovery log entry */
struct LogPageDiscoveryEntry final {
    TransportType transport_type{};
    AddressFamily address_family{};
    SubsystemType subsystem_type{};
    TReqSecureChannel treq_secure_channel{};
    uint16_t port_id{};
    uint16_t controller_id{};
    uint16_t admin_max_sq_size{};
    uint8_t reserved_10[22]{};
    uint8_t transport_service_id[TRANSPORT_SERVICE_ID_SIZE]{};
    uint8_t reserved_64[192]{};
    uint8_t subsystem_nqn[SUBSYSTEM_NQN_SIZE]{};
    uint8_t transport_address[TRANSPORT_ADDRESS_SIZE]{};
    union TransportSpecificAddressSubtype {
        char common[TRANSPORT_ADDRESS_SUBTYPE_SIZE];
        struct Rdma {
            RdmaQPType qptype;
            RdmaProviderType prtype;
            RdmaCMS cms;
            uint8_t reserved_3[5];
            uint16_t pkey;
            uint8_t reserved_10[246];
        } rdma;
    } tsas{};
};
static_assert(sizeof(LogPageDiscoveryEntry) == 1024, "Incorrect size");

struct NvmfCapsuleCmd {
    uint8_t opcode;
    uint8_t reserved1;
    uint16_t cid;
    NvmfCommandType fctype;
    uint8_t reserved2[35];
    uint8_t fabric_specific[24];
};
static_assert(sizeof(NvmfCapsuleCmd) == 64, "Incorrect size");

struct NvmfConnectData {
    uint8_t hostid[16];
    uint16_t cntlid;
    uint8_t reserved5[238];
    uint8_t subnqn[256];
    uint8_t hostnqn[256];
    uint8_t reserved6[256];
};
static_assert(sizeof(NvmfConnectData) == 1024, "Incorrect size");

struct NvmfConnectCmd {
    uint8_t opcode;
    uint8_t reserved1;
    uint16_t cid;
    NvmfCommandType fctype;
    uint8_t reserved2[19];
    struct SglDescriptor sgl1;
    uint16_t recfmt; /* Connect Record Format */
    uint16_t qid; /* Queue Identifier */
    uint16_t sqsize; /* Submission Queue Size */
    uint8_t cattr; /* queue attributes */
    uint8_t reserved3;
    uint32_t kato; /* keep alive timeout */
    uint8_t reserved4[12];
};
static_assert(sizeof(NvmfConnectCmd) == 64, "Incorrect size");

struct NvmfConnectRsp {
    union {
        struct {
            uint16_t cntlid;
            uint16_t authreq;
        } success;
        struct {
            uint16_t ipo;
            uint8_t iattr;
            uint8_t reserved;
        } invalid;
        uint32_t raw;
    } status_code_specific;
    uint32_t reserved0;
    uint16_t sqhd;
    uint16_t reserved1;
    uint16_t cid;
    struct NvmeStatus status;
};
static_assert(sizeof(NvmfConnectRsp) == 16, "Incorrect size");

struct NvmfPropertyGetCmd {
    uint8_t opcode;
    uint8_t reserved1;
    uint16_t cid;
    NvmfCommandType fctype;
    uint8_t reserved2[35];
    struct {
        uint8_t size : 2;
        uint8_t reserved : 6;
    } attrib;
    uint8_t reserved3[3];
    NvmeCtrlRegisterOffset ofst;
    uint8_t reserved4[16];
};
static_assert(sizeof(NvmfPropertyGetCmd) == 64, "Incorrect size");

struct NvmfPropertyGetRsp {
    union {
        uint64_t u64;
        struct {
            uint32_t low;
            uint32_t high;
        } u32;
    } value;
    uint16_t sqhd;
    uint16_t reserved0;
    uint16_t cid;
    struct NvmeStatus status;
};
static_assert(sizeof(NvmfPropertyGetRsp) == 16, "Incorrect size");

struct NvmfPropertySetCmd {
    uint8_t opcode;
    uint8_t reserved0;
    uint16_t cid;
    NvmfCommandType fctype;
    uint8_t reserved1[35];
    struct {
        uint8_t size : 2;
        uint8_t reserved : 6;
    } attrib;
    uint8_t reserved2[3];
    NvmeCtrlRegisterOffset ofst;
    union {
        uint64_t u64;
        struct {
            uint32_t low;
            uint32_t high;
        } u32;
    } value;
    uint8_t reserved4[8];
};
static_assert(sizeof(NvmfPropertySetCmd) == 64, "Incorrect size");

#pragma pack(pop)

}
