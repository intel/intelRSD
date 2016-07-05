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

package com.intel.podm.discovery.external.finalizers.rmm;

import com.intel.podm.business.entities.base.DomainObject;
import com.intel.podm.business.entities.dao.ChassisDao;
import com.intel.podm.business.entities.dao.GenericDao;
import com.intel.podm.business.entities.redfish.Chassis;
import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.business.entities.redfish.properties.RackChassisAttributes;
import com.intel.podm.discovery.external.finalizers.DiscoveryFinalizer;
import com.intel.podm.discovery.external.finders.RackChassisFinder;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.util.Collection;
import java.util.List;
import java.util.Objects;
import java.util.Set;

import static com.google.common.base.Preconditions.checkNotNull;
import static com.intel.podm.common.types.ChassisType.DRAWER;
import static com.intel.podm.common.types.ChassisType.POD;
import static com.intel.podm.common.types.ChassisType.RACK;
import static com.intel.podm.common.types.ServiceType.RMM;
import static com.intel.podm.common.utils.Collections.filterByType;
import static com.intel.podm.common.utils.IterableHelper.single;
import static java.util.Objects.isNull;
import static java.util.Optional.ofNullable;
import static java.util.stream.Collectors.toList;

@Dependent
public class RmmDiscoveryFinalizer extends DiscoveryFinalizer {

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
    public void finalize(Set<DomainObject> discoveredDomainObjects, ExternalService service) {
        Chassis rackChassis = getRackChassisFromDiscoveredObjects(discoveredDomainObjects);

        linkDiscoveredRackToPod(rackChassis);
        assignLocationId(rackChassis);
        removeUselessRacks(rackChassis);
        assureConsistentTopology(rackChassis);
    }

    private void assureConsistentTopology(Chassis rackChassis) {
        rackChassis.getContainedChassis().stream()
                .filter(chassis -> chassis.is(DRAWER))
                .filter(drawer -> !Objects.equals(drawer.getLocationParentId(), rackChassis.getLocationId()))
                .forEach(drawer -> {
                    rackChassis.unlinkContainedChassis(drawer);
                    Chassis newRackChassis = rackChassisFinder.findAnyOrCreate(drawer.getLocationParentId());
                    newRackChassis.contain(drawer);
                });
    }

    private void assignLocationId(Chassis rackChassis) {
        checkNotNull(rackChassis, "Non null argument required here.");

        ofNullable(rackChassis.getRackChassisAttributes())
                .map(RackChassisAttributes::getRackPuid)
                .map(Object::toString)
                .ifPresent(rackChassis::setLocationId);
    }

    private void removeUselessRacks(Chassis discoveredRack) {
        List<Chassis> racksWithEqualLocationWithoutRmm = rackChassisFinder.findByLocation(discoveredRack.getLocationId())
                .stream()
                .filter(chassis -> !Objects.equals(chassis.getId(), discoveredRack.getId()))
                .filter(chassis -> isNull(chassis.getService()))
                .collect(toList());

        for (Chassis rack : racksWithEqualLocationWithoutRmm) {
            // move drawers to newly discovered rack
            rack.getContainedChassis().stream()
                    .filter(chassis -> chassis.is(DRAWER))
                    .forEach(discoveredRack::contain);

            genericDao.remove(rack);
        }
    }

    private void linkDiscoveredRackToPod(Chassis rackChassis) {
        Chassis podChassis = single(chassisDao.getAllByChassisType(POD));
        podChassis.contain(rackChassis);
    }

    private Chassis getRackChassisFromDiscoveredObjects(Set<DomainObject> discoveredDomainObjects) {
        Collection<Chassis> chassisCollection = filterByType(discoveredDomainObjects, Chassis.class);
        return chassisCollection.stream()
                .filter(chassis -> chassis.is(RACK))
                .findFirst()
                .orElseThrow(() -> new IllegalStateException("There should be at least one Rack discovered from RMM service."));
    }
}
