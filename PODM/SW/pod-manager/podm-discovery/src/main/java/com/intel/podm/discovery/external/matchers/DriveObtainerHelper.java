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
import com.intel.podm.client.api.ExternalServiceApiReaderException;
import com.intel.podm.client.api.reader.ResourceSupplier;
import com.intel.podm.client.api.resources.redfish.ChassisResource;
import com.intel.podm.client.api.resources.redfish.ComputerSystemResource;
import com.intel.podm.client.api.resources.redfish.DriveResource;

import javax.enterprise.context.Dependent;
import java.net.URI;
import java.util.Iterator;
import java.util.Objects;
import java.util.Optional;
import java.util.function.Function;
import java.util.function.Predicate;

@Dependent
public class DriveObtainerHelper implements EntityObtainerHelper<DriveResource> {

    @Override
    public ComputerSystemResource findComputerSystemResourceFor(DriveResource resource) throws ExternalServiceApiReaderException {
        ChassisResource chassisResource = (ChassisResource) resource.getParentChassis().get();
        Iterator<ResourceSupplier> iterator = chassisResource.getComputerSystems().iterator();

        if (!iterator.hasNext()) {
            throw new ExternalServiceApiReaderException(
                    "We assume that there should be at least one link between Chassis and ComputerSystem for LUI service.",
                    resource.getUri()
            );
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
                .filter(byTheSameLastPathSegment(resource))
                .findFirst();
    }

    private Predicate<? super Drive> byTheSameLastPathSegment(DriveResource resource) {
        return drive -> Objects.equals(lastPathSegment(drive.getSourceUri()), lastPathSegment(resource.getUri()));
    }

    private String lastPathSegment(URI uri) {
        String path = uri.getPath();
        return path.substring(path.lastIndexOf("/") + 1);
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
