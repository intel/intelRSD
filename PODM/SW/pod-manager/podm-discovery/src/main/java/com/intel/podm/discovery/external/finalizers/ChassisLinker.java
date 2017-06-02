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
import com.intel.podm.common.logger.Logger;
import com.intel.podm.discovery.external.finders.RackChassisFinder;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.Objects;

import static com.intel.podm.common.types.ChassisType.POD;
import static com.intel.podm.common.types.ChassisType.RACK;
import static com.intel.podm.common.utils.IterableHelper.single;
import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
public class ChassisLinker {
    @Inject
    private ChassisDao chassisDao;

    @Inject
    private Logger logger;

    @Inject
    private RackChassisFinder rackFinder;

    @Transactional(MANDATORY)
    public void linkToModel(Chassis chassis) {
        logger.d("Linking chassis {}:{} ", chassis.getChassisType(), chassis.getId());

        if (shouldBeLinkedToPod(chassis)) {
            linkToPod(chassis);
        } else {
            linkToRack(chassis);
        }
    }

    private void linkToRack(Chassis chassis) {
        if (isAlreadyLinkedToProperRack(chassis)) {
            return;
        }

        unlinkFromInvalidParent(chassis);
        rackFinder
            .findAnyOrCreate(chassis.getLocationParentId())
            .addContainedChassis(chassis);
    }

    private void unlinkFromInvalidParent(Chassis chassis) {
        Chassis parent = chassis.getContainedByChassis();
        boolean chassisIsUnderInvalidParent = parent != null
            && !Objects.equals(parent.getLocationId(), chassis.getLocationParentId());

        if (chassisIsUnderInvalidParent) {
            parent.unlinkContainedChassis(chassis);
        }
    }

    private void linkToPod(Chassis chassis) {
        Chassis pod = single(chassisDao.getAllByChassisType(POD));
        pod.addContainedChassis(chassis);
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
}
