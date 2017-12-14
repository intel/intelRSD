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

package com.intel.podm.business.redfish.services.bootcontrol;

import com.intel.podm.business.entities.dao.ComputerSystemDao;
import com.intel.podm.business.entities.dao.ExternalServiceDao;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.common.enterprise.utils.retry.NumberOfRetriesOnRollback;
import com.intel.podm.common.enterprise.utils.retry.RetryOnRollbackInterceptor;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.types.Id;
import com.intel.podm.config.base.Config;
import com.intel.podm.config.base.Holder;
import com.intel.podm.config.base.dto.DiscoveryConfig;

import javax.annotation.Resource;
import javax.enterprise.concurrent.ManagedScheduledExecutorService;
import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.interceptor.Interceptors;
import javax.transaction.Transactional;
import java.util.List;
import java.util.Objects;
import java.util.Optional;
import java.util.UUID;

import static com.intel.podm.common.enterprise.utils.beans.JndiNames.DEEP_DISCOVERY_EXECUTOR;
import static com.intel.podm.common.types.DeepDiscoveryState.FAILED;
import static com.intel.podm.common.types.DeepDiscoveryState.RUNNING;
import static com.intel.podm.common.types.DeepDiscoveryState.WAITING_TO_START;
import static com.intel.podm.common.types.DiscoveryState.DEEP_FAILED;
import static java.lang.String.format;
import static java.util.UUID.randomUUID;
import static java.util.concurrent.TimeUnit.SECONDS;
import static java.util.stream.Collectors.toList;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

/**
 * Class responsible for triggering deep discovery process for all selected computer systems
 */
@Dependent
@SuppressWarnings("checkstyle:ClassFanOutComplexity")
class DeepDiscoveryRunner {
    @Inject
    private Logger logger;

    @Inject
    @Config
    private Holder<DiscoveryConfig> configHolder;

    @Inject
    private ExternalServiceDao externalServiceDao;

    @Inject
    private DeepDiscoveryInvoker invoker;

    @Inject
    private DeepDiscoveryTimeoutTaskFactory taskFactory;

    @Inject
    private DeepDiscoveryLaunchGuard deepDiscoveryLaunchGuard;

    @Resource(lookup = "java:/" + DEEP_DISCOVERY_EXECUTOR)
    private ManagedScheduledExecutorService executorService;

    @Transactional(REQUIRES_NEW)
    public void deepDiscoverComputerSystems(UUID serviceUuid) {
        ExternalService externalService = externalServiceDao.tryGetUniqueExternalServiceByUuid(serviceUuid);
        if (externalService == null) {
            throw new RuntimeException(format("Starting deep discovery failed, unique Service with UUID: %s was not found.", serviceUuid));
        }

        for (ComputerSystem computerSystem : getComputerSystemsWaitingToStart(externalService)) {
            ExternalService service = computerSystem.getService();
            if (service == null) {
                logger.w("There is no Service associated with ComputerSystem {}, skipping deep discovery launch", computerSystem.getId());
                continue;
            }
            try {
                UUID taskUuid = launchDeepDiscovery(computerSystem);
                logger.i("Deep discovery for ComputerSystem {} started, [ service: {}, path: {} ]",
                    computerSystem.getId(), service.getBaseUri(), computerSystem.getSourceUri());
                deepDiscoveryLaunchGuard.onDeepDiscoveryLaunchSuccess(computerSystem.getId(), taskUuid);
            } catch (DeepDiscoveryException e) {
                logger.e("Starting deep discovery failed: ", e);
                deepDiscoveryLaunchGuard.onDeepDiscoveryLaunchFailure(computerSystem.getId());
            }
        }
    }

    private List<ComputerSystem> getComputerSystemsWaitingToStart(ExternalService externalService) {
        return externalService.getOwned(ComputerSystem.class).stream()
            .filter(computerSystem -> Objects.equals(computerSystem.getMetadata().getDeepDiscoveryState(), WAITING_TO_START))
            .collect(toList());
    }

    private UUID launchDeepDiscovery(ComputerSystem computerSystem) throws DeepDiscoveryException {
        invoker.startDeepDiscovery(computerSystem.getId());
        UUID deepDiscoveryTaskUuid = randomUUID();
        DeepDiscoveryTimeoutTask task = taskFactory.create(computerSystem.getId(), deepDiscoveryTaskUuid);
        executorService.schedule(task, configHolder.get().getDeepDiscoveryExecutionTimeoutSeconds(), SECONDS);
        return deepDiscoveryTaskUuid;
    }

    @Dependent
    @Interceptors(RetryOnRollbackInterceptor.class)
    private static class DeepDiscoveryLaunchGuard {
        @Inject
        private ComputerSystemDao computerSystemDao;

        @Inject
        private Logger logger;

        DeepDiscoveryLaunchGuard() {
        }

        @NumberOfRetriesOnRollback(3)
        @Transactional(REQUIRES_NEW)
        public void onDeepDiscoveryLaunchSuccess(Id computerSystemId, UUID taskUuid) {
            Optional<ComputerSystem> possibleComputerSystem = computerSystemDao.tryFind(computerSystemId);
            if (!possibleComputerSystem.isPresent()) {
                logger.w("ComputerSystem {} has been removed during deep discovery", computerSystemId);
                return;
            }

            ComputerSystem computerSystem = possibleComputerSystem.get();
            computerSystem.getMetadata().setTaskUuid(taskUuid);
            computerSystem.getMetadata().setDeepDiscoveryState(RUNNING);
        }

        @NumberOfRetriesOnRollback(3)
        @Transactional(REQUIRES_NEW)
        public void onDeepDiscoveryLaunchFailure(Id computerSystemId) {
            Optional<ComputerSystem> possibleComputerSystem = computerSystemDao.tryFind(computerSystemId);
            if (!possibleComputerSystem.isPresent()) {
                logger.w("ComputerSystem {} has been removed during deep discovery", computerSystemId);
                return;
            }

            ComputerSystem computerSystem = possibleComputerSystem.get();
            computerSystem.getMetadata().setTaskUuid(null);
            computerSystem.getMetadata().setAllocated(false);
            computerSystem.getMetadata().setDeepDiscoveryState(FAILED);
            computerSystem.setDiscoveryState(DEEP_FAILED);
        }
    }
}
