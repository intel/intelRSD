/*
 * Copyright (c) 2015-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.composition.allocation.strategy;

import com.intel.rsd.nodecomposer.business.Violations;
import com.intel.rsd.nodecomposer.business.redfish.services.helpers.VolumeHelper;
import com.intel.rsd.nodecomposer.business.services.redfish.requests.RequestedNode.RemoteDrive;
import com.intel.rsd.nodecomposer.business.services.redfish.requests.RequestedNode.RemoteDrive.MasterDrive;
import com.intel.rsd.nodecomposer.persistence.EntityNotFoundException;
import com.intel.rsd.nodecomposer.persistence.dao.GenericDao;
import com.intel.rsd.nodecomposer.persistence.redfish.StoragePool;
import com.intel.rsd.nodecomposer.persistence.redfish.Volume;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;

import java.math.BigDecimal;

import static com.intel.rsd.nodecomposer.utils.Converters.convertBytesToGib;

@Component
public class MasterBasedNewDriveAllocationContextDescriber implements RemoteDriveAllocationContextDescriber {
    private final GenericDao genericDao;
    private final VolumeHelper volumeHelper;

    @Autowired
    public MasterBasedNewDriveAllocationContextDescriber(GenericDao genericDao, VolumeHelper volumeHelper) {
        this.genericDao = genericDao;
        this.volumeHelper = volumeHelper;
    }

    public RemoteDriveAllocationContextDescriptor describe(RemoteDrive remoteDrive) throws ResourceFinderException {
        RemoteDriveAllocationContextDescriptor descriptor = new RemoteDriveAllocationContextDescriptor();

        Volume masterDrive = getMasterDrive(remoteDrive.getMaster());

        validateStoragePool(masterDrive);

        descriptor.setMasterUri(masterDrive.getUri().toUri());
        descriptor.setStoragePoolODataId(masterDrive.getStoragePool().getUri());
        descriptor.setStorageServiceODataId(masterDrive.getStorageService().getUri());
        descriptor.setCapacity(getCapacity(remoteDrive, masterDrive));
        descriptor.setProtocol(volumeHelper.retrieveProtocolFromVolume(masterDrive));
        descriptor.setReplicaType(remoteDrive.getMaster().getType());

        return descriptor;
    }

    private void validateStoragePool(Volume masterDrive) throws ResourceFinderException {
        StoragePool storagePool = masterDrive.getStoragePool();
        if (storagePool == null) {
            throw new ResourceFinderException(Violations.createWithViolations("Storage pool was not found on Volume."));
        }

        if (!hasRequiredSpace(storagePool, masterDrive.getCapacityBytes())) {
            throw new ResourceFinderException(Violations.createWithViolations("Storage pool with sufficient space was not found."));
        }

    }

    private BigDecimal getCapacity(RemoteDrive remoteDrive, Volume masterDrive) {
        if (remoteDrive.getCapacityGib() != null) {
            return remoteDrive.getCapacityGib();
        }

        return convertBytesToGib(masterDrive.getCapacityBytes());
    }

    private Volume getMasterDrive(MasterDrive masterDrive) {
        try {
            return genericDao.find(Volume.class, masterDrive.getResourceODataId());
        } catch (EntityNotFoundException e) {
            throw new IllegalStateException("Provided master drive no longer exist.");
        }
    }
}
