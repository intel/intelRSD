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

package com.intel.podm.business.redfish.services.helpers;

import com.intel.podm.actions.ActionException;
import com.intel.podm.business.ContextResolvingException;
import com.intel.podm.business.entities.redfish.EthernetSwitch;
import com.intel.podm.business.entities.redfish.EthernetSwitchPort;
import com.intel.podm.business.entities.redfish.EthernetSwitchPortVlan;
import com.intel.podm.business.redfish.EntityTreeTraverser;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.common.types.actions.EthernetSwitchPortDefinition;
import com.intel.podm.common.types.actions.EthernetSwitchPortRedefinition;
import com.intel.podm.common.types.redfish.RedfishEthernetSwitchPort;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.net.URI;
import java.util.Set;

import static java.util.stream.Collectors.toSet;
import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
public class EthernetSwitchPortActionHelper {
    @Inject
    private EntityTreeTraverser traverser;

    @Transactional(MANDATORY)
    public Set<EthernetSwitchPort> validateAndGetSwitchPorts(EthernetSwitch ethernetSwitch,
                                                              RedfishEthernetSwitchPort.Links requestedSwitchPortLinks)
            throws ActionException {

        Set<EthernetSwitchPort> switchPorts = convertContextsToSwitchPorts(validateSwitchPortsAndGetContexts(requestedSwitchPortLinks));

        if (shouldCheckSwitchPorts(switchPorts)) {
            Set<EthernetSwitch> switches = switchPorts.stream().map(EthernetSwitchPort::getEthernetSwitch).collect(toSet());

            if (switches.size() != 1 || !ethernetSwitch.equals(switches.iterator().next())) {
                throw new ActionException("Provided ports don't belong to single EthernetSwitch");
            }

            if (!ethernetSwitch.equals(switches.iterator().next())) {
                throw new ActionException("Provided ports don't belong to proper EthernetSwitch");
            }
        }

        return switchPorts;
    }

    private Set<EthernetSwitchPort> convertContextsToSwitchPorts(Set<Context> contextList) throws ActionException {
        if (contextList == null) {
            return null;
        }

        try {
            return traverser.traverse(contextList);
        } catch (ContextResolvingException e) {
            throw new ActionException("Provided ports contain a non-existing port entry");
        }
    }

    private Set<Context> validateSwitchPortsAndGetContexts(RedfishEthernetSwitchPort.Links links) throws ActionException {
        if (links == null || links.getPortMembers() == null) {
            return null;
        }

        return links.getPortMembers();
    }

    private boolean shouldCheckSwitchPorts(Set<EthernetSwitchPort> switchPorts) {
        return switchPorts != null && !switchPorts.isEmpty();
    }

    public EthernetSwitchPortDefinition switchPortDefinition(RedfishEthernetSwitchPort switchPortCreation,
                                                     Set<EthernetSwitchPort> switchPorts) {
        return EthernetSwitchPortDefinition
                .newBuilder()
                .name(switchPortCreation.getName())
                .portId(switchPortCreation.getPortId())
                .portMode(switchPortCreation.getPortMode())
                .uris(convertSwitchPortsToSourceUris(switchPorts))
                .build();
    }

    public EthernetSwitchPortRedefinition switchPortRedefinition(RedfishEthernetSwitchPort switchPortModification,
                                                         Set<EthernetSwitchPort> switchPorts,
                                                         EthernetSwitchPortVlan primaryVlan) {
        return EthernetSwitchPortRedefinition
                .newBuilder()
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
}
