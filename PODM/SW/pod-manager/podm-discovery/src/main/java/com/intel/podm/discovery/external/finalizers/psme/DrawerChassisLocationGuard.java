/*
 * Copyright (c) 2015 Intel Corporation
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

package com.intel.podm.discovery.external.finalizers.psme;

import com.intel.podm.business.entities.dao.ChassisDao;
import com.intel.podm.business.entities.redfish.Chassis;
import com.intel.podm.discovery.external.finders.RackChassisFinder;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.util.List;
import java.util.Objects;

import static com.google.common.base.Preconditions.checkArgument;
import static com.intel.podm.common.types.ChassisType.DRAWER;
import static com.intel.podm.common.types.ChassisType.RACK;
import static java.util.Objects.isNull;
import static java.util.stream.Collectors.toList;

@Dependent
class DrawerChassisLocationGuard {
    @Inject
    ChassisDao chassisDao;

    @Inject
    private RackChassisFinder rackChassisFinder;

    /**
     * This method unlinks drawer from racks which
     * have different location value. If rack is useless
     * (not managed by RMM and with empty children collection)
     * it is being removed.
     */
    public void assureSingleRackParent(Chassis drawerChassis) {
        checkArgument(drawerChassis.is(DRAWER), "This method requires chassis of type Drawer as an argument.");

        unlinkFromNonParentRacks(drawerChassis);
        deleteUselessRacks();
    }

    private void unlinkFromNonParentRacks(Chassis drawerChassis) {
        chassisDao.getAllByChassisType(RACK).stream()
                .filter(rackChassis -> rackChassis.getContainedChassis().contains(drawerChassis))
                .filter(rackChassis -> !Objects.equals(rackChassis.getLocationId(), drawerChassis.getLocationParentId()))
                .forEach(rackChassis -> rackChassis.unlinkContainedChassis(drawerChassis));
    }

    private void deleteUselessRacks() {
        List<Chassis> racksToRemove = chassisDao.getAllByChassisType(RACK).stream()
                .filter(rack -> rack.getContainedChassis().isEmpty())
                .filter(rack -> isNull(rack.getService()))
                .collect(toList());

        racksToRemove.forEach(chassisDao::remove);
    }
}
