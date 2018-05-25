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

package com.intel.podm.mappers.redfish;

import com.intel.podm.business.entities.redfish.embeddables.PciId;
import com.intel.podm.client.resources.redfish.PciIdResource;
import com.intel.podm.mappers.subresources.SubresourceMapper;
import com.intel.podm.mappers.subresources.strategies.EmbeddableCleanAndCreateStrategy;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.util.Objects;

@Dependent
public class PciIdMapper extends SubresourceMapper<PciIdResource, PciId> {

    @Inject
    public PciIdMapper(EmbeddableCleanAndCreateStrategy<PciId> embeddableCleanAndCreateStrategy) {
        super(PciId.class, embeddableCleanAndCreateStrategy);
    }

    @Override
    public void map(PciIdResource sourcePciId, PciId targetPciId) {
        targetPciId.setDeviceId(sourcePciId.getDeviceId());
        targetPciId.setSubsystemId(sourcePciId.getSubsystemId());
        targetPciId.setSubsystemVendorId(sourcePciId.getSubsystemVendorId());
        targetPciId.setVendorId(sourcePciId.getVendorId());
    }

    @Override
    public boolean equals(PciIdResource sourcePciId, PciId targetPciId) {
        return Objects.equals(sourcePciId.getDeviceId(), targetPciId.getDeviceId())
                && Objects.equals(sourcePciId.getSubsystemId(), targetPciId.getSubsystemId())
                && Objects.equals(sourcePciId.getSubsystemVendorId(), targetPciId.getSubsystemVendorId())
                && Objects.equals(sourcePciId.getVendorId(), targetPciId.getVendorId());
    }
}
