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
import com.intel.podm.business.entities.redfish.Drive;
import com.intel.podm.business.entities.redfish.Storage;
import com.intel.podm.business.entities.redfish.base.Entity;
import com.intel.podm.client.WebClientRequestException;
import com.intel.podm.client.reader.ResourceSupplier;
import com.intel.podm.client.resources.redfish.ChassisResource;
import com.intel.podm.client.resources.redfish.ComputerSystemResource;
import com.intel.podm.client.resources.redfish.DriveResource;

import javax.enterprise.context.Dependent;
import java.util.Iterator;
import java.util.Objects;
import java.util.Optional;
import java.util.function.Function;
import java.util.function.Predicate;

import static java.lang.String.format;
import static java.util.Objects.isNull;

@Dependent
public class DriveObtainerHelper implements EntityObtainerHelper<DriveResource> {

    @Override
    public ComputerSystemResource findComputerSystemResourceFor(DriveResource resource) throws WebClientRequestException {
        ChassisResource chassisResource = (ChassisResource) resource.getParentChassis().get();
        Iterator<ResourceSupplier> iterator = chassisResource.getComputerSystems().iterator();

        if (!iterator.hasNext()) {
            throw new IllegalStateException(format("Parent ComputerSystem resource has not been found for '%s'", resource.getUri()));
        }
        return (ComputerSystemResource) iterator.next().get();
    }

    @Override
    public Optional<? extends Entity> findEntityFor(ComputerSystem computerSystem, DriveResource resource) {
        return computerSystem.getStorages().stream()
                .findFirst()
                .flatMap(toDriveMatchingCriteria(resource));
    }

    private Function<Storage, Optional<Drive>> toDriveMatchingCriteria(DriveResource resource) {
        return storage -> storage.getDrives().stream()
                .filter(bySerialNumber(resource))
                .findFirst();
    }

    private Predicate<? super Drive> bySerialNumber(DriveResource resource) {
        return drive -> {
            String serialNumberFromDrive = drive.getSerialNumber();
            String serialNumberFromResource = resource.getSerialNumber().orElse(null);

            if (isNull(serialNumberFromDrive) && isNull(serialNumberFromResource)) {
                return false;
            }

            return Objects.equals(serialNumberFromDrive, serialNumberFromResource);
        };
    }

    @Override
    public Class<? extends Entity> getEntityClass() {
        return Drive.class;
    }

    @Override
    public Class<DriveResource> getResourceClass() {
        return DriveResource.class;
    }
}
