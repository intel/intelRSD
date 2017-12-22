/*
 * Copyright (c) 2017 Intel Corporation
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

package com.intel.podm.business.redfish.services.actions;

import com.intel.podm.business.entities.dao.ComputerSystemDao;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.embeddables.Boot;
import com.intel.podm.client.resources.redfish.BootObject;
import com.intel.podm.client.resources.redfish.ComputerSystemResource;
import com.intel.podm.common.enterprise.utils.retry.NumberOfRetriesOnRollback;
import com.intel.podm.common.enterprise.utils.retry.RetryOnRollbackInterceptor;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.types.Id;
import com.intel.podm.mappers.subresources.TrustedModuleMapper;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.interceptor.Interceptors;
import javax.transaction.Transactional;
import java.util.Optional;

import static com.intel.podm.common.utils.Contracts.requiresNonNull;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

@Dependent
@Interceptors(RetryOnRollbackInterceptor.class)
class ComputerSystemUpdater {
    @Inject
    private ComputerSystemDao computerSystemDao;

    @Inject
    private TrustedModuleMapper trustedModuleMapper;

    @Inject
    private Logger logger;

    @NumberOfRetriesOnRollback(3)
    @Transactional(REQUIRES_NEW)
    public void updateComputerSystemWithRetry(Id computerSystemId, ComputerSystemResource computerSystemResource) throws IllegalStateException {
        Optional<ComputerSystem> expectedComputerSystem = computerSystemDao.tryFind(computerSystemId);
        if (!expectedComputerSystem.isPresent()) {
            String errorMessage = "ComputerSystem was removed before it could be updated";
            logger.i(errorMessage + ", expected ComputerSystem id: {}", computerSystemId);
            throw new IllegalStateException(errorMessage);
        }
        ComputerSystem computerSystem = expectedComputerSystem.get();
        requiresNonNull(computerSystem.getBoot(), "computerSystem.getBoot()");
        requiresNonNull(computerSystemResource, "computerSystemResource");
        requiresNonNull(computerSystemResource.getBootObject(), "computerSystemResource.getBoot()");

        Boot boot = computerSystem.getBoot();
        BootObject updatedBoot = computerSystemResource.getBootObject();
        updatedBoot.getBootSourceOverrideTarget().ifAssigned(boot::setBootSourceOverrideTarget);
        updatedBoot.getBootSourceOverrideEnabled().ifAssigned(boot::setBootSourceOverrideEnabled);
        updatedBoot.getBootSourceOverrideMode().ifAssigned(boot::setBootSourceOverrideMode);

        computerSystemResource.getAssetTag().ifAssigned(computerSystem::setAssetTag);

        updateSecurityAttributes(computerSystemResource, computerSystem);
    }

    private void updateSecurityAttributes(ComputerSystemResource computerSystemResource, ComputerSystem computerSystem) {
        computerSystemResource.getUserModeEnabled().ifAssigned(computerSystem::setUserModeEnabled);
        computerSystemResource.getTrustedModules().ifAssigned(trustedModules ->
            trustedModuleMapper.map(trustedModules, computerSystem.getTrustedModules(), computerSystem::addTrustedModule));
    }
}
