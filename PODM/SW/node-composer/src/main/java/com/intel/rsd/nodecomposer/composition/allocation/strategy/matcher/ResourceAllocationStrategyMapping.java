/*
 * Copyright (c) 2017-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.composition.allocation.strategy.matcher;

import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.composition.allocation.strategy.EndpointAllocationStrategy;
import com.intel.rsd.nodecomposer.composition.allocation.strategy.ResourceAllocationStrategy;
import com.intel.rsd.nodecomposer.composition.allocation.strategy.VolumeAllocationStrategy;
import com.intel.rsd.nodecomposer.types.AttachableType;

import java.util.HashMap;
import java.util.Map;

import static com.intel.rsd.nodecomposer.types.AttachableType.ENDPOINT;
import static com.intel.rsd.nodecomposer.types.AttachableType.VOLUME;
import static com.intel.rsd.nodecomposer.types.redfish.ResourceNames.ENDPOINTS_RESOURCE_NAME;
import static com.intel.rsd.nodecomposer.types.redfish.ResourceNames.VOLUMES_RESOURCE_NAME;
import static com.intel.rsd.nodecomposer.utils.Contracts.requiresNonNull;
import static java.lang.String.format;
import static java.util.Collections.unmodifiableMap;

public final class ResourceAllocationStrategyMapping {
    private static final Map<AttachableType, Class<? extends ResourceAllocationStrategy>> RESOURCE_TO_STRATEGY_MAPPING;

    static {
        Map<AttachableType, Class<? extends ResourceAllocationStrategy>> map = new HashMap<>();
        map.put(VOLUME, VolumeAllocationStrategy.class);
        map.put(ENDPOINT, EndpointAllocationStrategy.class);

        RESOURCE_TO_STRATEGY_MAPPING = unmodifiableMap(map);
    }

    private ResourceAllocationStrategyMapping() {
    }

    //TODO: pzak - find a better way
    public static Class<? extends ResourceAllocationStrategy> getStrategyForResource(ODataId oDataId) {
        requiresNonNull(oDataId, "oDataId");

        Class<? extends ResourceAllocationStrategy> resource = null;
        String oDataIdString = oDataId.toString();
        if (oDataIdString.contains(ENDPOINTS_RESOURCE_NAME)) {
            resource = RESOURCE_TO_STRATEGY_MAPPING.get(ENDPOINT);
        } else if (oDataIdString.contains(VOLUMES_RESOURCE_NAME)) {
            resource = RESOURCE_TO_STRATEGY_MAPPING.get(VOLUME);
        }

        if (resource == null) {
            throw new UnsupportedOperationException(format("ContextType %s has no allocation strategy associated with it.", oDataId));
        }

        return resource;
    }
}
