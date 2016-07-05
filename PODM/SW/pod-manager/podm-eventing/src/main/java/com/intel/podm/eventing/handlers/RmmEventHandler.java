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

package com.intel.podm.eventing.handlers;

import com.intel.podm.business.entities.dao.ExternalServiceDao;
import com.intel.podm.business.entities.redfish.Chassis;
import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.discovery.external.DiscoveryScheduler;
import com.intel.podm.discovery.external.ExternalServiceAvailabilityChecker;
import com.intel.podm.eventing.dto.EventArrayDto;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.util.UUID;

import static com.google.common.base.Preconditions.checkState;
import static com.intel.podm.common.types.ChassisType.RACK;
import static java.util.Objects.nonNull;

@Dependent
public class RmmEventHandler implements EventHandler {

    @Inject
    private DiscoveryScheduler discoveryScheduler;

    @Inject
    private ExternalServiceDao externalServiceDao;

    @Inject
    private ExternalServiceAvailabilityChecker availabilityChecker;

    @Override
    public void handle(EventArrayDto eventArrayDto) {
        UUID serviceUuid = eventArrayDto.getServiceUuid();
        ExternalService rmmService = externalServiceDao.getExternalServiceByUuid(serviceUuid);
        checkState(nonNull(rmmService), "There should be a service associated with event");

        rmmService.getOwned(Chassis.class).stream()
            .filter(chassis -> chassis.is(RACK))
            .findFirst()
            .ifPresent(chassis ->
                chassis.getContainedChassis().stream()
                .map(this::toServiceEndpointUuid)
                .forEach(availabilityChecker::verifyServiceAvailabilityByUuid)
            );

        discoveryScheduler.triggerDiscovery(serviceUuid);
    }

    private UUID toServiceEndpointUuid(Chassis chassis) {
        ExternalService externalService = chassis.getService();
        checkState(nonNull(externalService), "Drawer chassis is not attached to ExternalService");
        return externalService.getUuid();
    }
}
