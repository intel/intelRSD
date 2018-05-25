/*
 * Copyright (c) 2015-2018 Intel Corporation
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

import com.intel.podm.business.EntityOperationException;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.client.WebClient;
import com.intel.podm.client.WebClientBuilder;
import com.intel.podm.client.WebClientRequestException;
import com.intel.podm.client.actions.ComputerSystemUpdateRequest;
import com.intel.podm.client.resources.redfish.ComputerSystemResource;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.types.actions.ComputerSystemUpdateDefinition;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.net.URI;

import static com.intel.podm.common.utils.Contracts.requires;
import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
public class ComputerSystemUpdateInvoker {
    @Inject
    private WebClientBuilder webClientBuilder;

    @Inject
    private ComputerSystemUpdater computerSystemUpdater;

    @Inject
    private Logger logger;

    @Transactional(MANDATORY)
    public void updateComputerSystem(ComputerSystem computerSystem, ComputerSystemUpdateDefinition computerSystemUpdateDefinition)
        throws EntityOperationException {
        ExternalService service = computerSystem.getService();
        requires(service != null, "There is no Service associated with selected computer system");

        URI computerSystemUri = computerSystem.getSourceUri();
        ComputerSystemResource computerSystemResource;
        try (WebClient webClient = webClientBuilder.newInstance(service.getBaseUri()).retryable().build()) {
            computerSystemResource = webClient.patchAndRetrieve(computerSystemUri, new ComputerSystemUpdateRequest(computerSystemUpdateDefinition));
        } catch (WebClientRequestException e) {
            String errorMessage = "Update on selected ComputerSystem failed";
            logger.w(errorMessage + " on [ service: {}, path: {} ]", service.getBaseUri(), computerSystemUri);
            throw new EntityOperationException(errorMessage, e);
        }

        computerSystemUpdater.updateComputerSystemWithRetry(computerSystem.getId(), computerSystemResource);
    }
}
