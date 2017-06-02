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

package com.intel.podm.discovery.external;

import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.client.api.ExternalServiceApiReaderConnectionException;
import com.intel.podm.client.api.ExternalServiceApiReaderException;
import com.intel.podm.client.api.reader.ExternalServiceReader;
import com.intel.podm.client.api.reader.ExternalServiceReaderFactory;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.synchronization.CancelableRunnable;
import com.intel.podm.common.synchronization.TaskCanceledException;
import com.intel.podm.config.base.Config;
import com.intel.podm.config.base.Holder;
import com.intel.podm.config.base.dto.DiscoveryConfig;
import com.intel.podm.discovery.external.restgraph.RestGraph;
import com.intel.podm.discovery.external.restgraph.RestGraphBuilderFactory;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.Objects;
import java.util.UUID;

import static com.intel.podm.common.utils.Contracts.requiresNonNull;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

@Dependent
@SuppressWarnings({"checkstyle:ClassFanOutComplexity", "checkstyle:IllegalCatch"})
public class DiscoveryRunner extends CancelableRunnable {
    @Inject
    private Logger logger;

    @Inject
    private ExternalServiceAvailabilityChecker availabilityChecker;

    @Inject
    private ExternalServiceReaderFactory readerFactory;

    @Inject
    private ExternalServiceRepository externalServiceRepository;

    @Inject
    private RestGraphBuilderFactory restGraphBuilderFactory;

    @Inject
    private EntityGraphMapper mapper;

    @Inject
    @Config
    private Holder<DiscoveryConfig> discoveryConfig;

    private UUID serviceUuid;

    @Override
    @Transactional(REQUIRES_NEW)
    public void run() {
        try {
            requiresNonNull(serviceUuid, "Service UUID cannot be null, discovery action has not been configured correctly");
            ExternalService service = externalServiceRepository.findOrNull(serviceUuid);

            if (service == null) {
                logger.e("Service with UUID: {} does not exist", serviceUuid);
                return;
            }

            discover(service);
        } finally {
            clearCancellationFlag();
        }
    }

    private void discover(ExternalService service) {
        logger.i("Polling data from {} started", service);
        try (ExternalServiceReader reader = readerFactory.createExternalServiceReaderWithCacheAndRetries(service.getBaseUri())) {
            UUID obtainedUuid = reader.getServiceRoot().getUuid();
            if (!Objects.equals(obtainedUuid, serviceUuid)) {
                logger.e("Service UUID change detected, terminating discovery! Expected UUID: {} obtained UUID: {}", serviceUuid, obtainedUuid);
                availabilityChecker.verifyServiceAvailabilityByUuid(serviceUuid);
                return;
            }
            RestGraph graph = restGraphBuilderFactory.createWithCancelableChecker(this::throwIfEligibleForCancellation).build(reader);
            mapper.map(graph);
            logger.i("Polling data from {} finished", service);
        } catch (ExternalServiceApiReaderConnectionException e) {
            logger.w("Connection error while getting data from {} service - performing check on this service", service);
            availabilityChecker.verifyServiceAvailabilityByUuid(serviceUuid);
        } catch (ExternalServiceApiReaderException e) {
            logger.w("Unable to process data from {} service, error: {}", service, e.getErrorResponse());
        } catch (TaskCanceledException e) {
            logger.i("Discovery was canceled for {}", service);
            throw e;
        } catch (RuntimeException e) {
            logger.e("Error while polling data from " + service, e);
        }
    }

    private void throwIfEligibleForCancellation() {
        if (discoveryConfig.get().isDiscoveryCancelable()) {
            throwWithMessageIfEligibleForCancellation("Discovery was canceled.");
        }
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
        DiscoveryRunner that = (DiscoveryRunner) o;
        return serviceUuid.equals(that.serviceUuid);
    }

    @Override
    public int hashCode() {
        return serviceUuid.hashCode();
    }

    @Override
    public String toString() {
        return String.format("DiscoveryRunner(%s)", serviceUuid);
    }
}
