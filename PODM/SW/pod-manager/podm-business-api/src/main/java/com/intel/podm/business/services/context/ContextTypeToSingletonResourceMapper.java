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

import java.util.Collections;
import java.util.HashSet;
import java.util.Set;

import static com.intel.podm.business.services.context.ContextType.EVENT_SERVICE;
import static com.intel.podm.business.services.context.ContextType.NETWORK_PROTOCOL;
import static com.intel.podm.business.services.context.ContextType.POWER;
import static com.intel.podm.business.services.context.ContextType.THERMAL;
import static com.intel.podm.common.utils.Contracts.requiresNonNull;

public class ContextTypeToSingletonResourceMapper {
    private static final Set<ContextType> MAPPING;

    static {
        Set<ContextType> set = new HashSet<>();
        set.add(NETWORK_PROTOCOL);
        set.add(POWER);
        set.add(THERMAL);
        set.add(EVENT_SERVICE);

        MAPPING = Collections.unmodifiableSet(set);
    }

    public boolean isSingletonResource(ContextType contextType) {
        requiresNonNull(contextType, "contextType");
        return MAPPING.contains(contextType);
    }
}
