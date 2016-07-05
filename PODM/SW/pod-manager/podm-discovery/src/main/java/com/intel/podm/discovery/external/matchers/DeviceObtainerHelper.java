/*
 * Copyright (c) 2015 Intel Corporation
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

import com.intel.podm.business.entities.base.DomainObject;
import com.intel.podm.business.entities.redfish.Adapter;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.Device;
import com.intel.podm.client.api.ExternalServiceApiReaderException;
import com.intel.podm.client.api.resources.redfish.AdapterResource;
import com.intel.podm.client.api.resources.redfish.ComputerSystemResource;
import com.intel.podm.client.api.resources.redfish.DeviceResource;

import javax.enterprise.context.Dependent;
import java.util.Collection;
import java.util.Objects;
import java.util.Optional;
import java.util.function.Predicate;

@Dependent
public class DeviceObtainerHelper implements DomainObjectObtainerHelper<DeviceResource> {
    @Override
    public ComputerSystemResource findComputerSystemResourceFor(DeviceResource resource) throws ExternalServiceApiReaderException {
        return (ComputerSystemResource) ((AdapterResource) resource.getAdapter().get()).getComputerSystem().get();
    }

    @Override
    public Optional<Device> findDomainObjectFor(ComputerSystem computerSystem, DeviceResource resource) {
        Predicate<Device> devicePredicate = device -> Objects.equals(resource.getBusInfo(), device.getBusInfo());

        return computerSystem.getAdapters().stream()
                                            .map(Adapter::getDevices)
                                            .flatMap(Collection::stream)
                                            .filter(devicePredicate)
                                            .findFirst();
    }

    @Override
    public Class<? extends DomainObject> getDomainObjectClass() {
        return Device.class;
    }

    @Override
    public Class<DeviceResource> getResourceClass() {
        return DeviceResource.class;
    }
}
