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
import com.intel.rsd.nodecomposer.discovery.external.partial.EthernetSwitchPortObtainer;
import com.intel.rsd.nodecomposer.externalservices.WebClient;
import com.intel.rsd.nodecomposer.externalservices.WebClientBuilder;
import com.intel.rsd.nodecomposer.externalservices.WebClientRequestException;
import com.intel.rsd.nodecomposer.externalservices.actions.EthernetSwitchPortResourceModificationRequest;
import com.intel.rsd.nodecomposer.persistence.dao.GenericDao;
import com.intel.rsd.nodecomposer.persistence.redfish.EthernetSwitchPort;
import com.intel.rsd.nodecomposer.types.actions.EthernetSwitchPortRedefinition;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import javax.transaction.Transactional;
import java.net.URI;

import static javax.transaction.Transactional.TxType.MANDATORY;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_SINGLETON;

@Slf4j
@Component
@Scope(SCOPE_SINGLETON)
public class EthernetSwitchPortActionsInvoker {
    private final EthernetSwitchPortObtainer ethernetSwitchPortObtainer;
    private final WebClientBuilder webClientBuilder;

    @Autowired
    public EthernetSwitchPortActionsInvoker(EthernetSwitchPortObtainer ethernetSwitchPortObtainer, GenericDao genericDao, WebClientBuilder webClientBuilder) {
        this.ethernetSwitchPortObtainer = ethernetSwitchPortObtainer;
        this.webClientBuilder = webClientBuilder;
    }

    @Transactional(MANDATORY)
    public void updateSwitchPort(EthernetSwitchPort switchPort, EthernetSwitchPortRedefinition switchPortRedefinition) throws EntityOperationException {
        URI switchPortUri = switchPort.getUri().toUri();
        try (WebClient webClient = webClientBuilder.createResourceManagerInstance().retryable().build()) {
            webClient.patch(switchPortUri, requestFromRedefinition(switchPortRedefinition));

        } catch (WebClientRequestException e) {
            String errorMessage = "Modification failed on selected EthernetSwitchPort";
            log.warn(errorMessage + " on [ path: {} ]", switchPortUri);
            throw new EntityOperationException(errorMessage, e);
        }

        obtainEthernetSwitchPort(switchPortUri);
    }

    private EthernetSwitchPort obtainEthernetSwitchPort(URI newSwitchPortUri) throws EntityOperationException {
        try {
            return ethernetSwitchPortObtainer.discoverPort(newSwitchPortUri);
        } catch (WebClientRequestException e) {
            String errorMessage = "EthernetSwitchPort creation was successful, but failed on refreshing selected port";
            log.warn(errorMessage + " on [ path: {} ]", e.getResourceUri());
            throw new EntityOperationException(errorMessage, e);
        }
    }

    private EthernetSwitchPortResourceModificationRequest requestFromRedefinition(EthernetSwitchPortRedefinition ethernetSwitchPortRedefinition) {
        EthernetSwitchPortResourceModificationRequest request = new EthernetSwitchPortResourceModificationRequest();
        request.setLinks(ethernetSwitchPortRedefinition.getPrimaryVlan());
        return request;
    }
}
