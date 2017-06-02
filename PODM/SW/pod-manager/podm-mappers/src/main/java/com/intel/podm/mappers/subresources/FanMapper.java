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

import com.intel.podm.business.entities.redfish.Fan;
import com.intel.podm.business.entities.redfish.embeddables.RackLocation;
import com.intel.podm.client.api.resources.redfish.FanResource;
import com.intel.podm.mappers.subresources.strategies.EntityCleanAndCreateStrategy;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.util.Objects;

@Dependent
public class FanMapper extends SubresourceMapper<FanResource, Fan> {
    @Inject
    private RackLocationMapper rackLocationMapper;

    @Inject
    public FanMapper(EntityCleanAndCreateStrategy<Fan> entityCleanAndCreateStrategy) {
        super(Fan.class, entityCleanAndCreateStrategy);
    }

    @Override
    public void map(FanResource sourceFan, Fan targetFan) {
        targetFan.setStatus(sourceFan.getStatus());
        targetFan.setName(sourceFan.getFanName());
        targetFan.setReadingRpm(sourceFan.getReadingRpm());

        if (sourceFan.getRackLocation() != null) {
            RackLocation rackLocation = new RackLocation();
            rackLocationMapper.map(sourceFan.getRackLocation(), rackLocation);
            targetFan.setRackLocation(rackLocation);
        }
    }

    @Override
    public boolean equals(FanResource sourceFan, Fan targetFan) {
        return Objects.equals(sourceFan.getStatus(), targetFan.getStatus())
                && Objects.equals(sourceFan.getFanName(), targetFan.getName())
                && Objects.equals(sourceFan.getReadingRpm(), targetFan.getReadingRpm())
                && rackLocationMapper.equals(sourceFan.getRackLocation(), targetFan.getRackLocation());
    }
}
