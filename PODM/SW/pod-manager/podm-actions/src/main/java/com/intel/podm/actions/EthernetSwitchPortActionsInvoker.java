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

import com.intel.podm.business.entities.redfish.EthernetSwitch;
import com.intel.podm.business.entities.redfish.EthernetSwitchPort;
import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.client.api.ExternalServiceApiActionException;
import com.intel.podm.client.api.ExternalServiceApiReaderException;
import com.intel.podm.client.api.actions.EthernetSwitchPortResourceActions;
import com.intel.podm.client.api.actions.EthernetSwitchPortResourceActionsFactory;
import com.intel.podm.client.api.actions.EthernetSwitchPortResourceCreationRequest;
import com.intel.podm.client.api.actions.EthernetSwitchPortResourceModificationRequest;
import com.intel.podm.client.api.actions.EthernetSwitchPortResourceRequestFactory;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.types.actions.EthernetSwitchPortDefinition;
import com.intel.podm.common.types.actions.EthernetSwitchPortRedefinition;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.net.URI;

import static java.net.URI.create;
import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
public class EthernetSwitchPortActionsInvoker {
    @Inject
    private Logger logger;

    @Inject
    private EthernetSwitchPortResourceActionsFactory actionsFactory;

    @Inject
    private EthernetSwitchPortResourceRequestFactory requestFactory;

    @Inject
    private EthernetSwitchPortObtainer ethernetSwitchPortObtainer;

    @Transactional(MANDATORY)
    public EthernetSwitchPort createSwitchPort(EthernetSwitch currentSwitch, EthernetSwitchPortDefinition portDefinition) throws ActionException {
        ExternalService service = currentSwitch.getService();
        if (service == null) {
            throw new IllegalStateException("There is no Service associated with selected switch");
        }

        URI switchPortCollectionUri = create(currentSwitch.getSourceUri() + "/Ports");
        EthernetSwitchPortResourceCreationRequest ethernetSwitchPortResourceCreationRequest = requestFactory.creationRequest(portDefinition);

        try {
            EthernetSwitchPortResourceActions actions = actionsFactory.create(service.getBaseUri());
            URI newSwitchPortUri = actions.createSwitchPort(switchPortCollectionUri, ethernetSwitchPortResourceCreationRequest);
            return ethernetSwitchPortObtainer.discoverPort(currentSwitch.getId(), newSwitchPortUri);
        } catch (ExternalServiceApiActionException e) {
            String errorMessage = "EthernetSwitchPort creation failed on selected switch";
            logger.w(errorMessage + " on [ service: {}, path: {} ]", service.getBaseUri(), switchPortCollectionUri);
            throw new ActionException(errorMessage, e.getErrorResponse(), e);
        } catch (ExternalServiceApiReaderException e) {
            String errorMessage = "EthernetSwitchPort creation was successful, but failed on refreshing selected port";
            logger.i(errorMessage + " on [ service: {}, path: {} ]", service.getBaseUri(), e.getResourceUri());
            throw new ActionException(errorMessage, e.getErrorResponse());
        }
    }

    @Transactional(MANDATORY)
    public void updateSwitchPort(EthernetSwitchPort switchPort, EthernetSwitchPortRedefinition ethernetSwitchPortRedefinition) throws ActionException {
        ExternalService service = switchPort.getService();
        if (service == null) {
            throw new IllegalStateException("There is no ExternalService associated with selected EthernetSwitchPort");
        }

        URI switchPortUri = switchPort.getSourceUri();
        EthernetSwitchPortResourceModificationRequest modificationRequest = requestFactory.modificationRequest(ethernetSwitchPortRedefinition);

        try {
            EthernetSwitchPortResourceActions actions = actionsFactory.create(service.getBaseUri());
            actions.updateSwitchPort(switchPortUri, modificationRequest);
            ethernetSwitchPortObtainer.discoverPort(switchPort.getEthernetSwitch().getId(), switchPortUri);
        } catch (ExternalServiceApiActionException e) {
            String errorMessage = "Modification failed on selected EthernetSwitchPort";
            logger.w(errorMessage + " on [ service: {}, path: {} ]", service.getBaseUri(), switchPortUri);
            throw new ActionException(errorMessage, e.getErrorResponse());
        } catch (ExternalServiceApiReaderException e) {
            String errorMessage = "EthernetSwitchPort modification successful, but failed on refreshing data";
            logger.i(errorMessage + " on [ service: {}, path: {} ]", service.getBaseUri(), switchPortUri);
            throw new ActionException(errorMessage, e.getErrorResponse());
        }
    }

    @Transactional(MANDATORY)
    public void deleteSwitchPort(EthernetSwitchPort switchPort) throws ActionException {
        ExternalService service = switchPort.getService();
        if (service == null) {
            throw new IllegalStateException("There is no ExternalService associated with selected EthernetSwitchPort");
        }

        URI switchPortUri = switchPort.getSourceUri();
        try {
            EthernetSwitchPortResourceActions actions = actionsFactory.create(service.getBaseUri());
            actions.deleteSwitchPort(switchPortUri);
        } catch (ExternalServiceApiActionException e) {
            String errorMessage = "Selected EthernetSwitchPort could not be deleted";
            logger.w(errorMessage + " on [ service: {}, path: {} ]", service.getBaseUri(), switchPortUri);
            throw new ActionException(errorMessage, e.getErrorResponse());
        }
    }
}
