/*
 * Copyright (c) 2017 Intel Corporation
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
import com.intel.podm.business.entities.redfish.StorageController;
import com.intel.podm.business.entities.redfish.base.Entity;
import com.intel.podm.client.WebClientRequestException;
import com.intel.podm.client.reader.ResourceSupplier;
import com.intel.podm.client.resources.redfish.ComputerSystemResource;
import com.intel.podm.client.resources.redfish.StorageControllerResource;

import javax.enterprise.context.Dependent;
import java.util.Objects;
import java.util.Optional;

@Dependent
public class StorageControllerObtainerHelper implements EntityObtainerHelper<StorageControllerResource> {

    @Override
    public ComputerSystemResource findComputerSystemResourceFor(StorageControllerResource resource) throws WebClientRequestException {
        ResourceSupplier resourceComputerSystem = resource.getComputerSystem();
        if (resourceComputerSystem == null) {
            throw new WebClientRequestException("Could not find ComputerSystem resource associated with specified StorageController",
                null, new IllegalStateException("StorageController -> ComputerSystem link is missing"));
        }
        return (ComputerSystemResource) resourceComputerSystem.get();
    }

    @Override
    public Optional<StorageController> findEntityFor(ComputerSystem computerSystem, StorageControllerResource resource) {
        String resourceStorageControllerName = resource.getName();

        return computerSystem.getStorages().stream()
            .flatMap(storage -> storage.getStorageControllers().stream())
            .filter(storageController -> Objects.equals(storageController.getName(), resourceStorageControllerName))
            .findFirst();
    }

    @Override
    public Class<? extends Entity> getEntityClass() {
        return StorageController.class;
    }

    @Override
    public Class<StorageControllerResource> getResourceClass() {
        return StorageControllerResource.class;
    }
}
