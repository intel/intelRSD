/*
 * Copyright (c) 2016-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.composition.allocation.strategy;

import com.intel.rsd.nodecomposer.business.dto.redfish.ODataIdPossessor;
import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.business.services.redfish.requests.RequestedNode;
import com.intel.rsd.nodecomposer.composition.allocation.validation.ODataIdToODataIdPossessorTypeMapping;
import com.intel.rsd.nodecomposer.composition.allocation.validation.ODataIdToODataIdPossessorTypeMapping.ODataIdPossessorType;

import java.util.HashSet;
import java.util.List;
import java.util.Objects;
import java.util.Set;

import static com.intel.rsd.nodecomposer.composition.allocation.validation.ODataIdToODataIdPossessorTypeMapping.ODataIdPossessorType.ETHERNET_INTERFACE;
import static com.intel.rsd.nodecomposer.composition.allocation.validation.ODataIdToODataIdPossessorTypeMapping.ODataIdPossessorType.MEMORY;
import static com.intel.rsd.nodecomposer.composition.allocation.validation.ODataIdToODataIdPossessorTypeMapping.ODataIdPossessorType.PROCESSOR;
import static com.intel.rsd.nodecomposer.composition.allocation.validation.ODataIdToODataIdPossessorTypeMapping.ODataIdPossessorType.SIMPLE_STORAGE;
import static java.util.Collections.emptySet;
import static java.util.stream.Collectors.toSet;
import static org.apache.commons.collections.CollectionUtils.isEmpty;

public final class ODataIdPossessorUtils {
    private ODataIdPossessorUtils() {
    }

    public static Set<ODataId> getChassisODataIds(List<? extends ODataIdPossessor> oDataIdPossessors) {
        if (isEmpty(oDataIdPossessors)) {
            return emptySet();
        }

        return oDataIdPossessors.stream()
            .map(ODataIdPossessor::getChassisODataId)
            .filter(Objects::nonNull)
            .collect(toSet());
    }

    public static Set<ODataIdToODataIdPossessorTypeMapping> getResourceODataIds(List<? extends ODataIdPossessor> oDataIdPossessors,
                                                                                ODataIdPossessorType oDataIdPossessorType) {
        if (isEmpty(oDataIdPossessors)) {
            return emptySet();
        }

        return oDataIdPossessors.stream()
            .map(ODataIdPossessor::getResourceODataId)
            .filter(Objects::nonNull)
            .map(oDataId -> new ODataIdToODataIdPossessorTypeMapping(oDataId, oDataIdPossessorType))
            .collect(toSet());
    }

    public static Set<ODataId> getAllChassisODataIds(RequestedNode requestedNode) {
        Set<ODataId> oDataIds = new HashSet<>();
        oDataIds.addAll(getChassisODataIds(requestedNode.getProcessors()));
        oDataIds.addAll(getChassisODataIds(requestedNode.getMemoryModules()));
        oDataIds.addAll(getChassisODataIds(requestedNode.getEthernetInterfaces()));

        return oDataIds;
    }

    public static Set<ODataIdToODataIdPossessorTypeMapping> getAllResourceODataIds(RequestedNode requestedNode) {
        Set<ODataIdToODataIdPossessorTypeMapping> oDataIdToODataIdPossessorTypeMappings = new HashSet<>();
        oDataIdToODataIdPossessorTypeMappings.addAll(getResourceODataIds(requestedNode.getProcessors(), PROCESSOR));
        oDataIdToODataIdPossessorTypeMappings.addAll(getResourceODataIds(requestedNode.getMemoryModules(), MEMORY));
        oDataIdToODataIdPossessorTypeMappings.addAll(getResourceODataIds(requestedNode.getEthernetInterfaces(), ETHERNET_INTERFACE));

        return oDataIdToODataIdPossessorTypeMappings;
    }

    public static Set<ODataIdToODataIdPossessorTypeMapping> getLocalStorageUnderneathComputerSystemResourceODataIds(
        List<? extends ODataIdPossessor> oDataIdPossessors) {
        return getResourceODataIds(oDataIdPossessors, SIMPLE_STORAGE).stream()
            .filter(oDataIdToODataIdPossessorTypeMapping -> oDataIdToODataIdPossessorTypeMapping.getODataId().toString().contains("System"))
            .collect(toSet());
    }
}
