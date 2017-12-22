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

import com.intel.podm.business.EntityOperationException;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.client.WebClient;
import com.intel.podm.client.WebClientBuilder;
import com.intel.podm.client.WebClientRequestException;
import com.intel.podm.client.actions.ChangeTpmStateRequest;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.types.actions.ChangeTpmStatusUpdateDefinition;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.net.URI;

import static com.intel.podm.common.utils.Contracts.requiresNonNull;
import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
public class ChangeTpmStateActionsInvoker {
    private static final String CHANGE_TPM_STATE_URI = "/Actions/Oem/Intel.Oem.ChangeTPMState";

    @Inject
    private WebClientBuilder webClientBuilder;

    @Inject
    private Logger logger;

    @Transactional(MANDATORY)
    public void changeTpmState(ComputerSystem computerSystem, ChangeTpmStatusUpdateDefinition updateDefinition) throws EntityOperationException {
        ExternalService service = computerSystem.getService();
        requiresNonNull(service, "There is no Service associated with selected computer system");

        URI baseUri = service.getBaseUri();
        String computerSystemActionUri = computerSystem.getSourceUri() + CHANGE_TPM_STATE_URI;
        try (WebClient webClient = webClientBuilder.newInstance(baseUri).retryable().build()) {
            webClient.postNotMonitored(computerSystemActionUri, new ChangeTpmStateRequest(updateDefinition));
        } catch (WebClientRequestException e) {
            String errorMessage = "TPM state update on selected ComputerSystem failed";
            logger.w("{} on [ service: {}, path: {} ]", errorMessage, baseUri, computerSystemActionUri);
            throw new EntityOperationException(errorMessage, e);
        }
    }
}
