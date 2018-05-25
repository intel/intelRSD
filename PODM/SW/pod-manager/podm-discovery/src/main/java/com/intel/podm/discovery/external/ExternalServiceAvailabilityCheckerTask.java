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

package com.intel.podm.discovery.external;

import com.intel.podm.business.entities.dao.ChassisDao;
import com.intel.podm.business.entities.redfish.Chassis;
import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.business.entities.redfish.base.ComposableAsset;
import com.intel.podm.business.entities.redfish.embeddables.RackChassisAttributes;
import com.intel.podm.client.WebClientRequestException;
import com.intel.podm.client.reader.ExternalServiceReader;
import com.intel.podm.client.reader.ExternalServiceReaderFactory;
import com.intel.podm.client.resources.redfish.ServiceRootResource;
import com.intel.podm.common.enterprise.utils.logger.ServiceLifecycle;
import com.intel.podm.common.enterprise.utils.tasks.DefaultManagedTask;
import com.intel.podm.common.logger.ServiceLifecycleLogger;
import com.intel.podm.common.types.ServiceType;
import com.intel.podm.common.types.Status;
import com.intel.podm.config.base.Config;
import com.intel.podm.config.base.Holder;
import com.intel.podm.config.base.dto.ExternalServiceConfig;
import com.intel.podm.discovery.external.finalizers.ComposableAssetsDiscoveryListener;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.List;
import java.util.Objects;
import java.util.UUID;

import static com.intel.podm.common.types.ChassisType.RACK;
import static com.intel.podm.common.types.ServiceType.LUI;
import static com.intel.podm.common.types.ServiceType.RMM;
import static com.intel.podm.common.types.State.ABSENT;
import static com.intel.podm.common.utils.Collections.filterByType;
import static com.intel.podm.common.utils.Contracts.requiresNonNull;
import static java.lang.String.format;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

@Dependent
@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
class ExternalServiceAvailabilityCheckerTask extends DefaultManagedTask implements Runnable {
    @Inject
    @ServiceLifecycle
    private ServiceLifecycleLogger logger;

    @Inject
    private ExternalServiceRepository externalServiceRepository;

    @Inject
    private ExternalServiceReaderFactory readerFactory;

    @Inject
    @Config
    private Holder<ExternalServiceConfig> configHolder;

    @Inject
    private ComposableAssetsDiscoveryListener composableAssetsDiscoveryListener;

    @Inject
    private ChassisDao chassisDao;

    private UUID serviceUuid;

    @Override
    @Transactional(REQUIRES_NEW)
    public void run() {
        requiresNonNull(serviceUuid, "serviceUuid");
        logger.d("Verifying service with UUID {}", serviceUuid);

        ExternalService service = externalServiceRepository.findOrNull(serviceUuid);
        if (service == null) {
            logger.w("Requested service({}) is already removed", serviceUuid);
            return;
        }

        try (ExternalServiceReader reader = readerFactory.createExternalServiceReaderWithRetries(service.getBaseUri())) {
            ServiceRootResource serviceRoot = reader.getServiceRoot();
            if (!Objects.equals(serviceRoot.getUuid(), serviceUuid)) {
                logger.w("Service Root with UUID {} is no longer available at URI {}, UUID found at this URI: {}",
                    serviceUuid, serviceRoot.getUri(), serviceRoot.getUuid());
                markAsUnreachable(service);
            } else {
                logger.d("Service {} still exists", service);
            }
        } catch (WebClientRequestException e) {
            markAsUnreachable(service);
        }
    }

    private void markAsUnreachable(ExternalService service) {
        if (!service.isReachable()) {
            logger.lifecycleInfo("Service {} is already marked as unreachable", service);
            return;
        }

        ServiceType serviceType = service.getServiceType();
        if (!Objects.equals(serviceType, LUI)) {
            markExternalServiceUnreachable(service);
        }

        if (Objects.equals(serviceType, RMM)) {
            markRmmServiceUnreachable(service);
        }
    }

    private void markRmmServiceUnreachable(ExternalService service) {
        List<Chassis> rmmChassis = chassisDao.getChassis(RACK, service);
        requiresNonNull(rmmChassis, "chassis list");
        if (rmmChassis.size() == 1) {
            markRackAsUnreachable(rmmChassis.get(0));
        } else {
            throw new IllegalStateException(format("RMM should contain exactly one chassis, but found: %d", rmmChassis.size()));
        }
    }

    private void markExternalServiceUnreachable(ExternalService service) {
        logger.lifecycleInfo(
            "Service {} has been set to ABSENT state. Scheduling removal of service after {}",
            service,
            configHolder.get().getServiceRemovalDelay()
        );
        service.markAsNotReachable();
        composableAssetsDiscoveryListener.updateRelatedComposedNodes(filterByType(service.getOwnedEntities(), ComposableAsset.class));
    }

    private void markRackAsUnreachable(Chassis chassis) {
        chassis.setDescription(null);
        chassis.setManufacturer(null);
        chassis.setModel(null);
        chassis.setSku(null);
        chassis.setSerialNumber(null);
        chassis.setPartNumber(null);
        chassis.setAssetTag(null);
        chassis.setIndicatorLed(null);
        chassis.setStatus(new Status(ABSENT, null, null));
        RackChassisAttributes attributes = chassis.getRackChassisAttributes();
        attributes.setGeoTag(null);
        attributes.setRackSupportsDisaggregatedPowerCooling(null);
        chassis.setRackChassisAttributes(attributes);
    }

    public void setServiceUuid(UUID serviceUuid) {
        this.serviceUuid = serviceUuid;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) {
            return true;
        }
        if (o == null || getClass() != o.getClass()) {
            return false;
        }
        ExternalServiceAvailabilityCheckerTask that = (ExternalServiceAvailabilityCheckerTask) o;
        return Objects.equals(serviceUuid, that.serviceUuid);
    }

    @Override
    public int hashCode() {
        return Objects.hash(serviceUuid);
    }
}
