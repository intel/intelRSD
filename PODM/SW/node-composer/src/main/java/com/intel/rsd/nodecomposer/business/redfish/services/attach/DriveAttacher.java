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

package com.intel.rsd.nodecomposer.business.redfish.services.attach;

import com.intel.rsd.nodecomposer.business.Violations;
import com.intel.rsd.nodecomposer.business.redfish.services.helpers.DriveHelper;
import com.intel.rsd.nodecomposer.persistence.dao.DriveDao;
import com.intel.rsd.nodecomposer.persistence.redfish.ComposedNode;
import com.intel.rsd.nodecomposer.persistence.redfish.Drive;
import com.intel.rsd.nodecomposer.persistence.redfish.Endpoint;
import com.intel.rsd.nodecomposer.persistence.redfish.Fabric;
import com.intel.rsd.nodecomposer.types.Protocol;
import org.springframework.stereotype.Component;

import java.util.Set;

import static com.intel.rsd.nodecomposer.business.redfish.services.attach.Attacher.ATTACHER_BEAN_PREFIX;
import static com.intel.rsd.nodecomposer.business.redfish.services.helpers.DriveHelper.isRemoteDrive;
import static com.intel.rsd.nodecomposer.types.AttachableType.Types.DRIVE;

@Component(ATTACHER_BEAN_PREFIX + DRIVE)
public class DriveAttacher extends Attacher<Drive> {
    private final DriveDao driveDao;
    private final DriveHelper driveHelper;

    public DriveAttacher(DriveDao driveDao, DriveHelper driveHelper) {
        this.driveDao = driveDao;
        this.driveHelper = driveHelper;
    }

    @Override
    protected Fabric retrieveFabricFromAsset(Drive drive) {
        return driveHelper.retrieveFabricFromDrive(drive);
    }

    @Override
    protected Protocol retrieveAssetProtocol(Drive drive) {
        return drive.getProtocol();
    }

    @Override
    public Violations getAssetSpecificViolations(ComposedNode composedNode, Drive drive) {
        Violations violations = new Violations();

        addDriveNotAchievableViolation(composedNode, drive, violations);
        addDriveEndpointsAttachedViolation(drive, violations);

        return violations;
    }

    private void addDriveEndpointsAttachedViolation(Drive drive, Violations violations) {
        if (!drive.getEndpoints().isEmpty() && drive.getEndpoints().stream().noneMatch(Endpoint::isAttachable)) {
            violations.addViolation("All endpoints attached to selected drive are in use.");
        }
    }

    private void addDriveNotAchievableViolation(ComposedNode composedNode, Drive drive, Violations violations) {
        if (!isRemoteDrive(drive)) {
            Set<Drive> achievableDrives = driveDao.getAchievablePcieDrives(composedNode.getComputerSystem());
            if (!achievableDrives.contains(drive)) {
                violations.addViolation("Selected Drive is not achievable for this node!");
            }
        }
    }
}
