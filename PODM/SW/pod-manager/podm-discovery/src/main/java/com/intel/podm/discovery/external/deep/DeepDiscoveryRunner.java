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

package com.intel.podm.discovery.external.deep;

import com.intel.podm.business.entities.dao.ComputerSystemDao;
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
import java.util.Optional;
import java.util.UUID;

import static com.intel.podm.business.entities.redfish.base.DeepDiscoverable.DeepDiscoveryState.FAILED;
import static com.intel.podm.business.entities.redfish.base.DeepDiscoverable.DeepDiscoveryState.RUNNING;
import static com.intel.podm.business.entities.redfish.base.DeepDiscoverable.DeepDiscoveryState.WAITING_TO_START;
import static com.intel.podm.common.types.DiscoveryState.DEEP_FAILED;
import static java.util.UUID.randomUUID;
import static java.util.concurrent.TimeUnit.SECONDS;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

/**
 * Class responsible for triggering deep discovery process for all selected computer systems
 */
@Dependent
public class DeepDiscoveryRunner {

    private static final long DEEP_DISCOVERY_EXECUTION_TIMEOUT_SECONDS = 300L;

    @Inject
    private Logger logger;

    @Inject
    private ComputerSystemDao computerSystemDao;

    @Inject
    private DeepDiscoveryInvoker invoker;

    @Inject
    private DeepDiscoveryTimeoutTaskFactory taskFactory;

    @Inject
    private DeepDiscoveryLaunchGuard deepDiscoveryLaunchGuard;

    @Inject @Config
    private Holder<DiscoveryConfig> discoveryConfigHolder;

    @Resource(lookup = "java:/DeepDiscoveryExecutor")
    private ManagedScheduledExecutorService executorService;

    @Transactional(REQUIRES_NEW)
    public void deepDiscoverComputerSystems() {
        List<ComputerSystem> computerSystemsReadyForDeepDiscovery = computerSystemDao.getComputerSystemsByDeepDiscoveryState(WAITING_TO_START);
        for (ComputerSystem computerSystem : computerSystemsReadyForDeepDiscovery) {
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

    private UUID launchDeepDiscovery(ComputerSystem computerSystem) throws DeepDiscoveryException {
        invoker.startDeepDiscovery(computerSystem.getId());
        UUID deepDiscoveryTaskUuid = randomUUID();
        DeepDiscoveryTimeoutTask task = taskFactory.create(computerSystem.getId(), deepDiscoveryTaskUuid);
        long deepDiscoveryTimeoutSeconds = discoveryConfigHolder.get().getDeepDiscoveryTimeoutSeconds();
        executorService.schedule(task, deepDiscoveryTimeoutSeconds, SECONDS);
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
            computerSystem.setTaskUuid(taskUuid);
            computerSystem.setDeepDiscoveryState(RUNNING);
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
            computerSystem.setTaskUuid(null);
            computerSystem.setDiscoveryState(DEEP_FAILED);
            computerSystem.setAllocated(false);
            computerSystem.setDeepDiscoveryState(FAILED);
        }
    }
}
