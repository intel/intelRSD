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

package com.intel.podm.business.redfish.services.helpers;

import com.intel.podm.business.ContextResolvingException;
import com.intel.podm.business.EntityOperationException;
import com.intel.podm.business.RequestValidationException;
import com.intel.podm.business.entities.redfish.EthernetSwitch;
import com.intel.podm.business.entities.redfish.EthernetSwitchPort;
import com.intel.podm.business.redfish.EntityTreeTraverser;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.common.types.redfish.RedfishEthernetSwitchPort.Links;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.Set;

import static com.intel.podm.business.Violations.createWithViolations;
import static java.util.stream.Collectors.toSet;
import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
public class EthernetSwitchPortActionHelper {
    @Inject
    private EntityTreeTraverser traverser;

    @Transactional(MANDATORY)
    public Set<EthernetSwitchPort> validateAndGetSwitchPorts(EthernetSwitch ethernetSwitch, Links requestedSwitchPortLinks)
        throws EntityOperationException, RequestValidationException {

        Set<EthernetSwitchPort> switchPorts = convertContextsToSwitchPorts(validateSwitchPortsAndGetContexts(requestedSwitchPortLinks));
        if (shouldCheckSwitchPorts(switchPorts)) {
            Set<EthernetSwitch> switches = switchPorts.stream().map(EthernetSwitchPort::getEthernetSwitch).collect(toSet());
            if (switches.size() != 1 || !ethernetSwitch.equals(switches.iterator().next())) {
                throw new EntityOperationException("Provided ports don't belong to single EthernetSwitch");
            }

            if (!ethernetSwitch.equals(switches.iterator().next())) {
                throw new EntityOperationException("Provided ports don't belong to proper EthernetSwitch");
            }
        }
        return switchPorts;
    }

    private Set<EthernetSwitchPort> convertContextsToSwitchPorts(Set<Context> contextList) throws RequestValidationException {
        if (contextList == null) {
            return null;
        }

        try {
            return traverser.traverse(contextList);
        } catch (ContextResolvingException e) {
            throw new RequestValidationException(createWithViolations("Provided ports contain a non-existing port entry"));
        }
    }

    private Set<Context> validateSwitchPortsAndGetContexts(Links links) {
        if (links == null || links.getPortMembers() == null) {
            return null;
        }

        return links.getPortMembers();
    }

    private boolean shouldCheckSwitchPorts(Set<EthernetSwitchPort> switchPorts) {
        return switchPorts != null && !switchPorts.isEmpty();
    }
}
