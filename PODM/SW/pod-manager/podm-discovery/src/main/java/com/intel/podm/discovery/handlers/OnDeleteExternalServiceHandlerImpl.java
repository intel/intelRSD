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

package com.intel.podm.discovery.handlers;

import com.intel.podm.business.entities.dao.GenericDao;
import com.intel.podm.business.entities.handlers.OnDeleteExternalServiceHandler;
import com.intel.podm.business.entities.redfish.Chassis;
import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.discovery.external.ExternalServiceAvailabilityChecker;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.Set;

import static com.intel.podm.common.types.ChassisType.DRAWER;
import static com.intel.podm.common.types.ChassisType.RACK;
import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
@Transactional(MANDATORY)
public class OnDeleteExternalServiceHandlerImpl implements OnDeleteExternalServiceHandler {
    @Inject
    private GenericDao genericDao;

    @Inject
    private ExternalServiceAvailabilityChecker availabilityChecker;

    @Override
    public void preRemove(ExternalService externalService) {
        switch (externalService.getServiceType()) {
            case LUI:
                break;
            case RMM:
                handleRmmServiceDelete(externalService);
                break;
            case RSS:
                handleRssServiceRemoval(externalService);
                break;
            default:
                deleteAllOwnedEntities(externalService);
        }
    }

    private void handleRssServiceRemoval(ExternalService externalService) {
        deleteAllOwnedEntities(externalService);
    }

    private void deleteAllOwnedEntities(ExternalService externalService) {
        genericDao.removeAndClear(externalService.getAllOwnedEntities());
    }

    private void handleRmmServiceDelete(ExternalService externalService) {
        externalService.getOwned(Chassis.class).stream()
                .filter(chassis -> chassis.is(RACK))
                .findFirst()
                .ifPresent(rackChassis -> this.deleteRmmAssociatedObjects(externalService, rackChassis));
    }

    private void deleteRmmAssociatedObjects(ExternalService externalService, Chassis rackChassis) {
        if (canDeleteAllOwnedEntities(rackChassis)) {
            deleteAllOwnedEntities(externalService);
            return;
        }

        triggerPsmeServicesCheck(rackChassis);

        genericDao.removeAndClear(externalService.getAllOwnedEntities(), object -> !(object instanceof Chassis));
        externalService.getOwned(Chassis.class).stream()
                .filter(chassis -> chassis.is(RACK))
                .forEach(chassis -> chassis.unlinkExternalService(externalService));
    }

    private void triggerPsmeServicesCheck(Chassis rackChassis) {
        Set<Chassis> rackChassisSet = rackChassis.getContainedChassis();

        rackChassisSet.stream()
                .filter(chassis -> chassis.is(DRAWER))
                .map(Chassis::getService)
                .map(ExternalService::getUuid)
                .forEach(availabilityChecker::verifyServiceAvailabilityByUuid);
    }

    private boolean canDeleteAllOwnedEntities(Chassis rackChassis) {
        return !rackChassis.getContainedChassis().stream()
                .filter(chassis -> chassis.is(DRAWER))
                .findAny()
                .isPresent();
    }
}
