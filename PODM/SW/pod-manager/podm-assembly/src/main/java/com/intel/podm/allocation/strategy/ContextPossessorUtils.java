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

package com.intel.podm.allocation.strategy;

import com.intel.podm.business.dto.redfish.ContextPossessor;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.requests.RequestedNode;

import java.util.HashSet;
import java.util.List;
import java.util.Objects;
import java.util.Set;

import static com.intel.podm.business.services.context.Context.hasParentOfTypeOnTopOf;
import static com.intel.podm.business.services.context.ContextType.COMPUTER_SYSTEM;
import static java.util.Collections.emptySet;
import static java.util.stream.Collectors.toSet;
import static org.apache.commons.collections.CollectionUtils.isEmpty;

public final class ContextPossessorUtils {

    private ContextPossessorUtils() {
    }

    public static Set<Context> getChassisContexts(List<? extends ContextPossessor> contextPossessors) {
        if (isEmpty(contextPossessors)) {
            return emptySet();
        }

        return contextPossessors.stream()
            .map(ContextPossessor::getChassisContext)
            .filter(Objects::nonNull)
            .collect(toSet());
    }

    public static Set<Context> getResourceContexts(List<? extends ContextPossessor> contextPossessors) {
        if (isEmpty(contextPossessors)) {
            return emptySet();
        }

        return contextPossessors.stream()
            .map(ContextPossessor::getResourceContext)
            .filter(Objects::nonNull)
            .collect(toSet());
    }

    public static Set<Context> getAllChassisContexts(RequestedNode requestedNode) {
        Set<Context> contexts = new HashSet<>();
        contexts.addAll(getChassisContexts(requestedNode.getProcessors()));
        contexts.addAll(getChassisContexts(requestedNode.getMemoryModules()));
        contexts.addAll(getChassisContexts(requestedNode.getEthernetInterfaces()));

        return contexts;
    }

    public static Set<Context> getAllResourceContexts(RequestedNode requestedNode) {
        Set<Context> contexts = new HashSet<>();
        contexts.addAll(getResourceContexts(requestedNode.getProcessors()));
        contexts.addAll(getResourceContexts(requestedNode.getMemoryModules()));
        contexts.addAll(getResourceContexts(requestedNode.getEthernetInterfaces()));

        return contexts;
    }

    public static Set<Context> getLocalStorageUnderneathComputerSystemResourceContexts(List<? extends ContextPossessor> contextPossessors) {
        return getResourceContexts(contextPossessors).stream()
            .filter(context -> hasParentOfTypeOnTopOf(context, COMPUTER_SYSTEM))
            .collect(toSet());
    }

}
