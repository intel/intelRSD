/*
 * Copyright (c) 2017-2018 Intel Corporation
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

package com.intel.podm.business.redfish.services.allocation.strategy.matcher;

import com.intel.podm.business.redfish.services.allocation.strategy.EndpointAllocationStrategy;
import com.intel.podm.business.redfish.services.allocation.strategy.ResourceAllocationStrategy;
import com.intel.podm.business.redfish.services.allocation.strategy.VolumeAllocationStrategy;
import com.intel.podm.business.services.context.ContextType;

import java.util.HashMap;
import java.util.Map;

import static com.intel.podm.business.services.context.ContextType.ENDPOINT;
import static com.intel.podm.business.services.context.ContextType.VOLUME;
import static com.intel.podm.common.utils.Contracts.requiresNonNull;
import static java.lang.String.format;
import static java.util.Collections.unmodifiableMap;

public final class ResourceAllocationStrategyMapping {
    private static final Map<ContextType, Class<? extends ResourceAllocationStrategy>> RESOURCE_TO_STRATEGY_MAPPING;

    static {
        Map<ContextType, Class<? extends ResourceAllocationStrategy>> map = new HashMap<>();
        map.put(VOLUME, VolumeAllocationStrategy.class);
        map.put(ENDPOINT, EndpointAllocationStrategy.class);

        RESOURCE_TO_STRATEGY_MAPPING = unmodifiableMap(map);
    }

    private ResourceAllocationStrategyMapping() {
    }

    public static Class<? extends ResourceAllocationStrategy> getStrategyForResource(ContextType contextType) {
        requiresNonNull(contextType, "contextType");

        Class<? extends ResourceAllocationStrategy> resource = RESOURCE_TO_STRATEGY_MAPPING.get(contextType);
        if (resource == null) {
            throw new UnsupportedOperationException(format("ContextType %s has no allocation strategy associated with it.", contextType));
        }

        return resource;
    }
}
