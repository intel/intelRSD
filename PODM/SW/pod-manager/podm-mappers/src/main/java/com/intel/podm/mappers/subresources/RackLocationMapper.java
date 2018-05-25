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

package com.intel.podm.mappers.subresources;

import com.intel.podm.business.entities.redfish.embeddables.RackLocation;
import com.intel.podm.client.resources.redfish.RackLocationObject;
import com.intel.podm.mappers.subresources.strategies.EmbeddableCleanAndCreateStrategy;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.util.Objects;

@Dependent
public class RackLocationMapper extends SubresourceMapper<RackLocationObject, RackLocation> {
    @Inject
    public RackLocationMapper(EmbeddableCleanAndCreateStrategy<RackLocation> embeddableCleanAndCreateStrategy) {
        super(RackLocation.class, embeddableCleanAndCreateStrategy);
    }

    @Override
    public void map(RackLocationObject sourceRackLocation, RackLocation targetRackLocation) {
        targetRackLocation.setRackUnits(sourceRackLocation.getRackUnits());
        targetRackLocation.setUHeight(sourceRackLocation.getUHeight());
        targetRackLocation.setULocation(sourceRackLocation.getULocation());
        targetRackLocation.setXLocation(sourceRackLocation.getXLocation());
    }

    @Override
    public boolean equals(RackLocationObject sourceRackLocation, RackLocation targetRackLocation) {
        return Objects.equals(sourceRackLocation.getRackUnits(), targetRackLocation.getRackUnits())
                && Objects.equals(sourceRackLocation.getUHeight(), targetRackLocation.getUHeight())
                && Objects.equals(sourceRackLocation.getULocation(), targetRackLocation.getULocation())
                && Objects.equals(sourceRackLocation.getXLocation(), targetRackLocation.getXLocation());
    }
}
