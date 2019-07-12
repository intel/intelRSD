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

package com.intel.rsd.nodecomposer.composition.allocation.strategy.matcher;

import com.intel.rsd.nodecomposer.business.Violations;
import com.intel.rsd.nodecomposer.business.services.redfish.requests.RequestedNode.RemoteDrive;
import com.intel.rsd.nodecomposer.composition.allocation.strategy.RemoteDriveAllocationContextDescriber;
import com.intel.rsd.nodecomposer.composition.allocation.strategy.RemoteDriveAllocationContextDescriptor;
import com.intel.rsd.nodecomposer.composition.allocation.strategy.ResourceFinderException;
import com.intel.rsd.nodecomposer.persistence.dao.GenericDao;
import com.intel.rsd.nodecomposer.persistence.redfish.StoragePool;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;

import java.math.BigDecimal;
import java.util.Optional;

import static com.intel.rsd.nodecomposer.utils.Converters.convertGibToBytes;

@Component
public class NewDriveAllocationContextDescriber implements RemoteDriveAllocationContextDescriber {
    @Autowired
    private GenericDao genericDao;

    public RemoteDriveAllocationContextDescriptor describe(RemoteDrive remoteDrive) throws ResourceFinderException {
        RemoteDriveAllocationContextDescriptor resource = new RemoteDriveAllocationContextDescriptor();

        Optional<StoragePool> pool = getAnyPoolMatchingRequest(remoteDrive);

        if (!pool.isPresent()) {
            throw new ResourceFinderException(Violations.createWithViolations("There is no Storage pool with sufficient space available."));
        }

        StoragePool selectedPool = pool.get();

        resource.setStoragePoolODataId(selectedPool.getUri());
        resource.setStorageServiceODataId(selectedPool.getStorageService().getUri());
        resource.setCapacity(remoteDrive.getCapacityGib());
        resource.setProtocol(remoteDrive.getProtocol());

        return resource;
    }

    private Optional<StoragePool> getAnyPoolMatchingRequest(RemoteDrive remoteDrive) {
        BigDecimal requestedSize = convertGibToBytes(remoteDrive.getCapacityGib());

        return genericDao.findAll(StoragePool.class).stream()
            .filter(storagePool -> hasRequiredSpace(storagePool, requestedSize))
            .filter(storagePool -> remoteDrive.getProtocol().equals(storagePool.getProtocol()))
            .findFirst();
    }
}
