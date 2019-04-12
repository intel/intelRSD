/*
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.intel.rsd.nodecomposer.types;

public enum Protocol implements EnumeratedType {
    PCIE("PCIe", 1),
    AHCI("AHCI", 2),
    UHCI("UHCI", 3),
    SAS("SAS", 4),
    SATA("SATA", 5),
    USB("USB", 6),
    NVME("NVMe", 7),
    FC("FC", 8),
    ISCSI("iSCSI", 9),
    FCOE("FCoE", 10),
    NVME_OVER_FABRICS("NVMeOverFabrics", 11),
    SMB("SMB", 12),
    NFS_V3("NFSv3", 13),
    NFS_V4("NFSv4", 14),
    HTTP("HTTP", 15),
    HTTPS("HTTPS", 16),
    FTP("FTP", 17),
    SFTP("SFTP", 18),
    IWARP("iWARP", 19),
    ROCE("RoCE", 20),
    ROCEV2("RoCEv2", 21),
    OEM("OEM", 22),
    FPGA_OVER_FABRICS("FPGA-oF", 23);

    private final String value;
    private final Integer priority;

    Protocol(String value, Integer priority) {
        this.value = value;
        this.priority = priority;
    }

    @Override
    public String getValue() {
        return value;
    }

    @Override
    public Integer getPriority() {
        return priority;
    }

    @Override
    public String toString() {
        return getValue();
    }
}
