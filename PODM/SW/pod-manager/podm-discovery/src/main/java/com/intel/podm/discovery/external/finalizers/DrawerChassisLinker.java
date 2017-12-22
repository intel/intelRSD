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

package com.intel.podm.discovery.external.finalizers;

import com.intel.podm.business.entities.dao.ChassisDao;
import com.intel.podm.business.entities.redfish.Chassis;
import com.intel.podm.common.enterprise.utils.logger.TimeMeasured;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.discovery.external.finders.RackChassisFinder;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.Collection;
import java.util.HashMap;
import java.util.Map;
import java.util.Objects;
import java.util.stream.Stream;

import static com.intel.podm.common.types.ChassisType.POD;
import static com.intel.podm.common.types.ChassisType.RACK;
import static com.intel.podm.common.utils.IterableHelper.single;
import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
public class DrawerChassisLinker {
    @Inject
    private ChassisDao chassisDao;

    @Inject
    private Logger logger;

    @Inject
    private RackChassisFinder rackFinder;

    @TimeMeasured(tag = "[Finalizer]")
    @Transactional(MANDATORY)
    public void linkToModel(Collection<Chassis> topLevelChassis) {
        linkToPod(topLevelChassis.stream().filter(this::shouldBeLinkedToPod));
        linkToRack(topLevelChassis.stream().filter(chassis -> !shouldBeLinkedToPod(chassis)));
    }

    private void linkToPod(Stream<Chassis> chassisStream) {
        Chassis podChassis = single(chassisDao.getAllByChassisType(POD));
        chassisStream
            .peek(chassis -> logger.t("Linking chassis {}:{} ", chassis.getChassisType(), chassis.getId()))
            .forEach(podChassis::addContainedChassis);
    }

    private void linkToRack(Stream<Chassis> chassisForRackLinking) {
        RackCache rackCache = new RackCache(rackFinder);

        chassisForRackLinking
            .filter(chassis -> !isAlreadyLinkedToProperRack(chassis))
            .forEach(chassis -> {
                unlinkFromInvalidParent(chassis);
                rackCache.findAnyOrCreate(chassis.getLocationParentId()).addContainedChassis(chassis);
            });
    }

    private void unlinkFromInvalidParent(Chassis chassis) {
        Chassis parent = chassis.getContainedByChassis();
        boolean chassisIsUnderInvalidParent = parent != null
            && !Objects.equals(parent.getLocationId(), chassis.getLocationParentId());

        if (chassisIsUnderInvalidParent) {
            parent.unlinkContainedChassis(chassis);
        }
    }

    private boolean shouldBeLinkedToPod(Chassis chassis) {
        return chassis.getLocationParentId() == null || chassis.is(RACK);
    }

    private boolean isAlreadyLinkedToProperRack(Chassis chassis) {
        Chassis parent = chassis.getContainedByChassis();
        return parent != null
            && parent.is(RACK)
            && Objects.equals(parent.getLocationId(), chassis.getLocationParentId());
    }

    static class RackCache {
        private Map<String, Chassis> cache = new HashMap<>();

        private RackChassisFinder rackFinder;

        RackCache(RackChassisFinder rackFinder) {
            this.rackFinder = rackFinder;
        }

        Chassis findAnyOrCreate(String locationId) {
            return cache.computeIfAbsent(locationId, s -> rackFinder.findAnyOrCreate(locationId));
        }
    }
}
