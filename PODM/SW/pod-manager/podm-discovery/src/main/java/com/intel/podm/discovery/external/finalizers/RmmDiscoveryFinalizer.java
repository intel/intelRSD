/*
 * Copyright (c) 2015-2018 Intel Corporation
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

package com.intel.podm.discovery.external.finalizers;

import com.intel.podm.business.entities.dao.ChassisDao;
import com.intel.podm.business.entities.dao.GenericDao;
import com.intel.podm.business.entities.redfish.Chassis;
import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.business.entities.redfish.base.DiscoverableEntity;
import com.intel.podm.discovery.external.finders.RackChassisFinder;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.util.HashSet;
import java.util.List;
import java.util.Objects;
import java.util.Set;

import static com.intel.podm.common.types.ChassisType.DRAWER;
import static com.intel.podm.common.types.ChassisType.POD;
import static com.intel.podm.common.types.ChassisType.RACK;
import static com.intel.podm.common.types.ServiceType.RMM;
import static com.intel.podm.common.utils.Collections.filterByType;
import static com.intel.podm.common.utils.IterableHelper.single;
import static java.util.Objects.isNull;
import static java.util.stream.Collectors.toList;
import static java.util.stream.Collectors.toSet;

@Dependent
public class RmmDiscoveryFinalizer extends ServiceTypeSpecializedDiscoveryFinalizer {
    @Inject
    private GenericDao genericDao;

    @Inject
    private ChassisDao chassisDao;

    @Inject
    private RackChassisFinder rackChassisFinder;

    public RmmDiscoveryFinalizer() {
        super(RMM);
    }

    @Override
    public void finalize(Set<DiscoverableEntity> discoveredEntities, ExternalService service) {
        for (Chassis rack : getRackChassisFromDiscoveredObjects(discoveredEntities)) {
            linkDiscoveredRackToPod(rack);
            removeUselessRacks(rack);
            assureConsistentTopology(rack);
        }
    }

    private void linkDiscoveredRackToPod(Chassis rackChassis) {
        Chassis podChassis = single(chassisDao.getAllByChassisType(POD));
        podChassis.addContainedChassis(rackChassis);
        rackChassis.setLocationParentId(podChassis.getLocationId());
    }

    private void removeUselessRacks(Chassis discoveredRack) {
        List<Chassis> racksWithEqualLocationWithoutRmm = rackChassisFinder.findByLocation(discoveredRack.getLocationId())
            .stream()
            .filter(chassis -> !Objects.equals(chassis.getId(), discoveredRack.getId()))
            .filter(chassis -> isNull(chassis.getService()))
            .collect(toList());

        moveContainedChassisToNewlyDiscoveredRack(discoveredRack, racksWithEqualLocationWithoutRmm);
    }

    private void moveContainedChassisToNewlyDiscoveredRack(Chassis discoveredRack, List<Chassis> racksWithEqualLocationWithoutRmm) {
        for (Chassis rack : racksWithEqualLocationWithoutRmm) {
            // copy of contained chassis needed because of ConcurrentModificationException
            Set<Chassis> containedChassis = new HashSet<>(rack.getContainedChassis());
            containedChassis.forEach(discoveredRack::addContainedChassis);

            genericDao.remove(rack);
        }
    }

    private void assureConsistentTopology(Chassis rackChassis) {
        Set<Chassis> drawers = rackChassis.getContainedChassis().stream()
            .filter(chassis -> chassis.is(DRAWER))
            .filter(drawer -> drawer.getLocationParentId() != null)
            .filter(drawer -> !Objects.equals(drawer.getLocationParentId(), rackChassis.getLocationId()))
            .collect(toSet());

        drawers.forEach(drawer -> {
            rackChassis.unlinkContainedChassis(drawer);
            Chassis newRackChassis = rackChassisFinder.findAnyOrCreate(drawer.getLocationParentId());
            newRackChassis.addContainedChassis(drawer);
        });
    }

    private List<Chassis> getRackChassisFromDiscoveredObjects(Set<DiscoverableEntity> discoveredEntities) {
        return filterByType(discoveredEntities, Chassis.class).stream()
            .filter(chassis -> chassis.is(RACK))
            .collect(toList());
    }
}
