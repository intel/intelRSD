/*
 * Copyright (c) 2015-2017 Intel Corporation
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

package com.intel.podm.business.redfish.services.handlers;

import com.intel.podm.business.entities.NonUniqueResultException;
import com.intel.podm.business.entities.dao.ExternalServiceDao;
import com.intel.podm.business.entities.redfish.Chassis;
import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.common.types.redfish.RedfishEventArray;
import com.intel.podm.discovery.external.DiscoveryScheduler;
import com.intel.podm.discovery.external.ExternalServiceAvailabilityChecker;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.UUID;

import static com.intel.podm.common.types.ChassisType.DRAWER;
import static com.intel.podm.common.types.ChassisType.RACK;
import static com.intel.podm.common.utils.Contracts.requires;
import static java.util.Objects.nonNull;
import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
class RmmEventHandler implements EventHandler {

    @Inject
    private DiscoveryScheduler discoveryScheduler;

    @Inject
    private ExternalServiceDao externalServiceDao;

    @Inject
    private ExternalServiceAvailabilityChecker availabilityChecker;

    @Override
    @Transactional(MANDATORY)
    public void handle(UUID serviceUuid, RedfishEventArray eventArray) {
        ExternalService rmmService = getExternalServiceByUuid(serviceUuid);
        requires(nonNull(rmmService), "There should be a service associated with event");

        rmmService.getOwned(Chassis.class).stream()
            .filter(chassis -> chassis.is(RACK))
            .findFirst()
            .ifPresent(chassis ->
                chassis.getContainedChassis().stream()
                    .filter(containedChassis -> containedChassis.is(DRAWER))
                    .map(this::toServiceEndpointUuid)
                    .forEach(availabilityChecker::verifyServiceAvailabilityByUuid));

        discoveryScheduler.enqueueDiscovery(serviceUuid);
    }

    private ExternalService getExternalServiceByUuid(UUID serviceUuid) {
        try {
            return externalServiceDao.getExternalServiceByUuid(serviceUuid);
        } catch (NonUniqueResultException e) {
            return null;
        }
    }

    private UUID toServiceEndpointUuid(Chassis chassis) {
        ExternalService externalService = chassis.getService();
        requires(nonNull(externalService), "Drawer chassis is not attached to ExternalService");
        return externalService.getUuid();
    }
}
