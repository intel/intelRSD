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
import com.intel.podm.business.entities.dao.DiscoverableEntityDao;
import com.intel.podm.business.entities.redfish.EthernetSwitch;
import com.intel.podm.business.entities.redfish.EthernetSwitchPort;
import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.client.WebClient;
import com.intel.podm.client.WebClientBuilder;
import com.intel.podm.client.WebClientRequestException;
import com.intel.podm.client.actions.EthernetSwitchPortResourceCreationRequest;
import com.intel.podm.client.actions.EthernetSwitchPortResourceModificationRequest;
import com.intel.podm.client.actions.EthernetSwitchPortResourceModificationRequest.PriorityFlowControlImpl;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.types.actions.EthernetSwitchPortDefinition;
import com.intel.podm.common.types.actions.EthernetSwitchPortRedefinition;
import com.intel.podm.discovery.external.partial.EthernetSwitchPortObtainer;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.net.URI;

import static com.intel.podm.common.utils.Contracts.requiresNonNull;
import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
public class EthernetSwitchPortActionsInvoker {
    @Inject
    private Logger logger;

    @Inject
    private EthernetSwitchPortObtainer ethernetSwitchPortObtainer;

    @Inject
    private DiscoverableEntityDao discoverableEntityDao;

    @Inject
    private WebClientBuilder webClientBuilder;

    @Transactional(MANDATORY)
    public EthernetSwitchPort createSwitchPort(EthernetSwitch currentSwitch, EthernetSwitchPortDefinition portDefinition) throws EntityOperationException {
        ExternalService service = currentSwitch.getService();
        requiresNonNull(service, "service", "There is no Service associated with selected switch");

        URI switchPortCollectionUri = URI.create(currentSwitch.getSourceUri() + "/Ports");
        URI newSwitchPortUri = performCreateSwitchPortAction(service, requestFromDefinition(portDefinition), switchPortCollectionUri);
        EthernetSwitchPort ethernetSwitchPort = obtainEthernetSwitchPort(service, newSwitchPortUri);

        currentSwitch.addPort(ethernetSwitchPort);
        return ethernetSwitchPort;
    }

    private EthernetSwitchPortResourceCreationRequest requestFromDefinition(EthernetSwitchPortDefinition ethernetSwitchPortDefinition) {
        EthernetSwitchPortResourceCreationRequest request = new EthernetSwitchPortResourceCreationRequest();
        request.setName(ethernetSwitchPortDefinition.getName());
        request.setPortId(ethernetSwitchPortDefinition.getPortId());
        request.setPortMode(ethernetSwitchPortDefinition.getPortMode());
        request.setPortMembers(ethernetSwitchPortDefinition.getUris());
        return request;
    }

    private URI performCreateSwitchPortAction(ExternalService service, EthernetSwitchPortResourceCreationRequest switchPortCreationRequest,
                                              URI switchPortCollectionUri) throws EntityOperationException {
        try (WebClient webClient = webClientBuilder.newInstance(service.getBaseUri()).retryable().build()) {
            return webClient.post(switchPortCollectionUri, switchPortCreationRequest);
        } catch (WebClientRequestException e) {
            String errorMessage = "EthernetSwitchPort creation failed on selected switch";
            logger.w(errorMessage + " on [ service: {}, path: {} ]", service.getBaseUri(), switchPortCollectionUri);
            throw new EntityOperationException(errorMessage, e);
        }
    }

    private EthernetSwitchPort obtainEthernetSwitchPort(ExternalService service, URI newSwitchPortUri) throws EntityOperationException {
        try {
            return ethernetSwitchPortObtainer.discoverPort(service, newSwitchPortUri);
        } catch (WebClientRequestException e) {
            String errorMessage = "EthernetSwitchPort creation was successful, but failed on refreshing selected port";
            logger.w(errorMessage + " on [ service: {}, path: {} ]", service.getBaseUri(), e.getResourceUri());
            throw new EntityOperationException(errorMessage, e);
        }
    }

    @Transactional(MANDATORY)
    public void updateSwitchPort(EthernetSwitchPort switchPort, EthernetSwitchPortRedefinition switchPortRedefinition) throws EntityOperationException {
        ExternalService service = switchPort.getService();
        if (service == null) {
            throw new IllegalStateException("There is no ExternalService associated with selected EthernetSwitchPort");
        }

        URI switchPortUri = switchPort.getSourceUri();
        try (WebClient webClient = webClientBuilder.newInstance(service.getBaseUri()).retryable().build()) {
            webClient.patch(switchPortUri, requestFromRedefinition(switchPortRedefinition));

        } catch (WebClientRequestException e) {
            String errorMessage = "Modification failed on selected EthernetSwitchPort";
            logger.w(errorMessage + " on [ service: {}, path: {} ]", service.getBaseUri(), switchPortUri);
            throw new EntityOperationException(errorMessage, e);
        }

        obtainEthernetSwitchPort(service, switchPortUri);
    }

    private EthernetSwitchPortResourceModificationRequest requestFromRedefinition(EthernetSwitchPortRedefinition ethernetSwitchPortRedefinition) {
        PriorityFlowControlImpl priorityFlowControl = null;

        if (ethernetSwitchPortRedefinition.getPriorityFlowControl() != null) {
            priorityFlowControl = new PriorityFlowControlImpl(
                ethernetSwitchPortRedefinition.getPriorityFlowControl().getEnabled(),
                ethernetSwitchPortRedefinition.getPriorityFlowControl().getEnabledPriorities());
        }

        EthernetSwitchPortResourceModificationRequest request = new EthernetSwitchPortResourceModificationRequest();
        request.setAdministrativeState(ethernetSwitchPortRedefinition.getAdministrativeState());
        request.setLinkSpeed(ethernetSwitchPortRedefinition.getLinkSpeed());
        request.setFrameSize(ethernetSwitchPortRedefinition.getFrameSize());
        request.setAutosense(ethernetSwitchPortRedefinition.getAutosense());
        request.setDcbxState(ethernetSwitchPortRedefinition.getDcbxState());
        request.setLldpEnabled(ethernetSwitchPortRedefinition.getLldpEnabled());
        request.setPriorityFlowControl(priorityFlowControl);
        request.setLinks(ethernetSwitchPortRedefinition.getUris(), ethernetSwitchPortRedefinition.getPrimaryVlan());
        return request;
    }

    @Transactional(MANDATORY)
    public void deleteSwitchPort(EthernetSwitchPort switchPort) throws EntityOperationException {
        ExternalService service = switchPort.getService();
        if (service == null) {
            throw new IllegalStateException("There is no ExternalService associated with selected EthernetSwitchPort");
        }

        URI switchPortUri = switchPort.getSourceUri();
        try (WebClient webClient = webClientBuilder.newInstance(service.getBaseUri()).retryable().build()) {
            webClient.delete(switchPortUri);
        } catch (WebClientRequestException e) {
            String errorMessage = "Selected EthernetSwitchPort could not be deleted";
            logger.w(errorMessage + " on [ service: {}, path: {} ]", service.getBaseUri(), switchPortUri);
            throw new EntityOperationException(errorMessage, e);
        }
        discoverableEntityDao.removeWithConnectedExternalLinks(switchPort);
    }
}
