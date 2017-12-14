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

import com.intel.podm.business.BusinessApiException;
import com.intel.podm.business.ContextResolvingException;
import com.intel.podm.business.EntityOperationException;
import com.intel.podm.business.RequestValidationException;
import com.intel.podm.business.ResourceStateMismatchException;
import com.intel.podm.business.entities.redfish.EthernetSwitch;
import com.intel.podm.business.entities.redfish.EthernetSwitchPort;
import com.intel.podm.business.entities.redfish.EthernetSwitchPortVlan;
import com.intel.podm.business.redfish.EntityTreeTraverser;
import com.intel.podm.business.redfish.services.actions.EthernetSwitchPortActionsInvoker;
import com.intel.podm.business.redfish.services.helpers.EthernetSwitchPortActionHelper;
import com.intel.podm.business.redfish.services.helpers.EthernetSwitchPortVlanHelper;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.common.types.actions.EthernetSwitchPortDefinition;
import com.intel.podm.common.types.actions.EthernetSwitchPortRedefinition;
import com.intel.podm.common.types.redfish.RedfishEthernetSwitchPort;

import javax.ejb.Stateless;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.net.URI;
import java.util.Set;

import static com.intel.podm.business.Violations.createWithViolations;
import static com.intel.podm.business.redfish.Contexts.toContext;
import static com.intel.podm.common.types.PortClass.LOGICAL;
import static java.lang.String.format;
import static java.util.stream.Collectors.toSet;
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
    public Context createSwitchPort(Context switchContext, RedfishEthernetSwitchPort requestedSwitchPortCreation) throws BusinessApiException {
        EthernetSwitch currentSwitch = (EthernetSwitch) traverser.traverse(switchContext);

        if (!currentSwitch.isEnabledAndHealthy()) {
            throw new ResourceStateMismatchException("EthernetSwitch should be enabled and healthy in order to invoke actions on it.");
        }

        Set<EthernetSwitchPort> portMembers = switchPortHelper.validateAndGetSwitchPorts(currentSwitch, requestedSwitchPortCreation.getLinks());
        EthernetSwitchPort newSwitchPort = invoker.createSwitchPort(currentSwitch, toSwitchPortDefinition(requestedSwitchPortCreation, portMembers));
        return toContext(newSwitchPort);
    }

    private EthernetSwitchPortDefinition toSwitchPortDefinition(RedfishEthernetSwitchPort switchPortCreation, Set<EthernetSwitchPort> switchPorts) {
        return EthernetSwitchPortDefinition.newBuilder()
            .name(switchPortCreation.getName())
            .portId(switchPortCreation.getPortId())
            .portMode(switchPortCreation.getPortMode())
            .uris(convertSwitchPortsToSourceUris(switchPorts))
            .build();
    }

    @Transactional(REQUIRES_NEW)
    public void updateSwitchPort(Context switchPortContext, RedfishEthernetSwitchPort ethernetSwitchPortModification)
        throws EntityOperationException, ContextResolvingException, RequestValidationException {

        EthernetSwitchPort switchPort = (EthernetSwitchPort) traverser.traverse(switchPortContext);
        RedfishEthernetSwitchPort.Links links = ethernetSwitchPortModification.getLinks();
        Set<EthernetSwitchPort> switchPorts = null;
        EthernetSwitchPortVlan primaryVlan = null;

        if (links != null) {
            switchPorts = switchPortHelper.validateAndGetSwitchPorts(switchPort.getEthernetSwitch(), links);
            primaryVlan = vlanHelper.validateAndGetVlan(switchPort, links.getPrimaryVlan());
        }

        invoker.updateSwitchPort(switchPort, switchPortRedefinition(ethernetSwitchPortModification, switchPorts, primaryVlan));
    }

    private EthernetSwitchPortRedefinition switchPortRedefinition(RedfishEthernetSwitchPort switchPortModification,
                                                                  Set<EthernetSwitchPort> switchPorts,
                                                                  EthernetSwitchPortVlan primaryVlan) {
        return EthernetSwitchPortRedefinition.newBuilder()
            .administrativeState(switchPortModification.getAdministrativeState())
            .linkSpeedMbps(switchPortModification.getLinkSpeedMbps())
            .frameSize(switchPortModification.getFrameSize())
            .autosense(switchPortModification.getAutosense())
            .primaryVlan(primaryVlan != null ? primaryVlan.getSourceUri() : null)
            .uris(convertSwitchPortsToSourceUris(switchPorts))
            .build();
    }

    private Set<URI> convertSwitchPortsToSourceUris(Set<EthernetSwitchPort> switchPorts) {
        if (switchPorts == null) {
            return null;
        }

        return switchPorts.stream().map(EthernetSwitchPort::getSourceUri).collect(toSet());
    }

    @Transactional(REQUIRES_NEW)
    public void deleteSwitchPort(Context switchPortContext) throws BusinessApiException {
        EthernetSwitchPort switchPort = (EthernetSwitchPort) traverser.traverse(switchPortContext);
        if (!LOGICAL.equals(switchPort.getPortClass())) {
            throw new RequestValidationException(createWithViolations(format("Only EthernetSwitchPorts with PortClass '%s' can be deleted.", LOGICAL)));
        }
        invoker.deleteSwitchPort(switchPort);
    }
}
