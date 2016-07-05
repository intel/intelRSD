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

package com.intel.podm.allocation.strategy.matcher;

import com.intel.podm.allocation.mappers.ethernetinterface.EthernetInterfacesAllocationMapper;
import com.intel.podm.business.dto.redfish.RequestedEthernetInterface;
import com.intel.podm.business.dto.redfish.RequestedNode;
import com.intel.podm.business.entities.redfish.EthernetInterface;

import javax.inject.Inject;
import java.util.List;
import java.util.Map;
import java.util.Objects;

import static org.apache.commons.collections.CollectionUtils.isEmpty;

public class EthernetInterfaceMatcher {
    @Inject
    protected EthernetInterfacesAllocationMapper ethernetInterfacesAllocationMapper;

    public boolean matches(RequestedNode requestedNode, List<EthernetInterface> availableInterfaces) {
        List<RequestedEthernetInterface> requestedInterfaces = requestedNode.getEthernetInterfaces();
        boolean ethernetInterfacesAreNotSpecified = isEmpty(requestedInterfaces);

        if (ethernetInterfacesAreNotSpecified) {
            return true;
        }
        return canMapRequestedWithAvailableInterfaces(requestedInterfaces, availableInterfaces);
    }

    private boolean canMapRequestedWithAvailableInterfaces(List<RequestedEthernetInterface> requestedInterfaces,
                                                           List<EthernetInterface> availableInterfaces) {
        Map<EthernetInterface, RequestedEthernetInterface> map
                = ethernetInterfacesAllocationMapper.map(availableInterfaces, requestedInterfaces);

        return Objects.equals(map.size(), requestedInterfaces.size());
    }
}
