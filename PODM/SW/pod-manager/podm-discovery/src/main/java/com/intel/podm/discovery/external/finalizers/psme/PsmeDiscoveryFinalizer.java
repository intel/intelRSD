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

package com.intel.podm.discovery.external.finalizers.psme;

import com.intel.podm.business.entities.base.DomainObject;
import com.intel.podm.business.entities.redfish.Chassis;
import com.intel.podm.business.entities.redfish.EthernetInterface;
import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.business.entities.redfish.components.ComposedNodeUpdater;
import com.intel.podm.discovery.external.StorageGuard;
import com.intel.podm.discovery.external.finalizers.DiscoveryFinalizer;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.util.Collection;
import java.util.Objects;
import java.util.Set;

import static com.intel.podm.common.types.ChassisType.DRAWER;
import static com.intel.podm.common.types.ChassisType.RACK;
import static com.intel.podm.common.types.ServiceType.PSME;
import static com.intel.podm.common.utils.Collections.filterByType;
import static java.util.Objects.isNull;
import static java.util.stream.Collectors.toSet;

@Dependent
public class PsmeDiscoveryFinalizer extends DiscoveryFinalizer {
    @Inject
    private StorageGuard storageGuard;

    @Inject
    private ChassisLinker chassisLinker;

    @Inject
    private ComposedNodeUpdater composedNodeUpdater;

    @Inject
    private DrawerChassisLocationGuard drawerLocationGuard;

    public PsmeDiscoveryFinalizer() {
        super(PSME);
    }

    @Override
    public void finalize(Set<DomainObject> discoveredDomainObjects, ExternalService service) {
        linkToDomainModel(discoveredDomainObjects);
        protectStorageComputerSystems(discoveredDomainObjects);
        assureSingularParentForDrawers(discoveredDomainObjects);
        composedNodeUpdater.updateRelatedComposedNodes(discoveredDomainObjects);
    }

    Collection<Chassis> getTopLevelChassis(Set<DomainObject> discoveredDomainObjects) {
        return filterByType(discoveredDomainObjects, Chassis.class).stream()
            .filter(chassis -> chassis.is(DRAWER) || chassis.is(RACK))
            .filter(chassis ->
                chassis.getContainedBy() == null
                || !Objects.equals(chassis.getContainedBy().getService(), chassis.getService())
            )
            .collect(toSet());
    }

    private void linkToDomainModel(Set<DomainObject> discoveredDomainObjects) {
        Collection<Chassis> topLevelChassis = getTopLevelChassis(discoveredDomainObjects);
        topLevelChassis.forEach(chassisLinker::linkToDomainModel);
    }

    private void protectStorageComputerSystems(Set<DomainObject> discoveredDomainObjects) {
        filterByType(discoveredDomainObjects, EthernetInterface.class).stream()
                .filter(this::byAssociatedComputerSystemsThatNeedsProtection)
                .map(EthernetInterface::getComputerSystem)
                .filter(computerSystem -> !isNull(computerSystem))
                .forEach(computerSystem -> {
                    storageGuard.associateComputerSystemWithStorage(computerSystem);
                });
    }

    private void assureSingularParentForDrawers(Set<DomainObject> discoveredDomainObjects) {
        filterByType(discoveredDomainObjects, Chassis.class).stream()
                .filter(chassis -> chassis.is(DRAWER))
                .forEach(drawerLocationGuard::assureSingleRackParent);
    }

    private boolean byAssociatedComputerSystemsThatNeedsProtection(EthernetInterface ethernetInterface) {
        return ethernetInterface.getMacAddress() != null && storageGuard.isAssociatedWithProtectedStorage(ethernetInterface.getMacAddress());
    }

}
