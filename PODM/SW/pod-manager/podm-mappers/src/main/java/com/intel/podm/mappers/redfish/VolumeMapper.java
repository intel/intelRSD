/*
 * Copyright (c) 2017-2018 Intel Corporation
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

import com.intel.podm.business.entities.redfish.Volume;
import com.intel.podm.business.entities.redfish.embeddables.Capacity;
import com.intel.podm.client.resources.redfish.VolumeResource;
import com.intel.podm.mappers.EntityMapper;
import com.intel.podm.mappers.subresources.IdentifierMapper;
import com.intel.podm.mappers.subresources.SimpleTypeMapper;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;

import static java.lang.Boolean.TRUE;

@Dependent
public class VolumeMapper extends EntityMapper<VolumeResource, Volume> {
    @Inject
    private CapacityMapper capacityMapper;

    @Inject
    private IdentifierMapper identifierMapper;

    @Inject
    private OperationMapper operationsMapper;

    @Inject
    private SimpleTypeMapper simpleTypeMapper;

    public VolumeMapper() {
        super(VolumeResource.class, Volume.class);
    }

    @Override
    protected void performNotAutomatedMapping(VolumeResource source, Volume target) {
        super.performNotAutomatedMapping(this.source, this.target);

        identifierMapper.map(source.getIdentifiers(), target.getIdentifiers(), target::addIdentifier);
        mapCapacity(source);
        operationsMapper.map(source.getOperations(), target.getOperations(), target::addOperations);
        simpleTypeMapper.map(source.getAccessCapabilities(), target.getAccessCapabilities(), target::addAccessCapability);
        if (source.getActions().getInitialize().getTarget().isAssigned()) {
            target.setInitializeActionSupported(TRUE);
            source.getActions().getInitialize().getAllowableValues().ifAssigned(allowableInitializeTypes ->
                simpleTypeMapper.map(allowableInitializeTypes, target.getAllowableInitializeTypes(), target::addAllowableInitializeType)
            );
        }

        if (target.getEraseOnDetach() == null) {
            target.setEraseOnDetach(true);
        }
    }

    private void mapCapacity(VolumeResource source) {
        if (source.getCapacity() != null) {
            Capacity capacity = new Capacity();
            capacityMapper.map(source.getCapacity(), capacity);
        }
    }
}
