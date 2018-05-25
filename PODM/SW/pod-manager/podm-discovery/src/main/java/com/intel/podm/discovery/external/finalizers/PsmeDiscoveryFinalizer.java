/*
 * Copyright (c) 2015-2018 Intel Corporation
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

package com.intel.podm.discovery.external.finalizers;

import com.intel.podm.business.entities.redfish.Chassis;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.business.entities.redfish.base.ComposableAsset;
import com.intel.podm.business.entities.redfish.base.DiscoverableEntity;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.Set;

import static com.intel.podm.common.types.ServiceType.PSME;
import static com.intel.podm.common.utils.Collections.filterByType;
import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
public class PsmeDiscoveryFinalizer extends ServiceTypeSpecializedDiscoveryFinalizer {
    @Inject
    private ComposableAssetsDiscoveryListener composableAssetsDiscoveryListener;

    @Inject
    private EndpointLinker endpointLinker;

    @Inject
    private ChassisHierarchyMaintainer chassisHierarchyMaintainer;

    public PsmeDiscoveryFinalizer() {
        super(PSME);
    }

    @Override
    @Transactional(MANDATORY)
    public void finalize(Set<DiscoverableEntity> discoveredEntities, ExternalService service) {
        chassisHierarchyMaintainer.maintain(filterByType(discoveredEntities, Chassis.class));
        endpointLinker.linkSystemToRelatedEndpoint(filterByType(discoveredEntities, ComputerSystem.class));
        composableAssetsDiscoveryListener.updateRelatedComposedNodes(filterByType(discoveredEntities, ComposableAsset.class));
    }
}
