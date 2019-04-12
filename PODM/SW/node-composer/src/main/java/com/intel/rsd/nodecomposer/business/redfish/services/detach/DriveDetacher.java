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

package com.intel.rsd.nodecomposer.business.redfish.services.detach;

import com.intel.rsd.nodecomposer.business.RequestValidationException;
import com.intel.rsd.nodecomposer.business.redfish.services.actions.SecureEraseInvoker;
import com.intel.rsd.nodecomposer.business.redfish.services.helpers.DriveHelper;
import com.intel.rsd.nodecomposer.externalservices.resources.redfish.DriveResource;
import com.intel.rsd.nodecomposer.mappers.redfish.DriveMapper;
import com.intel.rsd.nodecomposer.persistence.redfish.ComposedNode;
import com.intel.rsd.nodecomposer.persistence.redfish.Drive;
import com.intel.rsd.nodecomposer.persistence.redfish.Fabric;
import lombok.SneakyThrows;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import static com.intel.rsd.nodecomposer.business.Violations.createWithViolations;
import static com.intel.rsd.nodecomposer.types.AttachableType.Types.DRIVE;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_SINGLETON;

@Component(DRIVE)
@Scope(SCOPE_SINGLETON)
public class DriveDetacher extends Detacher<Drive> {
    private final DriveMapper driveMapper;
    private final SecureEraseInvoker<Drive, DriveResource> secureEraseInvoker;
    private final DriveHelper driveHelper;

    @Autowired
    public DriveDetacher(DriveMapper driveMapper, SecureEraseInvoker<Drive, DriveResource> secureEraseInvoker, DriveHelper driveHelper) {
        this.driveMapper = driveMapper;
        this.secureEraseInvoker = secureEraseInvoker;
        this.driveHelper = driveHelper;
    }

    @Override
    protected void validate(ComposedNode composedNode, Drive drive) throws RequestValidationException {
        if (!composedNode.getDrives().contains(drive)) {
            throw new RequestValidationException(createWithViolations("Provided drive is not attached to this Node."));
        }
    }

    @Override
    protected Fabric retrieveFabricFromAsset(Drive drive) {
        return driveHelper.retrieveFabricFromDrive(drive);
    }

    @SneakyThrows
    @Override
    protected void performResourceSpecificActions(ComposedNode composedNode, Drive drive) {
        secureEraseInvoker.updateErased(drive, false, driveMapper);
    }
}
