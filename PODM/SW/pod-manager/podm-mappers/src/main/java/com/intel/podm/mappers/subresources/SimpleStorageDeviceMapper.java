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

package com.intel.podm.mappers.subresources;

import com.intel.podm.business.entities.redfish.SimpleStorageDevice;
import com.intel.podm.client.resources.redfish.SimpleStorageDeviceResource;
import com.intel.podm.mappers.subresources.strategies.EntityCleanAndCreateStrategy;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.util.Objects;

@Dependent
public class SimpleStorageDeviceMapper extends SubresourceMapper<SimpleStorageDeviceResource, SimpleStorageDevice> {
    @Inject
    public SimpleStorageDeviceMapper(EntityCleanAndCreateStrategy<SimpleStorageDevice> entityCleanAndCreateStrategy) {
        super(SimpleStorageDevice.class, entityCleanAndCreateStrategy);
    }

    @Override
    public void map(SimpleStorageDeviceResource sourceDevice, SimpleStorageDevice targetDevice) {
        targetDevice.setName(sourceDevice.getName());
        targetDevice.setManufacturer(sourceDevice.getManufacturer());
        targetDevice.setModel(sourceDevice.getModel());
        targetDevice.setStatus(sourceDevice.getStatus());
        targetDevice.setCapacityBytes(sourceDevice.getCapacityBytes());
        targetDevice.setOem(sourceDevice.getOem());
    }

    @Override
    public boolean equals(SimpleStorageDeviceResource sourceDevice, SimpleStorageDevice targetDevice) {
        return nameStatusAndManufacturerEquals(sourceDevice, targetDevice)
            && Objects.equals(sourceDevice.getModel(), targetDevice.getModel())
            && Objects.equals(sourceDevice.getCapacityBytes(), targetDevice.getCapacityBytes())
            && Objects.equals(sourceDevice.getOem(), targetDevice.getOem());
    }

    private boolean nameStatusAndManufacturerEquals(SimpleStorageDeviceResource sourceDevice, SimpleStorageDevice targetDevice) {
        return Objects.equals(sourceDevice.getName(), targetDevice.getName())
            && Objects.equals(sourceDevice.getStatus(), targetDevice.getStatus())
            && Objects.equals(sourceDevice.getManufacturer(), targetDevice.getManufacturer());
    }
}
