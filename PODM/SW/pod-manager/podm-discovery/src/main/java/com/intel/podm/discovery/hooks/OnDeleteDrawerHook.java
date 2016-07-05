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

package com.intel.podm.discovery.hooks;

import com.intel.podm.business.entities.base.DomainObject;
import com.intel.podm.business.entities.dao.GenericDao;
import com.intel.podm.business.entities.hooks.OnDeleteHook;
import com.intel.podm.business.entities.redfish.Chassis;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;

import static com.google.common.base.Preconditions.checkState;
import static com.intel.podm.common.types.ChassisType.DRAWER;
import static com.intel.podm.common.types.ChassisType.RACK;
import static com.intel.podm.common.types.ServiceType.RMM;
import static java.util.Objects.nonNull;
import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
@Transactional(MANDATORY)
public class OnDeleteDrawerHook implements OnDeleteHook {
    @Inject
    private GenericDao genericDao;

    @Override
    public boolean canExecute(DomainObject domainObjectClass) {
        return domainObjectClass instanceof Chassis
                && ((Chassis) domainObjectClass).is(DRAWER);
    }

    @Override
    public void execute(DomainObject domainObject) {
        Chassis drawerChassis = (Chassis) domainObject;
        Chassis parentChassis = drawerChassis.getContainedBy();

        checkState(nonNull(parentChassis), "Drawer must have exact one parent Chassis.");

        if (rackShouldBeDeleted(parentChassis)) {
            genericDao.remove(parentChassis);
        }
    }

    private boolean rackShouldBeDeleted(Chassis parentChassis) {
        return parentChassis.is(RACK) && rackContainsLessThanTwoDrawers(parentChassis)
                && rackIsNotOwnedByRmmService(parentChassis);
    }

    private boolean rackContainsLessThanTwoDrawers(Chassis rackChassis) {
        return rackChassis.getContainedChassis().stream().filter(chassis -> chassis.is(DRAWER)).count() < 2;
    }

    private boolean rackIsNotOwnedByRmmService(Chassis rackChassis) {
        return rackChassis.getService() == null || !RMM.equals(rackChassis.getService().getServiceType());
    }
}
