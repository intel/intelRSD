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

package com.intel.podm.discovery.hooks;

import com.intel.podm.business.entities.base.DomainObject;
import com.intel.podm.business.entities.hooks.OnDeleteHook;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.RemoteTarget;
import com.intel.podm.business.entities.redfish.components.ComposedNode;
import com.intel.podm.business.entities.redfish.components.ComposedNodeUpdater;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.HashMap;
import java.util.Map;
import java.util.function.Function;

import static com.intel.podm.common.enterprise.utils.proxy.Unproxier.unproxy;
import static java.lang.String.format;
import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
@Transactional(MANDATORY)
public class OnDeleteComposedNodeComponentHook implements OnDeleteHook {

    private static final Map<Class, Function> VALID_COMPONENTS = new HashMap<>();

    static {
        registerComponent(ComputerSystem.class, ComputerSystem::getComposedNode);
        registerComponent(RemoteTarget.class, RemoteTarget::getComposedNode);
    }

    @Inject
    private ComposedNodeUpdater composedNodeUpdater;

    @Override
    public void execute(DomainObject domainObject) {
        Class<DomainObject> domainObjectClass = unproxy(domainObject.getClass());
        Function function = VALID_COMPONENTS.get(domainObjectClass);
        if (function == null) {
            throw new IllegalStateException(format("Cannot execute this hook for %s", domainObjectClass));
        }
        ComposedNode composedNode = (ComposedNode) function.apply(domainObject);
        if (composedNode != null) {
            composedNodeUpdater.disableComposedNode(composedNode, "Domain object: " + domainObject + " removal");
        }
    }

    @Override
    public boolean canExecute(DomainObject domainObject) {
        return VALID_COMPONENTS.containsKey(unproxy(domainObject.getClass()));
    }

    private static <X extends DomainObject> void registerComponent(Class<X> componentClass, Function<X, ComposedNode> componentClassProvider) {
        VALID_COMPONENTS.put(componentClass, componentClassProvider);
    }
}
