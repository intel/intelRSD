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

package com.intel.podm.business.redfish.services.helpers;

import com.intel.podm.actions.ActionException;
import com.intel.podm.business.EntityNotFoundException;
import com.intel.podm.business.dto.redfish.RequestedEthernetSwitchPortCreation;
import com.intel.podm.business.dto.redfish.RequestedEthernetSwitchPortLinks;
import com.intel.podm.business.dto.redfish.RequestedEthernetSwitchPortModification;
import com.intel.podm.business.entities.redfish.EthernetSwitchPort;
import com.intel.podm.business.redfish.DomainObjectTreeTraverser;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.common.types.Id;
import com.intel.podm.common.types.actions.EthernetSwitchPortDefinition;
import com.intel.podm.common.types.actions.EthernetSwitchPortRedefinition;

import javax.inject.Inject;
import java.net.URI;
import java.util.List;
import java.util.Optional;
import java.util.Set;

import static java.util.Optional.empty;
import static java.util.Optional.of;
import static java.util.stream.Collectors.toList;
import static java.util.stream.Collectors.toSet;

public class EthernetSwitchPortActionHelper {

    @Inject
    private DomainObjectTreeTraverser traverser;

    public Optional<List<EthernetSwitchPort>> validateAndGetSwitchPorts(Id parentId, Optional<RequestedEthernetSwitchPortLinks> requestedSwitchPortLinks)
            throws ActionException {

        Optional<List<EthernetSwitchPort>> switchPorts = convertContextsToSwitchPorts(validateSwitchPortsAndGetContexts(requestedSwitchPortLinks));

        if (shouldCheckSwitchPorts(switchPorts)) {
            Set<Id> parentIds = switchPorts.get().stream().map(switchPort -> switchPort.getEthernetSwitch().getId()).collect(toSet());

            if (parentIds.size() != 1 || !parentId.equals(parentIds.iterator().next())) {
                throw new ActionException("Provided ports don't belong to single EthernetSwitch");
            }

            if (!parentId.equals(parentIds.iterator().next())) {
                throw new ActionException("Provided ports don't belong to proper EthernetSwitch");
            }
        }

        return switchPorts;
    }

    private Optional<List<EthernetSwitchPort>> convertContextsToSwitchPorts(Optional<List<Context>> contextList) throws ActionException {
        if (contextList == null) {
            return empty();
        }

        try {
            return contextList.isPresent() ? of(traverser.traverse(contextList.get())) : empty();
        } catch (EntityNotFoundException e) {
            throw new ActionException("Provided ports contain a non-existing port entry");
        }
    }

    private Optional<List<Context>> validateSwitchPortsAndGetContexts(Optional<RequestedEthernetSwitchPortLinks> links) throws ActionException {
        if (links == null) {
            return null;
        }

        if (!links.isPresent()) {
            return empty();
        }

        if (links.get().getPortMembers() == null) {
            throw new ActionException("SwitchPort action cannot be triggered. No ports provided.");
        }

        if (!links.get().getPortMembers().isPresent()) {
            return empty();
        }

        return links.get().getPortMembers();
    }

    private boolean shouldCheckSwitchPorts(Optional<List<EthernetSwitchPort>> switchPorts) {
        return switchPorts.isPresent() && !switchPorts.get().isEmpty();
    }

    public EthernetSwitchPortDefinition switchPortDefinition(RequestedEthernetSwitchPortCreation switchPortCreation,
                                                     Optional<List<EthernetSwitchPort>> switchPorts) {
        return EthernetSwitchPortDefinition
                .newBuilder()
                .name(switchPortCreation.getName())
                .portId(switchPortCreation.getPortId())
                .portMode(switchPortCreation.getPortMode())
                .uris(convertSwitchPortsToSourceUris(switchPorts))
                .build();
    }

    public EthernetSwitchPortRedefinition switchPortRedefinition(RequestedEthernetSwitchPortModification switchPortModification,
                                                         Optional<List<EthernetSwitchPort>> switchPorts,
                                                         URI primaryVlanUri) {

        return EthernetSwitchPortRedefinition
                .newBuilder()
                .administrativeState(switchPortModification.getAdministrativeState())
                .linkSpeed(switchPortModification.getLinkSpeed())
                .frameSize(switchPortModification.getFrameSize())
                .autosense(switchPortModification.getAutosense())
                .primaryVlan(primaryVlanUri)
                .uris(convertSwitchPortsToSourceUris(switchPorts))
                .build();
    }

    private Optional<List<URI>> convertSwitchPortsToSourceUris(Optional<List<EthernetSwitchPort>> switchPorts) {
        if (switchPorts == null) {
            return null;
        }

        return switchPorts.isPresent()
                ? of(switchPorts.get().stream().map(EthernetSwitchPort::getSourceUri).collect(toList()))
                : empty();
    }
}
