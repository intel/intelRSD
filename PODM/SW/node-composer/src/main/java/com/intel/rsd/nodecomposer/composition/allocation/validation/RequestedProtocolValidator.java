/*
 * Copyright (c) 2017-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.composition.allocation.validation;

import com.intel.rsd.nodecomposer.business.Violations;
import com.intel.rsd.nodecomposer.types.Protocol;
import org.springframework.stereotype.Component;

import java.util.EnumSet;

import static com.intel.rsd.nodecomposer.types.Protocol.ISCSI;
import static com.intel.rsd.nodecomposer.types.Protocol.NVME_OVER_FABRICS;

@Component
public class RequestedProtocolValidator {
    private static final EnumSet<Protocol> SUPPORTED_PROTOCOLS = EnumSet.of(NVME_OVER_FABRICS, ISCSI);

    public Violations validate(Protocol protocol) {
        Violations violations = new Violations();

        if (!SUPPORTED_PROTOCOLS.contains(protocol)) {
            violations.addViolation("Unsupported protocol for RemoteDrive, choose one of " + SUPPORTED_PROTOCOLS);
        }
        return violations;
    }
}
