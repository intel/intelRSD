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

package com.intel.rsd.nodecomposer.business.redfish.services.helpers;

import com.intel.rsd.nodecomposer.persistence.redfish.Drive;
import com.intel.rsd.nodecomposer.persistence.redfish.Fabric;
import com.intel.rsd.nodecomposer.persistence.redfish.Switch;
import com.intel.rsd.nodecomposer.types.Protocol;
import org.springframework.stereotype.Component;

import java.util.List;
import java.util.Objects;

import static com.intel.rsd.nodecomposer.types.Protocol.NVME;
import static com.intel.rsd.nodecomposer.types.Protocol.PCIE;
import static com.intel.rsd.nodecomposer.utils.Collector.toSingle;
import static java.util.Arrays.asList;

@Component
public class DriveHelper {
    private static final List<Protocol> REMOTE_DRIVE_PROTOCOLS = asList(PCIE, NVME);

    public Fabric retrieveFabricFromDrive(Drive drive) {
        return getFabricFromDrive(drive);
    }

    public static boolean isRemoteDrive(Drive drive) {
        return REMOTE_DRIVE_PROTOCOLS.contains(getProtocolFromDrive(drive));
    }

    public static Protocol getProtocolFromDrive(Drive drive) {
        return drive.getProtocol();
    }

    private static Fabric getFabricFromDrive(Drive drive) {
        return drive.getChassis().getSwitch()
            .stream()
            .map(Switch::getFabric)
            .filter(Objects::nonNull)
            .filter(fabric -> REMOTE_DRIVE_PROTOCOLS.contains(fabric.getFabricType()))
            .collect(toSingle());
    }
}
