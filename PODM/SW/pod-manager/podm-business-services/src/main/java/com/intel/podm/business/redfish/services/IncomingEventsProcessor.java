/*
 * Copyright (c) 2017-2018 Intel Corporation
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

package com.intel.podm.business.redfish.services;

import com.intel.podm.business.entities.dao.ExternalServiceDao;
import com.intel.podm.business.entities.redfish.Chassis;
import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.common.synchronization.TaskCoordinator;
import com.intel.podm.common.types.redfish.RedfishEventArray;
import com.intel.podm.discovery.ServiceExplorer;
import com.intel.podm.discovery.external.DiscoveryRunnerFactory;

import javax.enterprise.context.ApplicationScoped;
import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.List;
import java.util.Optional;
import java.util.UUID;
import java.util.function.Consumer;
import java.util.function.Function;
import java.util.stream.Stream;

import static com.intel.podm.common.types.ChassisType.DRAWER;
import static com.intel.podm.common.types.ChassisType.RACK;
import static com.intel.podm.common.types.events.EventType.RESOURCE_REMOVED;
import static java.util.stream.Collectors.toList;
import static javax.transaction.Transactional.TxType.MANDATORY;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

@ApplicationScoped
public class IncomingEventsProcessor {

    @Inject
    private ExternalServiceDao externalServiceDao;

    @Inject
    private RmmEventHandler rmmEventHandler;

    @Inject
    private DefaultEventHandler defaultEventHandler;

    @Transactional(REQUIRES_NEW)
    public void handle(UUID serviceUuid, RedfishEventArray events) {
        ExternalService service = externalServiceDao.tryGetUniqueExternalServiceByUuid(serviceUuid);
        if (service == null) {
            return;
        }
        switch (service.getServiceType()) {
            case RMM:
                rmmEventHandler.handle(service, events);
                break;
            default:
                defaultEventHandler.handle(service);
                break;
        }
    }

    @Dependent
    static class RmmEventHandler {

        @Inject
        private DefaultEventHandler defaultEventHandler;

        @Inject
        private ServiceExplorer serviceExplorer;

        @Transactional(MANDATORY)
        public void handle(ExternalService rmmService, RedfishEventArray events) {
            if (events.getEvents().stream().anyMatch(event -> RESOURCE_REMOVED.equals(event.getEventType()))) {
                findRack(rmmService)
                    .map(this::findAffectedDrawers)
                    .map(this::findExternalServices)
                    .map(serviceIsNot(rmmService))
                    .ifPresent(enqueueVerification());
            }

            defaultEventHandler.handle(rmmService);
        }

        private Function<Stream<UUID>, Stream<UUID>> serviceIsNot(ExternalService rmmService) {
            return uuidStream -> uuidStream.filter(uuid -> !rmmService.getUuid().equals(uuid));
        }

        private Consumer<Stream<UUID>> enqueueVerification() {
            return affectedServices -> affectedServices.forEach(serviceExplorer::enqueueVerification);
        }

        private List<Chassis> findAffectedDrawers(Chassis rack) {
            return rack.getContainedChassis().stream().filter(containedChassis -> containedChassis.is(DRAWER)).collect(toList());
        }

        private Optional<Chassis> findRack(ExternalService rmmService) {
            return rmmService.getOwned(Chassis.class).stream().filter(chassis -> chassis.is(RACK)).findFirst();
        }

        private Stream<UUID> findExternalServices(List<Chassis> drawers) {
            return drawers.stream()
                .flatMap(drawer -> drawer.getExternalLinks().stream())
                .map(el -> el.getExternalService().getUuid());
        }
    }

    @Dependent
    static class DefaultEventHandler {

        @Inject
        private TaskCoordinator taskCoordinator;

        @Inject
        private DiscoveryRunnerFactory discoveryRunnerFactory;

        @Transactional(MANDATORY)
        public void handle(ExternalService service) {
            taskCoordinator.registerAsync(service.getUuid(), discoveryRunnerFactory.createDiscoveryRunner(service.getUuid()));
        }
    }
}
