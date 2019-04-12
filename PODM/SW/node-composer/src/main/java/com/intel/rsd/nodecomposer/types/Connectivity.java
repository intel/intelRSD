/*
 * Copyright (c) 2018-2019 Intel Corporation
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

import java.util.EnumMap;
import java.util.Map;
import java.util.Optional;

import static com.intel.rsd.nodecomposer.types.Protocol.FPGA_OVER_FABRICS;
import static com.intel.rsd.nodecomposer.types.Protocol.PCIE;
import static java.util.Optional.ofNullable;

public enum Connectivity implements EnumeratedType {
    LOCAL("Local", 1),
    ETHERNET("Ethernet", 2),
    REMOTE_PCI("RemotePCIe", 3);

    private static final Map<Protocol, Connectivity> PROTOCOL_CONNECTIVITY_MAPPING = new EnumMap<>(Protocol.class);

    static {
        PROTOCOL_CONNECTIVITY_MAPPING.put(PCIE, REMOTE_PCI);
        PROTOCOL_CONNECTIVITY_MAPPING.put(FPGA_OVER_FABRICS, ETHERNET);
    }

    private String value;
    private Integer priority;

    Connectivity(String connectionMethod, Integer priority) {
        this.value = connectionMethod;
        this.priority = priority;
    }

    public static Optional<Connectivity> findByProtocol(Protocol fabricType) {
        return ofNullable(PROTOCOL_CONNECTIVITY_MAPPING.get(fabricType));
    }

    @Override
    public String getValue() {
        return value;
    }

    @Override
    public Integer getPriority() {
        return priority;
    }
}
