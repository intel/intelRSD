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
import com.intel.podm.client.resources.redfish.StorageControllerResource;
import com.intel.podm.mappers.EntityMapper;
import com.intel.podm.mappers.subresources.strategies.EmbeddableCleanAndCreateStrategy;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;

@Dependent
public class StorageControllerMapper extends EntityMapper<StorageControllerResource, StorageController> {
    @Inject
    private IdentifierMapper identifierMapper;

    @Inject
    private SimpleTypeMapper simpleTypeMapper;

    private UnknownOemMapper unknownOemMapper = new UnknownOemMapper(new EmbeddableCleanAndCreateStrategy<>());

    @Inject
    public StorageControllerMapper() {
        super(StorageControllerResource.class, StorageController.class);
    }

    @Override
    public void map(StorageControllerResource source, StorageController target) {
        target.setName(source.getName());
        target.setDescription(source.getDescription());
        target.setMemberId(source.getMemberId());
        target.setStatus(source.getStatus());
        target.setManufacturer(source.getManufacturer());
        target.setModel(source.getModel());
        target.setSku(source.getSku());
        target.setSerialNumber(source.getSerialNumber());
        target.setPartNumber(source.getPartNumber());
        target.setAssetTag(source.getAssetTag());
        target.setSpeedGbps(source.getSpeedGbps());
        target.setFirmwareVersion(source.getFirmwareVersion());
        simpleTypeMapper.map(source.getSupportedDeviceProtocols(), target.getSupportedDeviceProtocols(), target::addSupportedDeviceProtocol);
        simpleTypeMapper.map(source.getSupportedControllerProtocols(), target.getSupportedControllerProtocols(), target::addSupportedControllerProtocol);
        identifierMapper.map(source.getIdentifiers(), target.getIdentifiers(), target::addIdentifier);
        unknownOemMapper.map(source.getUnknownOems(), target.getUnknownOems(), target::addUnknownOem);
    }
}
