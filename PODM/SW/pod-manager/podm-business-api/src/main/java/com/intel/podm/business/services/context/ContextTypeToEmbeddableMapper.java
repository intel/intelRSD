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

import static com.intel.podm.business.services.context.ContextType.POWER_CONTROL;
import static com.intel.podm.business.services.context.ContextType.POWER_SUPPLY;
import static com.intel.podm.business.services.context.ContextType.POWER_VOLTAGE;
import static com.intel.podm.business.services.context.ContextType.REDUNDANCY;
import static com.intel.podm.business.services.context.ContextType.THERMAL_FAN;
import static com.intel.podm.business.services.context.ContextType.THERMAL_TEMPERATURE;
import static com.intel.podm.common.utils.Contracts.requiresNonNull;
import static java.util.Collections.unmodifiableSet;

public class ContextTypeToEmbeddableMapper {
    private static final Set<ContextType> MAPPING;

    static {
        Set<ContextType> set = new HashSet<>();
        set.add(POWER_VOLTAGE);
        set.add(POWER_SUPPLY);
        set.add(POWER_CONTROL);
        set.add(THERMAL_TEMPERATURE);
        set.add(THERMAL_FAN);
        set.add(REDUNDANCY);

        MAPPING = unmodifiableSet(set);
    }

    public boolean isEmbeddable(ContextType contextType) {
        requiresNonNull(contextType, "contextType");
        return MAPPING.contains(contextType);
    }
}
