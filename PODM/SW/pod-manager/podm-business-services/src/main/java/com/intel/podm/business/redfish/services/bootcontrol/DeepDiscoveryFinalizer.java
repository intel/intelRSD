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

import com.intel.podm.business.EntityOperationException;
import com.intel.podm.business.entities.dao.ComputerSystemDao;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.redfish.services.actions.ResetActionInvoker;
import com.intel.podm.common.enterprise.utils.retry.NumberOfRetriesOnRollback;
import com.intel.podm.common.enterprise.utils.retry.RetryOnRollbackInterceptor;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.types.Id;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.interceptor.Interceptors;
import javax.transaction.Transactional;
import java.util.Objects;
import java.util.Optional;
import java.util.UUID;

import static com.intel.podm.common.types.DeepDiscoveryState.DONE;
import static com.intel.podm.common.types.DeepDiscoveryState.FAILED;
import static com.intel.podm.common.types.DeepDiscoveryState.RUNNING;
import static com.intel.podm.common.types.DiscoveryState.DEEP;
import static com.intel.podm.common.types.DiscoveryState.DEEP_FAILED;
import static javax.transaction.Transactional.TxType.MANDATORY;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

/**
 * Class responsible for managing finished deep discovery
 */
@Dependent
@Interceptors(RetryOnRollbackInterceptor.class)
class DeepDiscoveryFinalizer {
    @Inject
    private Logger logger;

    @Inject
    private ComputerSystemDao computerSystemDao;

    @Inject
    private ResetActionInvoker resetActionInvoker;

    @Transactional(MANDATORY)
    public void finalizeSuccessfulDeepDiscovery(Id computerSystemId) {
        Optional<ComputerSystem> computerSystemOption = computerSystemDao.tryFind(computerSystemId);

        if (computerSystemOption.isPresent()) {
            ComputerSystem computerSystem = computerSystemOption.get();
            deallocateComputerSystem(computerSystem);
            computerSystem.setDiscoveryState(DEEP);
        } else {
            logger.w("ComputerSystem {} has been removed during deep discovery", computerSystemId);
        }
    }

    @NumberOfRetriesOnRollback(3)
    @Transactional(REQUIRES_NEW)
    public void finalizeDeepDiscoveryOnTimeout(Id computerSystemId, UUID taskUuid) {
        Optional<ComputerSystem> optionalComputerSystem = computerSystemDao.tryFind(computerSystemId);
        if (!optionalComputerSystem.isPresent()) {
            logger.w("ComputerSystem {} has been removed during deep discovery", computerSystemId);
            return;
        }
        ComputerSystem computerSystem = optionalComputerSystem.get();

        if (Objects.equals(taskUuid, computerSystem.getMetadata().getTaskUuid()) && computerSystem.getMetadata().isInAnyOfStates(RUNNING)) {
            deallocateComputerSystem(computerSystem);
            computerSystem.setDiscoveryState(DEEP_FAILED);
            computerSystem.getMetadata().setDeepDiscoveryState(FAILED);
            logger.w("Deep discovery timed out for ComputerSystem {}, [ service: {}, path: {} ]",
                computerSystem.getId(), computerSystem.getService().getBaseUri(), computerSystem.getSourceUri());
        }
    }

    private void deallocateComputerSystem(ComputerSystem computerSystem) {
        computerSystem.getMetadata().setAllocated(false);
        computerSystem.getMetadata().setDeepDiscoveryState(DONE);
        try {
            resetActionInvoker.shutdownGracefully(computerSystem);
        } catch (EntityOperationException e) {
            logger.e("Graceful shutdown failed for ComputerSystem {} , [ service: {}, path: {} ]",
                computerSystem.getId(), computerSystem.getService().getBaseUri(), computerSystem.getSourceUri());
        }
    }
}
