/*
 * Copyright (c) 2016-2017 Intel Corporation
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

package com.intel.podm.discovery.external.matchers;

import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.Storage;
import com.intel.podm.business.entities.redfish.base.Entity;
import com.intel.podm.client.WebClientRequestException;
import com.intel.podm.client.reader.ResourceSupplier;
import com.intel.podm.client.resources.ExternalServiceResource;
import com.intel.podm.client.resources.redfish.ChassisResource;
import com.intel.podm.client.resources.redfish.ComputerSystemResource;
import com.intel.podm.client.resources.redfish.StorageResource;

import javax.enterprise.context.Dependent;
import java.util.Iterator;
import java.util.Optional;

import static java.lang.String.format;

@Dependent
public class StorageObtainerHelper implements EntityObtainerHelper<StorageResource> {

    @Override
    public ComputerSystemResource findComputerSystemResourceFor(StorageResource luiStorageResource) throws WebClientRequestException {
        ChassisResource chassisResource = (ChassisResource) luiStorageResource.getChassis().get();
        Iterator iterator = chassisResource.getComputerSystems().iterator();

        if (!iterator.hasNext()) {
            throw new IllegalStateException(format("Parent ComputerSystem resource has not been found for '%s'", chassisResource.getUri()));
        }

        ExternalServiceResource externalServiceResource = ((ResourceSupplier) iterator.next()).get();
        return (ComputerSystemResource) externalServiceResource;
    }

    @Override
    public Optional<Storage> findEntityFor(ComputerSystem computerSystem, StorageResource resource) {
        return computerSystem.getStorages().stream().findFirst();
    }

    @Override
    public Class<? extends Entity> getEntityClass() {
        return Storage.class;
    }

    @Override
    public Class<StorageResource> getResourceClass() {
        return StorageResource.class;
    }
}
