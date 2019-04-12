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

import java.util.Map;
import java.util.Optional;

import static com.intel.rsd.nodecomposer.types.Protocol.FPGA_OVER_FABRICS;
import static com.intel.rsd.nodecomposer.types.Protocol.ISCSI;
import static com.intel.rsd.nodecomposer.types.Protocol.NVME_OVER_FABRICS;
import static com.intel.rsd.nodecomposer.types.Protocol.PCIE;
import static java.util.Arrays.stream;
import static java.util.Optional.ofNullable;
import static java.util.stream.Collectors.toMap;

public enum DurableNameFormat implements EnumeratedType {
    NAA("NAA"),
    IQN("iQN", ISCSI),
    FC_WWN("FC_WWN"),
    UUID("UUID", PCIE),
    EUI("EUI"),
    NSID("NSID"),
    NQN("NQN", NVME_OVER_FABRICS),
    SYSTEM_PATH("SystemPath");

    private static final Map<Protocol, DurableNameFormat> PROTOCOL_DURABLE_NAME_FORMAT_MAPPING;

    static {
        PROTOCOL_DURABLE_NAME_FORMAT_MAPPING = stream(DurableNameFormat.values())
            .filter(durableNameFormat -> durableNameFormat.relatedProtocol() != null)
            .collect(toMap(DurableNameFormat::relatedProtocol, durableNameFormat -> durableNameFormat, (a, b) -> b));

        //TODO: to be updated when protocol and DurableName format will be chosen for FPGAOverFabrics
        PROTOCOL_DURABLE_NAME_FORMAT_MAPPING.put(FPGA_OVER_FABRICS, UUID);
    }

    private final String value;
    private final Protocol protocol;

    DurableNameFormat(String value) {
        this.value = value;
        this.protocol = null;
    }

    DurableNameFormat(String value, Protocol protocol) {
        this.value = value;
        this.protocol = protocol;
    }

    public static Optional<DurableNameFormat> findByProtocol(Protocol fabricType) {
        return ofNullable(PROTOCOL_DURABLE_NAME_FORMAT_MAPPING.get(fabricType));
    }

    @Override
    public String getValue() {
        return value;
    }

    public Protocol relatedProtocol() {
        return protocol;
    }

    @Override
    public String toString() {
        return getValue();
    }
}
