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

import com.intel.podm.business.entities.redfish.Chassis;
import com.intel.podm.business.entities.redfish.embeddables.RackChassisAttributes;
import com.intel.podm.client.resources.redfish.ChassisResource;
import com.intel.podm.client.resources.redfish.RackScaleRackChassisOem;
import com.intel.podm.common.types.Ref;
import com.intel.podm.common.types.Status;
import com.intel.podm.mappers.EntityMapper;

import javax.enterprise.context.Dependent;

import static com.intel.podm.common.types.Health.OK;
import static com.intel.podm.common.types.State.ENABLED;

@Dependent
public class ChassisMapper extends EntityMapper<ChassisResource, Chassis> {
    public ChassisMapper() {
        super(ChassisResource.class, Chassis.class);
        registerProvider(RackChassisAttributes.class, target -> provideRackChassisAttributes());
        ignoredProperties("name");
    }

    @Override
    protected void performNotAutomatedMapping(ChassisResource sourceChassis, Chassis targetChassis) {
        super.performNotAutomatedMapping(source, target);
        if (sourceChassis.getName() != null) {
            targetChassis.setName(sourceChassis.getName());
        }

        mapRackChassis(sourceChassis, targetChassis);
    }

    private void mapRackChassis(ChassisResource sourceChassis, Chassis targetChassis) {
        RackScaleRackChassisOem rackScaleRackChassisOem = sourceChassis.getRackChassisAttributes();
        if (rackScaleRackChassisOem != null) {
            if (sourceChassis.getStatus() == null) {
                targetChassis.setStatus(new Status(ENABLED, OK, null));
            }

            final Ref<String> refLocationId = rackScaleRackChassisOem.getLocation().getId();
            String locationId = null;
            if (refLocationId.isAssigned()) {
                locationId = refLocationId.get();
            } else if (rackScaleRackChassisOem.getRackPuid() != null) {
                locationId = rackScaleRackChassisOem.getRackPuid().toString();
            }
            targetChassis.setLocationId(locationId);
        }
    }

    private RackChassisAttributes provideRackChassisAttributes() {
        RackChassisAttributes attributes = target.getRackChassisAttributes();
        return attributes == null ? new RackChassisAttributes() : attributes;
    }
}
