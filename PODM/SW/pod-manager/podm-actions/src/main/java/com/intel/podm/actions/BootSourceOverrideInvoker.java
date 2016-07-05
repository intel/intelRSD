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

package com.intel.podm.actions;

import com.intel.podm.business.entities.dao.ComputerSystemDao;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.business.entities.redfish.properties.Boot;
import com.intel.podm.client.api.ExternalServiceApiActionException;
import com.intel.podm.client.api.ExternalServiceApiReaderException;
import com.intel.podm.client.api.actions.ComputerSystemResourceActions;
import com.intel.podm.client.api.actions.ComputerSystemResourceActionsFactory;
import com.intel.podm.client.api.resources.redfish.BootObject;
import com.intel.podm.client.api.resources.redfish.ComputerSystemResource;
import com.intel.podm.common.enterprise.utils.retry.NumberOfRetriesOnRollback;
import com.intel.podm.common.enterprise.utils.retry.RetryOnRollbackInterceptor;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.types.BootSourceState;
import com.intel.podm.common.types.BootSourceType;
import com.intel.podm.common.types.Id;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.interceptor.Interceptors;
import javax.transaction.Transactional;
import java.net.URI;
import java.util.Optional;

import static com.google.common.base.Preconditions.checkNotNull;
import static com.google.common.base.Preconditions.checkState;
import static javax.transaction.Transactional.TxType.MANDATORY;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

@Dependent
@Transactional(MANDATORY)
public class BootSourceOverrideInvoker {

    @Inject
    private ComputerSystemResourceActionsFactory computerSystemActionsFactory;

    @Inject
    private BootSourceUpdater bootSourceUpdater;

    @Inject
    private Logger logger;

    public void overrideBootSource(ComputerSystem computerSystem, BootSourceType bootSourceType, BootSourceState bootSourceState)
            throws ActionException {
        ExternalService service = computerSystem.getService();
        checkState(service != null, "There is no Service associated with selected computer system");

        URI baseUri = service.getBaseUri();
        URI computerSystemUri = computerSystem.getSourceUri();

        try {
            ComputerSystemResourceActions actions = computerSystemActionsFactory.create(baseUri);
            ComputerSystemResource computerSystemResource = actions.bootSourceOverride(computerSystemUri, bootSourceType, bootSourceState);
            bootSourceUpdater.updateBootSourceWithRetry(computerSystem.getId(), computerSystemResource);
        } catch (ExternalServiceApiActionException e) {
            String errorMessage = "Set boot order action on selected ComputerSystem failed";
            logger.w(errorMessage + " on [ service: {}, path: {} ]", service.getBaseUri(), computerSystemUri);
            throw new ActionException(errorMessage, e.getErrorResponse(), e);
        } catch (ExternalServiceApiReaderException e) {
            String errorMessage = "Set boot order action was successful, but refreshing selected ComputerSystem failed";
            logger.i(errorMessage + " on [ service: {}, path: {} ]", service.getBaseUri(), computerSystemUri);
            throw new ActionException(errorMessage, e.getErrorResponse(), e);
        }
    }


    @Dependent
    @Interceptors(RetryOnRollbackInterceptor.class)
    private static class BootSourceUpdater {

        @Inject
        private ComputerSystemDao computerSystemDao;

        @Inject
        private Logger logger;

        BootSourceUpdater() {
        }

        @NumberOfRetriesOnRollback(3)
        @Transactional(REQUIRES_NEW)
        public void updateBootSourceWithRetry(Id computerSystemId, ComputerSystemResource computerSystemResource) throws IllegalStateException {
            Optional<ComputerSystem> expectedComputerSystem = computerSystemDao.tryFind(computerSystemId);
            if (!expectedComputerSystem.isPresent()) {
                String errorMessage = "ComputerSystem was removed before it could be updated with new boot source";
                logger.i(errorMessage + ", expected ComputerSystem id: {}", computerSystemId);
                throw new IllegalStateException(errorMessage);
            }
            ComputerSystem computerSystem = expectedComputerSystem.get();
            checkNotNull(computerSystem.getBoot(), "Boot from ComputerSystem cannot be null");
            checkNotNull(computerSystemResource, "ComputerSystemResource cannot be null.");
            checkNotNull(computerSystemResource.getBoot(), "Boot from ComputerSystemResource cannot be null");

            Boot boot = computerSystem.getBoot();
            BootObject updatedBoot = computerSystemResource.getBoot();
            boot.setBootSourceOverrideTarget(updatedBoot.getBootSourceOverrideTarget());
            boot.setBootSourceOverrideEnabled(updatedBoot.getBootSourceOverrideEnabled());
        }
    }
}
