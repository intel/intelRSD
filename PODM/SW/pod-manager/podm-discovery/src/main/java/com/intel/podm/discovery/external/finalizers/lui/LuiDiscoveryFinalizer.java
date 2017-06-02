/*
 * Copyright (c) 2015-2017 Intel Corporation
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

import com.intel.podm.business.entities.redfish.Chassis;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.Drive;
import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.business.entities.redfish.base.Entity;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.discovery.external.deep.DeepDiscoveryFinalizer;
import com.intel.podm.discovery.external.finalizers.ServiceTypeSpecializedDiscoveryFinalizer;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.util.Collection;
import java.util.Set;

import static com.intel.podm.common.types.ServiceType.LUI;
import static com.intel.podm.common.types.ServiceType.PSME;
import static com.intel.podm.common.utils.Collections.filterByType;
import static com.intel.podm.common.utils.Contracts.checkState;
import static com.intel.podm.common.utils.IterableHelper.single;

@Dependent
public class LuiDiscoveryFinalizer extends ServiceTypeSpecializedDiscoveryFinalizer {
    @Inject
    private DeepDiscoveryFinalizer deepDiscoveryFinalizer;

    @Inject
    private Logger logger;

    public LuiDiscoveryFinalizer() {
        super(LUI);
    }

    @Override
    public void finalize(Set<Entity> discoveredEntities, ExternalService service) {
        finalizeDeepDiscovery(discoveredEntities);
        ComputerSystem computerSystem = retrieveDiscoveredComputerSystem(discoveredEntities);
        logger.i("Finalizing Deep discovery for ComputerSystem: {} [service: {}, path: {}]",
            computerSystem.getId(), service.getBaseUri(), computerSystem.getSourceUri());
        ExternalService externalService = computerSystem.getService();
        if (externalService == null || !PSME.equals(externalService.getServiceType())) {
            throw new IllegalStateException("ComputerSystem should be associated with single PSME external service");
        }

        attachDrivesToChassis(discoveredEntities, computerSystem);
    }

    private static ComputerSystem retrieveDiscoveredComputerSystem(Set<Entity> discoveredEntities) {
        Collection<ComputerSystem> computerSystems = filterByType(discoveredEntities, ComputerSystem.class);
        checkState(computerSystems.size() == 1, "LUI service should have exactly one Computer System resource (" + computerSystems.size() + " found).");

        return computerSystems.iterator().next();
    }

    private static void attachDrivesToChassis(Set<Entity> discoveredEntities, ComputerSystem computerSystem) {
        Chassis chassis = single(computerSystem.getChassis());
        filterByType(discoveredEntities, Drive.class).forEach(drive -> drive.setChassis(chassis));
    }

    private void finalizeDeepDiscovery(Set<Entity> discoveredEntities) {
        filterByType(discoveredEntities, ComputerSystem.class).stream()
            .map(ComputerSystem::getId)
            .forEach(deepDiscoveryFinalizer::finalizeSuccessfulDeepDiscovery);
    }
}
