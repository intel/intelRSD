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

package com.intel.podm.discovery.external.finalizers.psme;

import com.intel.podm.business.entities.dao.ChassisDao;
import com.intel.podm.business.entities.redfish.Chassis;
import com.intel.podm.discovery.external.finders.RackChassisFinder;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.util.Collection;
import java.util.Objects;

import static com.google.common.base.Preconditions.checkArgument;
import static com.intel.podm.common.types.ChassisType.DRAWER;
import static com.intel.podm.common.types.ChassisType.POD;
import static com.intel.podm.common.utils.IterableHelper.single;

@Dependent
public class DrawerChassisLinker {
    @Inject
    private ChassisDao chassisDao;

    @Inject
    private RackChassisFinder rackFinder;

    public void linkToDomainModel(Chassis drawerChassis) {
        checkArgument(drawerChassis.is(DRAWER), "Only chassis of type drawer is accepted as an argument.");

        if (drawerShouldBeLinkedToPod(drawerChassis.getLocationParentId())) {
            linkToPod(drawerChassis);
        } else {
            linkToRack(drawerChassis);
        }

    }

    private void linkToRack(Chassis drawerChassis) {
        if (drawerIsAlreadyLinkedToRack(drawerChassis)) {
            return;
        }

        if (drawerIsAlreadyLinkedToPod(drawerChassis)) {
            Chassis pod = single(chassisDao.getAllByChassisType(POD));
            pod.unlinkContainedChassis(drawerChassis);
        }

        Chassis rackChassis = rackFinder.findAnyOrCreate(drawerChassis.getLocationParentId());
        rackChassis.contain(drawerChassis);
    }

    private void linkToPod(Chassis drawerChassis) {
        Chassis pod = single(chassisDao.getAllByChassisType(POD));
        pod.contain(drawerChassis);
        drawerChassis.setLocationParentId(pod.getLocationId());
    }

    private boolean drawerShouldBeLinkedToPod(String drawerParentLocationId) {
        return drawerParentLocationId == null;
    }

    private boolean drawerIsAlreadyLinkedToPod(Chassis drawerChassis) {
        Chassis pod = single(chassisDao.getAllByChassisType(POD));

        return  pod.getContainedChassis().stream()
                .filter(chassis -> chassis.is(DRAWER))
                .filter(chassis -> Objects.equals(chassis.getId(), drawerChassis.getId()))
                .findFirst()
                .isPresent();
    }

    private boolean drawerIsAlreadyLinkedToRack(Chassis drawerChassis) {
        return rackFinder.findByLocation(drawerChassis.getLocationParentId()).stream()
                .map(Chassis::getContainedChassis)
                .flatMap(Collection::stream)
                .filter(chassis -> Objects.equals(chassis.getId(), drawerChassis.getId()))
                .findFirst()
                .isPresent();
    }
}
