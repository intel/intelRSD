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
import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.discovery.external.ExternalServiceAvailabilityChecker;
import com.intel.podm.discovery.external.StorageGuard;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;

import static com.intel.podm.common.types.ChassisType.DRAWER;
import static com.intel.podm.common.types.ChassisType.RACK;
import static java.util.Optional.ofNullable;
import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
@Transactional(MANDATORY)
public class OnDeleteExternalServiceHook implements OnDeleteHook {

    @Inject
    private GenericDao genericDao;

    @Inject
    private StorageGuard storageGuard;

    @Inject
    private ExternalServiceAvailabilityChecker availabilityChecker;

    @Override
    public boolean canExecute(DomainObject domainObject) {
        return domainObject instanceof ExternalService;
    }

    @Override
    public void execute(DomainObject domainObject) {
        ExternalService service = (ExternalService) domainObject;

        switch (service.getServiceType()) {
            case LUI:
                break;
            case RMM:
                handleRmmServiceDelete(service);
                break;
            case RSS:
                handleRssServiceRemoval(service);
                break;
            default:
                deleteAllOwnedDomainObjects(service);
        }
    }

    private void handleRssServiceRemoval(ExternalService service) {
        storageGuard.releaseAssociatedStorageComputerSystem(service);
        deleteAllOwnedDomainObjects(service);
    }

    private void deleteAllOwnedDomainObjects(ExternalService service) {
        service.getAllOwnedDomainObjects().forEach(genericDao::remove);
    }

    private void handleRmmServiceDelete(ExternalService service) {
        service.getOwned(Chassis.class).stream()
                .filter(chassis -> chassis.is(RACK))
                .findFirst()
                .ifPresent(rackChassis -> this.deleteRmmAssociatedObjects(service, rackChassis));
    }

    private void deleteRmmAssociatedObjects(ExternalService service, Chassis rackChassis) {
        if (canDeleteAllOwnedDomainObjects(rackChassis)) {
            deleteAllOwnedDomainObjects(service);
            return;
        }

        triggerPsmeServicesCheck(rackChassis);

        ofNullable(rackChassis.getRackChassisAttributes()).ifPresent(genericDao::remove);
        service.getAllOwnedDomainObjects().stream()
                .filter(object -> !(object instanceof Chassis))
                .forEach(genericDao::remove);
    }

    private void triggerPsmeServicesCheck(Chassis rackChassis) {
        rackChassis.getContainedChassis().stream()
            .filter(chassis -> chassis.is(DRAWER))
            .map(Chassis::getService)
            .forEach(service -> availabilityChecker.verifyServiceAvailabilityByUuid(service.getUuid()));
    }

    private boolean canDeleteAllOwnedDomainObjects(Chassis rackChassis) {
        return !rackChassis.getContainedChassis().stream()
                .filter(chassis -> chassis.is(DRAWER))
                .findAny()
                .isPresent();
    }
}
