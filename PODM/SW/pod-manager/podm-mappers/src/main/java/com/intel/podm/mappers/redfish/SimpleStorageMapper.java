/*
 * Copyright (c) 2016 Intel Corporation
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

package com.intel.podm.mappers.redfish;

import com.intel.podm.business.entities.dao.GenericDao;
import com.intel.podm.business.entities.redfish.SimpleStorage;
import com.intel.podm.business.entities.redfish.properties.SimpleStorageDevice;
import com.intel.podm.client.api.resources.redfish.SimpleStorageDeviceResource;
import com.intel.podm.client.api.resources.redfish.SimpleStorageResource;
import com.intel.podm.mappers.DomainObjectMapper;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.util.Collection;
import java.util.List;
import java.util.function.Supplier;

@Dependent
public class SimpleStorageMapper extends DomainObjectMapper<SimpleStorageResource, SimpleStorage> {
    @Inject
    private GenericDao genericDao;

    public SimpleStorageMapper() {
        super(SimpleStorageResource.class, SimpleStorage.class);
    }

    @Override
    protected void performNotAutomatedMapping(SimpleStorageResource source, SimpleStorage target) {
        clearDevices(target.getDevices());
        addDevices(source.getDevices(), target::addDevice);
    }

    private void clearDevices(Collection<SimpleStorageDevice> devices) {
        devices.forEach(genericDao::remove);
    }

    private void addDevices(List<SimpleStorageDeviceResource> devices, Supplier<SimpleStorageDevice> deviceSupplier) {
        if (devices == null) {
            return;
        }

        devices.forEach(sourceDevice -> {
            SimpleStorageDevice targetDevice = deviceSupplier.get();
            targetDevice.setName(sourceDevice.getName());
            targetDevice.setManufacturer(sourceDevice.getManufacturer());
            targetDevice.setModel(sourceDevice.getModel());
            targetDevice.setStatus(sourceDevice.getStatus());
            targetDevice.setCapacityBytes(sourceDevice.getCapacityBytes());
        });
    }
}
