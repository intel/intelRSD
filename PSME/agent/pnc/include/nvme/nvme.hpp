/*!
 * @copyright
 * Copyright (c) 2016-2019 Intel Corporation
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
 * @file nvme.hpp
 * @brief NVMe drive secure erase necessary structures
 * */

#pragma once

#pragma pack(push, 1)

/*! SecureEraseSettings mask */
static constexpr uint32_t SECURE_ERASE_SETTINGS_MASK = 0x00000E00;

/*! Protection Information Location mask */
static constexpr uint32_t PROTECTION_INFORMATION_LOCATION_MASK = 0x00000100;

/*! Protection Information mask */
static constexpr uint32_t PROTECTION_INFORMATION_MASK = 0x000000E0;

/*! Metadata settings mask */
static constexpr uint32_t METADATA_SETTINGS_MASK = 0x00000010;

/*! LBA (logical block addressing) format mask */
static constexpr uint32_t LBAF_MASK = 0x0000000F;

/*! Secure Erase Settings on bits 11:09 */
enum class SecureEraseSettings : std::uint32_t {
    NO_SECURE_ERASE = 0x00000000,
    USER_DATA_ERASE = 0x00000200,
    CRYPTOGRAPHIC_ERASE = 0x00000400
};

/*! Protection Information Location in metadata on bit 8*/
enum class ProtectionInformationLocation : std::uint32_t {
    LAST_8_BYTES = 0x00000000,
    FIRST_8_BYTES = 0x00000100
};

/*! Metadata settings on bit 4*/
enum class MetadataSettings : std::uint8_t {
    PART_OF_SEPARATE_BUFFER = 0x00000000,
    PART_OF_EXTENDED_DATA_LBA = 0x00000010
};

/*! NVMEe power state structure */
struct nvme_id_power_state {
    uint16_t max_power;
    uint8_t rsvd2;
    uint8_t flags;
    uint32_t entry_lat;
    uint32_t exit_lat;
    uint8_t read_tput;
    uint8_t read_lat;
    uint8_t write_tput;
    uint8_t write_lat;
    uint16_t idle_power;
    uint8_t idle_scale;
    uint8_t rsvd19;
    uint16_t active_power;
    uint8_t active_work_scale;
    uint8_t rsvd23[9];
};

/*! NVMEe common command structure */
struct nvme_common_command {
    uint8_t opcode;
    uint8_t flags;
    uint16_t command_id;
    uint32_t nsid;
    uint32_t cdw2[2];
    uint64_t metadata;
    uint64_t prp1;
    uint64_t prp2;
    uint32_t cdw10[6];
};

/*! NVMEe identify controller command response structure */
struct nvme_id_ctrl {
    uint16_t vid;
    uint16_t ssvid;
    char sn[20];
    char mn[40];
    char fr[8];
    uint8_t rab;
    uint8_t ieee[3];
    uint8_t mic;
    uint8_t mdts;
    uint16_t cntlid;
    uint32_t ver;
    uint8_t rsvd84[172];
    uint16_t oacs;
    uint8_t acl;
    uint8_t aerl;
    uint8_t frmw;
    uint8_t lpa;
    uint8_t elpe;
    uint8_t npss;
    uint8_t avscc;
    uint8_t apsta;
    uint16_t wctemp;
    uint16_t cctemp;
    uint8_t rsvd270[242];
    uint8_t sqes;
    uint8_t cqes;
    uint8_t rsvd514[2];
    uint32_t nn;
    uint16_t oncs;
    uint16_t fuses;
    uint8_t fna;
    uint8_t vwc;
    uint16_t awun;
    uint16_t awupf;
    uint8_t nvscc;
    uint8_t rsvd531;
    uint16_t acwu;
    uint8_t rsvd534[2];
    uint32_t sgls;
    uint8_t rsvd540[1508];
    struct nvme_id_power_state psd[32];
    uint8_t vs[1024];
};

/*! NVMEe LBA format structure */
struct nvme_lbaf {
    uint16_t ms;
    uint8_t ds;
    uint8_t rp;
};

/*! NVMEe identify namespace command response structure */
struct nvme_id_ns {
    uint64_t nsze;
    uint64_t ncap;
    uint64_t nuse;
    uint8_t nsfeat;
    uint8_t nlbaf;
    uint8_t flbas;
    uint8_t mc;
    uint8_t dpc;
    uint8_t dps;
    uint8_t nmic;
    uint8_t rescap;
    uint8_t fpi;
    uint8_t rsvd33;
    uint16_t nawun;
    uint16_t nawupf;
    uint16_t nacwu;
    uint16_t nabsn;
    uint16_t nabo;
    uint16_t nabspf;
    uint16_t rsvd46;
    uint64_t nvmcap[2];
    uint8_t rsvd64[40];
    uint8_t nguid[16];
    uint8_t eui64[8];
    struct nvme_lbaf lbaf[16];
    uint8_t rsvd192[192];
    uint8_t vs[3712];
};

/*! NVMEe Admin command opcodes */
enum nvme_admin_opcode {
    nvme_admin_delete_sq = 0x00,
    nvme_admin_create_sq = 0x01,
    nvme_admin_get_log_page = 0x02,
    nvme_admin_delete_cq = 0x04,
    nvme_admin_create_cq = 0x05,
    nvme_admin_identify = 0x06,
    nvme_admin_abort_cmd = 0x08,
    nvme_admin_set_features = 0x09,
    nvme_admin_get_features = 0x0a,
    nvme_admin_async_event = 0x0c,
    nvme_admin_activate_fw = 0x10,
    nvme_admin_download_fw = 0x11,
    nvme_admin_format_nvm = 0x80,
    nvme_admin_security_send = 0x81,
    nvme_admin_security_recv = 0x82,
};

/*! NVMEe identify command structure */
struct nvme_identify {
    uint8_t opcode;
    uint8_t flags;
    uint16_t command_id;
    uint32_t nsid;
    uint64_t rsvd2[2];
    uint64_t prp1;
    uint64_t prp2;
    uint32_t cns;
    uint32_t rsvd11[5];
    uint32_t timeout_ms;
    uint32_t result;
};

/*! NVMEe format command structure */
struct nvme_format_cmd {
    uint8_t opcode;
    uint8_t flags;
    uint16_t command_id;
    uint32_t nsid;
    uint64_t rsvd2[4];
    uint32_t cdw10;
    uint32_t rsvd11[5];
    uint32_t timeout_ms;
    uint32_t result;
};

/*! NVMEe command union */
struct nvme_command {
    union {
        struct nvme_identify identify;
        struct nvme_format_cmd format;
    };
};

#pragma pack(pop)
