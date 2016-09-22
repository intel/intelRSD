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

package com.intel.podm.discovery.external.finalizers.lui;

import com.intel.podm.business.entities.base.DomainObject;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.discovery.external.deep.DeepDiscoveryFinalizer;
import com.intel.podm.discovery.external.finalizers.DiscoveryFinalizer;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.util.Collection;
import java.util.Set;

import static com.intel.podm.common.types.ServiceType.LUI;
import static com.intel.podm.common.types.ServiceType.PSME;
import static com.intel.podm.common.utils.Collections.filterByType;

@Dependent
public class LuiDiscoveryFinalizer extends DiscoveryFinalizer {
    @Inject
    DeepDiscoveryFinalizer deepDiscoveryFinalizer;

    public LuiDiscoveryFinalizer() {
        super(LUI);
    }

    @Override
    public void finalize(Set<DomainObject> discoveredDomainObjects, ExternalService service) {
        finalizeDeepDiscovery(discoveredDomainObjects);
        Collection<ComputerSystem> computerSystems = filterByType(discoveredDomainObjects, ComputerSystem.class);

        if (computerSystems.size() != 1) {
            throw new IllegalStateException("LUI service should have exactly one Computer System resource");
        }

        ExternalService externalService = computerSystems.iterator().next().getService();
        if (externalService == null || !PSME.equals(externalService.getServiceType())) {
            throw new IllegalStateException("ComputerSystem should be associated with single PSME external service");
        }
    }

    private void finalizeDeepDiscovery(Set<DomainObject> discoveredDomainObjects) {
        filterByType(discoveredDomainObjects, ComputerSystem.class).stream()
                .map(DomainObject::getId)
                .forEach(deepDiscoveryFinalizer::finalizeSuccessfulDeepDiscovery);
    }
}
