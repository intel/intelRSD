/*
 * Copyright (c) 2016-2018 Intel Corporation
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

import com.intel.podm.business.BusinessApiException;
import com.intel.podm.business.ContextResolvingException;
import com.intel.podm.business.RequestValidationException;
import com.intel.podm.business.entities.redfish.Chassis;
import com.intel.podm.business.entities.redfish.ExternalLink;
import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.business.entities.redfish.base.DiscoverableEntity;
import com.intel.podm.business.entities.redfish.embeddables.RackChassisAttributes;
import com.intel.podm.business.redfish.EntityTreeTraverser;
import com.intel.podm.business.redfish.services.actions.ChassisActionsInvoker;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.common.enterprise.utils.retry.RetryOnRollback;
import com.intel.podm.common.enterprise.utils.retry.RetryOnRollbackInterceptor;
import com.intel.podm.common.synchronization.TaskCoordinator;
import com.intel.podm.common.types.actions.BaseChassisUpdateDefinition;
import com.intel.podm.common.types.actions.ChassisUpdateDefinition;
import com.intel.podm.common.types.actions.LegacyRackChassisUpdateDefinition;
import com.intel.podm.common.types.redfish.RedfishChassis;
import com.intel.podm.discovery.ServiceExplorer;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.interceptor.Interceptors;
import javax.transaction.Transactional;
import java.util.Collection;
import java.util.List;
import java.util.Map;
import java.util.Objects;
import java.util.Optional;
import java.util.UUID;
import java.util.concurrent.TimeoutException;
import java.util.function.Consumer;
import java.util.stream.Collector;
import java.util.stream.Stream;

import static com.intel.podm.business.Violations.createWithViolations;
import static java.lang.Integer.parseInt;
import static java.lang.String.format;
import static java.util.stream.Collectors.groupingBy;
import static java.util.stream.Collectors.mapping;
import static java.util.stream.Collectors.toList;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

@Dependent
@SuppressWarnings("checkstyle:ClassFanOutComplexity")
@Interceptors(RetryOnRollbackInterceptor.class)
public class ChassisActionsService {
    @Inject
    private ChassisActionsInvoker invoker;

    @Inject
    private EntityTreeTraverser traverser;

    @Inject
    private TaskCoordinator taskCoordinator;

    @Inject
    private ServiceExplorer serviceExplorer;

    @Transactional(REQUIRES_NEW)
    @RetryOnRollback(9)
    public void updateLocalChassis(Context chassisContext, RedfishChassis representation) throws TimeoutException, ContextResolvingException {
        Chassis localChassis = (Chassis) traverser.traverse(chassisContext);
        updateLocalChassis(representation, localChassis);
    }

    @Transactional(REQUIRES_NEW)
    @RetryOnRollback(9)
    public void updateChassisOnSingleExternalService(UUID serviceUuid, Context chassisContext, RedfishChassis representation) throws BusinessApiException {
        Chassis chassis = (Chassis) traverser.traverse(chassisContext);

        Optional<ExternalLink> externalLink = chassis.getExternalLinks()
            .stream()
            .filter(el -> Objects.equals(el.getExternalService().getUuid(), serviceUuid))
            .findAny();

        if (externalLink.isPresent()) {
            updateChassisOnSingleExternalService(externalLink.get(), representation);
        } else {
            String msg = format("Chassis %s is not owned by External Service %s", chassisContext, serviceUuid);
            throw new IllegalArgumentException(msg);
        }
    }

    private void updateChassisOnSingleExternalService(ExternalLink externalLink, RedfishChassis representation) throws BusinessApiException {
        BaseChassisUpdateDefinition chassisUpdateDefinition = prepareChassisUpdateDefinition(representation, externalLink);
        invoker.updateChassis(externalLink, chassisUpdateDefinition);

        boolean fullUpdateRequired = representation.getLocationId() != null;

        if (fullUpdateRequired) {
            // TODO eliminate use of forced discovery (deprecated)
            serviceExplorer.discover(externalLink.getExternalService().getUuid());
        } else {
            Chassis discoverableEntity = (Chassis) externalLink.getDiscoverableEntity();

            if (representation.getAssetTag() != null) {
                discoverableEntity.setAssetTag(representation.getAssetTag());
            }

            if (representation.getGeoTag() != null) {
                if (discoverableEntity.getRackChassisAttributes() == null) {
                    discoverableEntity.setRackChassisAttributes(new RackChassisAttributes());
                }
                discoverableEntity.getRackChassisAttributes().setGeoTag(representation.getGeoTag());
            }
        }
    }

    private BaseChassisUpdateDefinition prepareChassisUpdateDefinition(RedfishChassis representation, ExternalLink externalLink)
        throws BusinessApiException {

        String assetTag = representation.getAssetTag();
        String geoTag = representation.getGeoTag();
        String locationId = representation.getLocationId();

        if (locationId != null && !invoker.chassisHasLocationId(externalLink)) {
            try {
                Integer rackPuid = parseInt(locationId);
                return new LegacyRackChassisUpdateDefinition(assetTag, geoTag, rackPuid);
            } catch (NumberFormatException e) {
                throw new RequestValidationException(createWithViolations("Location.Id should be a string representation of a number."));
            }
        } else {
            return new ChassisUpdateDefinition(assetTag, geoTag, locationId);
        }
    }

    private void forEachContainedChassis(Chassis localChassis, Consumer<Chassis> chassisConsumer) throws TimeoutException {
        Map<ExternalService, List<Chassis>> chassisByService = findAffectedExternalLinks(localChassis).collect(groupChassisByExternalService());
        for (Map.Entry<ExternalService, List<Chassis>> entry : chassisByService.entrySet()) {
            taskCoordinator.run(entry.getKey().getUuid(), () -> entry.getValue().forEach(chassisConsumer));
        }
        localChassis.getContainedChassis().stream().filter(de -> !de.hasAnyExternalServiceAssigned()).forEach(chassisConsumer);
    }

    private Collector<ExternalLink, ?, Map<ExternalService, List<Chassis>>> groupChassisByExternalService() {
        return groupingBy(
            ExternalLink::getExternalService,
            mapping(el -> (Chassis) el.getDiscoverableEntity(), toList())
        );
    }

    private Stream<ExternalLink> findAffectedExternalLinks(Chassis chassis) {
        return chassis.getContainedChassis().stream()
            .map(DiscoverableEntity::getExternalLinks)
            .filter(links -> !links.isEmpty())
            .flatMap(Collection::stream);
    }

    private void updateLocalChassis(RedfishChassis representation, Chassis localChassis) throws TimeoutException {
        if (representation.getAssetTag() != null) {
            localChassis.setAssetTag(representation.getAssetTag());
        }
        if (representation.getLocationId() != null) {
            String locationId = representation.getLocationId();
            localChassis.setLocationId(locationId);
            forEachContainedChassis(localChassis, chassis -> chassis.setLocationParentId(locationId));
        }
    }
}
