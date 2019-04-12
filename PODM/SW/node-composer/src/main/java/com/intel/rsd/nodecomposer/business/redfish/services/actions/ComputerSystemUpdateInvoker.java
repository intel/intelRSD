/*
 * Copyright (c) 2015-2019 Intel Corporation
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

import com.intel.rsd.nodecomposer.business.EntityOperationException;
import com.intel.rsd.nodecomposer.externalservices.WebClient;
import com.intel.rsd.nodecomposer.externalservices.WebClientBuilder;
import com.intel.rsd.nodecomposer.externalservices.WebClientRequestException;
import com.intel.rsd.nodecomposer.externalservices.actions.ComputerSystemUpdateRequest;
import com.intel.rsd.nodecomposer.externalservices.resources.redfish.ComputerSystemResource;
import com.intel.rsd.nodecomposer.persistence.redfish.ComputerSystem;
import com.intel.rsd.nodecomposer.types.actions.ComputerSystemUpdateDefinition;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import javax.transaction.Transactional;
import java.net.URI;

import static com.intel.rsd.nodecomposer.business.redfish.ODataIds.toODataId;
import static javax.transaction.Transactional.TxType.MANDATORY;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_SINGLETON;

@Slf4j
@Component
@Scope(SCOPE_SINGLETON)
public class ComputerSystemUpdateInvoker {
    private final WebClientBuilder webClientBuilder;
    private final ComputerSystemUpdater computerSystemUpdater;

    @Autowired
    public ComputerSystemUpdateInvoker(WebClientBuilder webClientBuilder, ComputerSystemUpdater computerSystemUpdater) {
        this.webClientBuilder = webClientBuilder;
        this.computerSystemUpdater = computerSystemUpdater;
    }

    @Transactional(MANDATORY)
    public void updateComputerSystem(ComputerSystem computerSystem, ComputerSystemUpdateDefinition computerSystemUpdateDefinition)
        throws EntityOperationException {

        URI computerSystemUri = computerSystem.getUri().toUri();
        ComputerSystemResource computerSystemResource;
        try (WebClient webClient = webClientBuilder.createResourceManagerInstance().retryable().build()) {
            computerSystemResource = webClient.patchAndRetrieve(computerSystemUri, new ComputerSystemUpdateRequest(computerSystemUpdateDefinition));
        } catch (WebClientRequestException e) {
            String errorMessage = "Update on selected ComputerSystem failed";
            log.warn(errorMessage + " on [ path: {} ]", computerSystemUri);
            throw new EntityOperationException(errorMessage, e);
        }

        computerSystemUpdater.updateComputerSystemWithRetry(toODataId(computerSystem), computerSystemResource);
    }
}
