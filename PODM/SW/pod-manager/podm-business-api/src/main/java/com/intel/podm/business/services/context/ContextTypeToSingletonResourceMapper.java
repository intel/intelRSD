/*
 * Copyright (c) 2016-2017 Intel Corporation
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

package com.intel.podm.business.services.context;

import java.util.HashSet;
import java.util.Set;

import static com.intel.podm.business.services.context.ContextType.COMPUTER_SYSTEM_METRICS;
import static com.intel.podm.business.services.context.ContextType.ETHERNET_SWITCH_METRICS;
import static com.intel.podm.business.services.context.ContextType.ETHERNET_SWITCH_PORT_METRICS;
import static com.intel.podm.business.services.context.ContextType.EVENT_SERVICE;
import static com.intel.podm.business.services.context.ContextType.MEMORY_METRICS;
import static com.intel.podm.business.services.context.ContextType.NETWORK_PROTOCOL;
import static com.intel.podm.business.services.context.ContextType.PORT_METRICS;
import static com.intel.podm.business.services.context.ContextType.POWER;
import static com.intel.podm.business.services.context.ContextType.PROCESSOR_METRICS;
import static com.intel.podm.business.services.context.ContextType.TELEMETRY_SERVICE;
import static com.intel.podm.business.services.context.ContextType.THERMAL;
import static com.intel.podm.common.utils.Contracts.requiresNonNull;
import static java.util.Collections.unmodifiableSet;

class ContextTypeToSingletonResourceMapper {
    private static final Set<ContextType> MAPPING;

    static {
        Set<ContextType> set = new HashSet<>();
        set.add(NETWORK_PROTOCOL);
        set.add(POWER);
        set.add(THERMAL);
        set.add(EVENT_SERVICE);
        set.add(TELEMETRY_SERVICE);
        set.add(COMPUTER_SYSTEM_METRICS);
        set.add(PROCESSOR_METRICS);
        set.add(MEMORY_METRICS);
        set.add(PORT_METRICS);
        set.add(ETHERNET_SWITCH_METRICS);
        set.add(ETHERNET_SWITCH_PORT_METRICS);

        MAPPING = unmodifiableSet(set);
    }

    boolean isSingletonResource(ContextType contextType) {
        requiresNonNull(contextType, "contextType");
        return MAPPING.contains(contextType);
    }
}
