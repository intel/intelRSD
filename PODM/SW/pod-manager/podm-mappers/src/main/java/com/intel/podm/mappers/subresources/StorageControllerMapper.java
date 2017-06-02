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

import com.intel.podm.business.entities.redfish.StorageController;
import com.intel.podm.client.api.resources.redfish.StorageResource.StorageControllerResource;
import com.intel.podm.mappers.subresources.strategies.EntityCleanAndCreateStrategy;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.util.Objects;

@Dependent
public class StorageControllerMapper extends SubresourceMapper<StorageControllerResource, StorageController> {
    @Inject
    private IdentifierMapper identifierMapper;

    @Inject
    private SimpleTypeMapper simpleTypeMapper;

    @Inject
    public StorageControllerMapper(EntityCleanAndCreateStrategy<StorageController> entityCleanAndCreateStrategy) {
        super(StorageController.class, entityCleanAndCreateStrategy);
    }

    @Override
    public void map(StorageControllerResource source, StorageController target) {
        target.setSku(source.getSku());
        target.setModel(source.getModel());
        target.setStatus(source.getStatus());
        target.setAssetTag(source.getAssetTag());
        target.setMemberId(source.getMemberId());
        target.setSpeedGbps(source.getSpeedGbps());
        target.setPartNumber(source.getPartNumber());
        target.setManufacturer(source.getManufacturer());
        target.setSerialNumber(source.getSerialNumber());
        target.setFirmwareVersion(source.getFirmwareVersion());
        simpleTypeMapper.map(source.getSupportedDeviceProtocols(), target.getSupportedDeviceProtocols(), target::addSupportedDeviceProtocol);
        simpleTypeMapper.map(source.getSupportedControllerProtocols(), target.getSupportedControllerProtocols(), target::addSupportedControllerProtocol);
        identifierMapper.map(source.getIdentifiers(), target.getIdentifiers(), target::addIdentifier);
    }

    @Override
    protected boolean equals(StorageControllerResource source, StorageController target) {
        return Objects.equals(target.getManufacturer(), source.getManufacturer())
            && Objects.equals(target.getModel(), source.getModel())
            && Objects.equals(target.getSku(), source.getSku())
            && checkSerialAndPartNumber(source, target);
    }

    private boolean checkSerialAndPartNumber(StorageControllerResource source, StorageController target) {
        return Objects.equals(target.getSerialNumber(), source.getSerialNumber())
            && Objects.equals(target.getPartNumber(), source.getPartNumber());
    }
}
