/*
 * Copyright (c) 2015-2017 Intel Corporation
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

package com.intel.podm.discovery.external.finders;

import com.intel.podm.business.entities.dao.ChassisDao;
import com.intel.podm.business.entities.redfish.Chassis;
import com.intel.podm.common.synchronization.TaskCanceledException;
import com.intel.podm.common.types.ChassisType;
import com.intel.podm.common.types.Health;
import com.intel.podm.common.types.State;
import com.intel.podm.common.types.Status;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.Base64;
import java.util.List;

import static com.google.common.base.Strings.isNullOrEmpty;
import static com.intel.podm.common.types.ChassisType.RACK;
import static com.intel.podm.common.types.Id.id;
import static com.intel.podm.common.utils.Contracts.checkArgument;
import static com.intel.podm.common.utils.IterableHelper.single;
import static javax.transaction.Transactional.TxType.MANDATORY;

@Transactional(MANDATORY)
@Dependent
public class RackChassisFinder {
    private static final String DEFAULT_RACK_NAME = "Rack created by PODM";

    @Inject
    private ChassisDao chassisDao;

    /**
     * Finds or creates rack with given location id
     */
    public Chassis findAnyOrCreate(String locationId) {
        return findByLocation(locationId).stream()
            .findAny()
            .orElseGet(() -> createRackChassis(locationId));
    }

    public List<Chassis> findByLocation(String locationId) {
        return chassisDao.getChassis(RACK, locationId);
    }

    /**
     * Creates a new Rack instance using provided locationId. Pod should be available
     * for this rack to be properly attached to assets structure.
     */
    public Chassis createRackChassis(String locationId) {
        checkArgument(!isNullOrEmpty(locationId),
            () -> new TaskCanceledException("Chassis does not have required property : 'parentId'. Cannot create rack structure"));

        Chassis rackChassis = chassisDao.create();
        Chassis podChassis = single(chassisDao.getAllByChassisType(ChassisType.POD));

        podChassis.addContainedChassis(rackChassis);
        rackChassis.setLocationParentId(podChassis.getLocationId());
        rackChassis.setLocationId(locationId);
        rackChassis.setId(id(encodeBase64(locationId)));
        rackChassis.setChassisType(RACK);
        rackChassis.setName(DEFAULT_RACK_NAME);
        rackChassis.setStatus(new Status(State.ENABLED, Health.OK, null));

        return rackChassis;
    }

    private String encodeBase64(String locationId) {
        return Base64.getEncoder().withoutPadding().encodeToString(locationId.getBytes());
    }
}
