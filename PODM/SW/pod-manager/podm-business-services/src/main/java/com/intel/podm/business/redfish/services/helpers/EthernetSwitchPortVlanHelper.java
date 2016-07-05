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
import com.intel.podm.business.entities.redfish.EthernetSwitchPortVlan;
import com.intel.podm.business.redfish.DomainObjectTreeTraverser;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.common.types.Id;

import javax.inject.Inject;
import java.net.URI;

public class EthernetSwitchPortVlanHelper {
    @Inject
    private DomainObjectTreeTraverser traverser;

    public URI validateAndGetVlanUri(Id parentId, Context vlanContext) throws ActionException {
        if (vlanContext == null) {
            return null;
        }
        EthernetSwitchPortVlan vlan = convertVlanContextToVlan(vlanContext);
        verifyVlanToSwitchPortRelation(parentId, vlan);

        return vlan.getSourceUri();
    }

    private void verifyVlanToSwitchPortRelation(Id parentId, EthernetSwitchPortVlan vlan) throws ActionException {
        if (!parentId.equals(vlan.getSwitchPort().getId())) {
            throw new ActionException("Provided VLAN doesn't belong to proper switch port");
        }
    }

    private EthernetSwitchPortVlan convertVlanContextToVlan(Context vlanContext) throws ActionException {
        try {
            return (EthernetSwitchPortVlan) traverser.traverse(vlanContext);
        } catch (EntityNotFoundException e) {
            throw new ActionException("Provided VLAN was not found in specified context");
        }
    }
}
