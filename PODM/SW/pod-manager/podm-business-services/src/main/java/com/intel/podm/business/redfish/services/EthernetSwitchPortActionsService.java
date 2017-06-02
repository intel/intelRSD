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

package com.intel.podm.business.redfish.services;

import com.intel.podm.actions.ActionException;
import com.intel.podm.actions.EthernetSwitchPortActionsInvoker;
import com.intel.podm.business.BusinessApiException;
import com.intel.podm.business.ContextResolvingException;
import com.intel.podm.business.EntityOperationException;
import com.intel.podm.business.ResourceStateMismatchException;
import com.intel.podm.business.entities.redfish.EthernetSwitch;
import com.intel.podm.business.entities.redfish.EthernetSwitchPort;
import com.intel.podm.business.entities.redfish.EthernetSwitchPortVlan;
import com.intel.podm.business.redfish.EntityTreeTraverser;
import com.intel.podm.business.redfish.services.helpers.EthernetSwitchPortActionHelper;
import com.intel.podm.business.redfish.services.helpers.EthernetSwitchPortVlanHelper;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.common.types.redfish.RedfishEthernetSwitchPort;

import javax.ejb.Stateless;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.Set;

import static com.intel.podm.business.redfish.Contexts.toContext;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

@Stateless
@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
public class EthernetSwitchPortActionsService {
    @Inject
    private EntityTreeTraverser traverser;

    @Inject
    private EthernetSwitchPortActionsInvoker invoker;

    @Inject
    private EthernetSwitchPortActionHelper switchPortHelper;

    @Inject
    private EthernetSwitchPortVlanHelper vlanHelper;

    @Transactional(REQUIRES_NEW)
    public Context createSwitchPort(Context switchContext, RedfishEthernetSwitchPort requestedEthernetSwitchPortCreation) throws BusinessApiException {
        EthernetSwitch currentSwitch = (EthernetSwitch) traverser.traverse(switchContext);

        if (!currentSwitch.isEnabledAndHealthy()) {
            throw new ResourceStateMismatchException("EthernetSwitch should be enabled and healthy in order to invoke actions on it.");
        }

        try {
            Set<EthernetSwitchPort> portMembers = switchPortHelper.validateAndGetSwitchPorts(
                    currentSwitch,
                    requestedEthernetSwitchPortCreation.getLinks()
            );

            EthernetSwitchPort newSwitchPort = invoker.createSwitchPort(
                    currentSwitch,
                    switchPortHelper.switchPortDefinition(requestedEthernetSwitchPortCreation, portMembers)
            );
            return toContext(newSwitchPort);
        } catch (ActionException e) {
            throw new EntityOperationException("Switch Port creation action could not be completed! " + e.getMessage(), e);
        }
    }

    @Transactional(REQUIRES_NEW)
    public void updateSwitchPort(Context switchPortContext, RedfishEthernetSwitchPort ethernetSwitchPortModification)
            throws EntityOperationException, ContextResolvingException {

        EthernetSwitchPort switchPort = (EthernetSwitchPort) traverser.traverse(switchPortContext);
        RedfishEthernetSwitchPort.Links links = ethernetSwitchPortModification.getLinks();
        Set<EthernetSwitchPort> switchPorts = null;
        EthernetSwitchPortVlan primaryVlan = null;

        try {
            if (links != null) {
                switchPorts = switchPortHelper.validateAndGetSwitchPorts(switchPort.getEthernetSwitch(), links);
                primaryVlan = vlanHelper.validateAndGetVlan(switchPort, links.getPrimaryVlan());
            }

            invoker.updateSwitchPort(switchPort, switchPortHelper.switchPortRedefinition(ethernetSwitchPortModification, switchPorts, primaryVlan));
        } catch (ActionException e) {
            throw new EntityOperationException("Switch Port creation action could not be completed! " + e.getMessage(), e);
        }
    }

    @Transactional(REQUIRES_NEW)
    public void deleteSwitchPort(Context switchPortContext) throws ContextResolvingException, EntityOperationException {
        EthernetSwitchPort switchPort = (EthernetSwitchPort) traverser.traverse(switchPortContext);
        try {
            invoker.deleteSwitchPort(switchPort);
        } catch (ActionException e) {
            throw new EntityOperationException("Switch Port creation action could not be completed! " + e.getMessage(), e);
        }
    }
}
