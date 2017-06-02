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

package com.intel.podm.business.redfish.services;

import com.intel.podm.actions.ActionException;
import com.intel.podm.actions.ChassisActionsInvoker;
import com.intel.podm.business.BusinessApiException;
import com.intel.podm.business.InvalidPayloadException;
import com.intel.podm.business.entities.redfish.Chassis;
import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.business.entities.redfish.base.DiscoverableEntity;
import com.intel.podm.business.redfish.EntityTreeTraverser;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.common.enterprise.utils.beans.BeanFactory;
import com.intel.podm.common.enterprise.utils.retry.NumberOfRetriesOnRollback;
import com.intel.podm.common.enterprise.utils.retry.RetryOnRollbackInterceptor;
import com.intel.podm.common.synchronization.TaskCoordinator;
import com.intel.podm.common.types.actions.BaseChassisUpdateDefinition;
import com.intel.podm.common.types.actions.ChassisUpdateDefinition;
import com.intel.podm.common.types.actions.LegacyRackChassisUpdateDefinition;
import com.intel.podm.common.types.redfish.RedfishChassis;
import com.intel.podm.discovery.external.DiscoveryRunner;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.interceptor.Interceptors;
import javax.transaction.Transactional;
import java.util.List;
import java.util.Map;
import java.util.UUID;
import java.util.concurrent.TimeoutException;
import java.util.function.Consumer;

import static java.lang.Integer.parseInt;
import static java.util.stream.Collectors.groupingBy;
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
    private BeanFactory beanFactory;

    @Transactional(REQUIRES_NEW)
    @NumberOfRetriesOnRollback(9)
    public void updateChassis(Context chassisContext, RedfishChassis representation) throws BusinessApiException {
        Chassis chassis = (Chassis) traverser.traverse(chassisContext);

        if (chassis.getService() != null) {
            handleForExternalService(representation, chassis);
        } else {
            handleForPod(representation, chassis);
        }
    }

    private void handleForExternalService(RedfishChassis representation, Chassis chassis) throws BusinessApiException {
        try {
            BaseChassisUpdateDefinition chassisUpdateDefinition = prepareChassisUpdateDefinition(representation, chassis);

            invoker.updateChassis(chassis, chassisUpdateDefinition);

            if (representation.getAssetTag() != null) {
                chassis.setAssetTag(representation.getAssetTag());
            }

            if (representation.getLocationId() != null) {
                runDiscovery(chassis.getService().getUuid());
            }
        } catch (ActionException e) {
            throw new BusinessApiException(e.getMessage(), e);
        }
    }

    private BaseChassisUpdateDefinition prepareChassisUpdateDefinition(RedfishChassis representation, Chassis chassis)
        throws ActionException, InvalidPayloadException {

        String assetTag = representation.getAssetTag();
        String locationId = representation.getLocationId();

        if (locationId != null && !invoker.chassisHasLocationId(chassis)) {
            try {
                Integer rackPuid = parseInt(locationId);
                return new LegacyRackChassisUpdateDefinition(assetTag, rackPuid);
            } catch (NumberFormatException e) {
                throw new InvalidPayloadException("Couldn't parse Location.Id string as number. " + e.getMessage(), e);
            }
        } else {
            return new ChassisUpdateDefinition(assetTag, locationId);
        }
    }

    private void runDiscovery(UUID serviceUuid) {
        DiscoveryRunner discoveryRunner = beanFactory.create(DiscoveryRunner.class);
        discoveryRunner.setServiceUuid(serviceUuid);
        discoveryRunner.run();
    }

    private void forEachContainedChassis(Chassis chassis, Consumer<Chassis> chassisConsumer) throws BusinessApiException {
        Map<ExternalService, List<Chassis>> chassisByExternalService =
            chassis.getContainedChassis().stream().filter(de -> de.getService() != null).collect(groupingBy(DiscoverableEntity::getService));

        for (Map.Entry<ExternalService, List<Chassis>> entry : chassisByExternalService.entrySet()) {
            try {
                taskCoordinator.run(
                    entry.getKey().getUuid(),
                    () -> entry.getValue().forEach(chassisConsumer)
                );
            } catch (TimeoutException e) {
                throw new BusinessApiException(e.getMessage(), e);
            }
        }
        chassis.getContainedChassis().stream().filter(de -> de.getService() == null).forEach(chassisConsumer);
    }

    private void handleForPod(RedfishChassis representation, Chassis chassis) throws BusinessApiException {
        if (representation.getAssetTag() != null) {
            chassis.setAssetTag(representation.getAssetTag());
        }
        if (representation.getLocationId() != null) {
            String locationId = representation.getLocationId();
            chassis.setLocationId(locationId);
            forEachContainedChassis(chassis, ch -> ch.setLocationParentId(locationId));
        }
    }
}
