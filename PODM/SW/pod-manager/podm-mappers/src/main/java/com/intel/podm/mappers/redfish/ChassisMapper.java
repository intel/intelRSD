/*
 * Copyright (c) 2016 Intel Corporation
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

import com.intel.podm.business.entities.dao.GenericDao;
import com.intel.podm.business.entities.redfish.Chassis;
import com.intel.podm.business.entities.redfish.properties.RackChassisAttributes;
import com.intel.podm.client.api.resources.redfish.ChassisResource;
import com.intel.podm.client.api.resources.redfish.ChassisResource.RackScaleRackChassisOem;
import com.intel.podm.client.api.resources.redfish.OemVendor;
import com.intel.podm.mappers.DomainObjectMapper;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;

import static java.util.Objects.nonNull;

@Dependent
public class ChassisMapper extends DomainObjectMapper<ChassisResource, Chassis> {
    @Inject
    private GenericDao genericDao;

    public ChassisMapper() {
        super(ChassisResource.class, Chassis.class);
        registerProvider(RackChassisAttributes.class, this::addRackChassisAttributes);
    }

    @Override
    protected void performNotAutomatedMapping(ChassisResource source, Chassis target) {
        super.performNotAutomatedMapping(source, target);
        RackScaleRackChassisOem attributes = source.getRackChassisAttributes();
        if (nonNull(attributes) && nonNull(attributes.getRackPuid())) {
            target.setLocationId(attributes.getRackPuid().toString());
        }
    }

    private RackChassisAttributes addRackChassisAttributes(OemVendor o) {
        RackChassisAttributes rackChassisAttributes = target.getRackChassisAttributes();
        if (rackChassisAttributes != null) {
            genericDao.remove(rackChassisAttributes);
        }

        return genericDao.create(RackChassisAttributes.class);
    }
}
