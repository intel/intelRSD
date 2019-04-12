/*
 * Copyright (c) 2016-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.mappers.subresources;

import com.intel.rsd.nodecomposer.externalservices.resources.redfish.SimpleStorageDeviceResource;
import com.intel.rsd.nodecomposer.mappers.subresources.strategies.EntityCleanAndCreateStrategy;
import com.intel.rsd.nodecomposer.persistence.redfish.SimpleStorageDevice;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import java.util.Objects;

import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_PROTOTYPE;

@Component
@Scope(SCOPE_PROTOTYPE)
public class SimpleStorageDeviceMapper extends SubresourceMapper<SimpleStorageDeviceResource, SimpleStorageDevice> {
    @Autowired
    public SimpleStorageDeviceMapper(EntityCleanAndCreateStrategy<SimpleStorageDevice> entityCleanAndCreateStrategy) {
        super(SimpleStorageDevice.class, entityCleanAndCreateStrategy);
    }

    @Override
    public void map(SimpleStorageDeviceResource sourceDevice, SimpleStorageDevice targetDevice) {
        targetDevice.setStatus(sourceDevice.getStatus());
        targetDevice.setCapacityBytes(sourceDevice.getCapacityBytes());
    }

    @Override
    public boolean equals(SimpleStorageDeviceResource sourceDevice, SimpleStorageDevice targetDevice) {
        return Objects.equals(sourceDevice.getStatus(), targetDevice.getStatus())
            && Objects.equals(sourceDevice.getCapacityBytes(), targetDevice.getCapacityBytes());
    }
}
