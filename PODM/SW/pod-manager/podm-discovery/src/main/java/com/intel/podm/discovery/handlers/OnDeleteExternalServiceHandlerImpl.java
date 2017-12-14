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
import com.intel.podm.business.entities.redfish.ExternalLink;
import com.intel.podm.business.entities.redfish.ExternalLinkDao;
import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.discovery.ServiceExplorer;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.Collection;
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
    private ExternalLinkDao externalLinkDao;

    @Inject
    private ServiceExplorer serviceExplorer;

    @Override
    public void preRemove(ExternalService externalService) {
        switch (externalService.getServiceType()) {
            case LUI:
                break;
            case RMM:
                handleRmmServiceDelete(externalService);
                break;
            default:
                deleteAllOwnedEntities(externalService);
        }
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

        triggerExternalServicesCheck(rackChassis);
        removeAllEntitiesButRack(externalService, rackChassis);
        clearReferenceToExternalService(externalService, rackChassis);
    }

    private void removeAllEntitiesButRack(ExternalService externalService, Chassis rackChassis) {
        externalLinkDao.removeAll(externalService, externalLink -> !externalLink.getDiscoverableEntity().equals(rackChassis));
    }

    private void clearReferenceToExternalService(ExternalService externalService, Chassis rackChassis) {
        Set<ExternalLink> existingLinks = rackChassis.getExternalLinks();
        for (ExternalLink existingLink : existingLinks) {
            externalService.unlinkOwnedLink(existingLink);
            genericDao.remove(existingLink);
        }
    }

    private boolean canDeleteAllOwnedEntities(Chassis rackChassis) {
        return !rackChassis.getContainedChassis().stream()
            .filter(chassis -> chassis.is(DRAWER))
            .findAny()
            .isPresent();
    }

    private void triggerExternalServicesCheck(Chassis rackChassis) {
        Set<Chassis> rackChassisSet = rackChassis.getContainedChassis();

        rackChassisSet.stream()
            .filter(chassis -> chassis.is(DRAWER))
            .map(Chassis::getExternalServices)
            .flatMap(Collection::stream)
            .map(ExternalService::getUuid)
            .forEach(serviceExplorer::enqueueVerification);
    }

    private void deleteAllOwnedEntities(ExternalService externalService) {
        externalLinkDao.removeAll(externalService, id -> true);
    }
}
