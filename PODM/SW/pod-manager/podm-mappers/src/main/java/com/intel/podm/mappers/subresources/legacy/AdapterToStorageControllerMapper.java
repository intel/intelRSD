/*
 * Copyright (c) 2016-2018 Intel Corporation
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

package com.intel.podm.mappers.subresources.legacy;

import com.intel.podm.business.entities.redfish.StorageController;
import com.intel.podm.client.resources.legacy.AdapterResource;
import com.intel.podm.mappers.subresources.SubresourceMapper;
import com.intel.podm.mappers.subresources.strategies.EntityCleanAndCreateStrategy;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.util.Objects;

@Dependent
public class AdapterToStorageControllerMapper extends SubresourceMapper<AdapterResource, StorageController> {

    @Inject
    public AdapterToStorageControllerMapper(EntityCleanAndCreateStrategy<StorageController> entityCleanAndCreateStrategy) {
        super(StorageController.class, entityCleanAndCreateStrategy);
    }

    @Override
    public void map(AdapterResource adapterResource, StorageController targetStorageController) {
        targetStorageController.setManufacturer(adapterResource.getManufacturer());
        targetStorageController.setModel(adapterResource.getModel());
        targetStorageController.setStatus(adapterResource.getStatus());
        targetStorageController.getSupportedDeviceProtocols().add(adapterResource.getInterface());
    }

    @Override
    public boolean equals(AdapterResource adapterResource, StorageController targetStorageController) {
        return Objects.equals(targetStorageController.getManufacturer(), adapterResource.getManufacturer())
                && Objects.equals(targetStorageController.getModel(), adapterResource.getModel());
    }
}
