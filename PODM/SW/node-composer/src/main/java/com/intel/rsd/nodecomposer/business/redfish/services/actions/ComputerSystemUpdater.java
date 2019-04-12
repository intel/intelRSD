/*
 * Copyright (c) 2017-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.business.redfish.services.actions;

import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.externalservices.resources.redfish.BootObject;
import com.intel.rsd.nodecomposer.externalservices.resources.redfish.ComputerSystemResource;
import com.intel.rsd.nodecomposer.mappers.subresources.TrustedModuleMapper;
import com.intel.rsd.nodecomposer.persistence.dao.ComputerSystemDao;
import com.intel.rsd.nodecomposer.persistence.redfish.ComputerSystem;
import com.intel.rsd.nodecomposer.persistence.redfish.embeddables.Boot;
import com.intel.rsd.nodecomposer.utils.retry.RetryOnRollback;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import javax.transaction.Transactional;
import java.util.Optional;

import static com.intel.rsd.nodecomposer.utils.Contracts.requiresNonNull;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_SINGLETON;

@Slf4j
@Component
@Scope(SCOPE_SINGLETON)
class ComputerSystemUpdater {
    private final ComputerSystemDao computerSystemDao;
    private final TrustedModuleMapper trustedModuleMapper;

    @Autowired
    ComputerSystemUpdater(ComputerSystemDao computerSystemDao, TrustedModuleMapper trustedModuleMapper) {
        this.computerSystemDao = computerSystemDao;
        this.trustedModuleMapper = trustedModuleMapper;
    }

    @RetryOnRollback(3)
    @Transactional(REQUIRES_NEW)
    public void updateComputerSystemWithRetry(ODataId computerSystemODataId, ComputerSystemResource computerSystemResource) throws IllegalStateException {
        Optional<ComputerSystem> expectedComputerSystem = computerSystemDao.tryFind(computerSystemODataId);
        if (!expectedComputerSystem.isPresent()) {
            String errorMessage = "ComputerSystem was removed before it could be updated";
            log.info(errorMessage + ", expected ComputerSystem URI: {}", computerSystemODataId);
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
