/*
 * Copyright (c) 2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.discovery.handlers;

import com.intel.rsd.nodecomposer.discovery.external.finalizers.ComposedNodeDisableService;
import com.intel.rsd.nodecomposer.persistence.redfish.ComputerSystem;
import com.intel.rsd.nodecomposer.persistence.redfish.Manager;
import com.intel.rsd.nodecomposer.persistence.redfish.StorageService;
import com.intel.rsd.nodecomposer.persistence.redfish.base.ComposableAsset;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import javax.transaction.Transactional;
import java.util.Collection;
import java.util.HashSet;
import java.util.Set;

import static java.util.stream.Collectors.toSet;
import static javax.transaction.Transactional.TxType.MANDATORY;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_PROTOTYPE;

@Slf4j
@Component
@Scope(SCOPE_PROTOTYPE)
public class OnUpdateManagerHandler {

    private static final String OFFLINE = "offline";
    private static final String ONLINE = "online";

    @Autowired
    private ComposedNodeDisableService composedNodeDisableService;

    @Transactional(MANDATORY)
    public void preUpdate(Manager manager) {
        log.info("Updating manager({}, status({}))", manager, manager.getStatus());
        if (manager.isOffline()) {
            updateAssets(manager, OFFLINE, false);
        } else if (manager.isAvailable()) {
            updateAssets(manager, ONLINE, true);
        }
    }

    private void updateAssets(Manager manager, String status, boolean achievable) {
        Set<ComposableAsset> assetsToModify = getComposableResources(manager).stream()
            .filter(asset -> asset.isAchievable() != achievable)
            .collect(toSet());

        if (!assetsToModify.isEmpty()) {
            log.warn("Manager (uri: {}, service uuid: {}) is {}.",
                manager.getRemoteRedfishServiceUri(), manager.getServiceEntryPointUuid(), status);
            assetsToModify.forEach(asset -> asset.setAchievable(achievable));
            if (OFFLINE.equals(status)) {
                composedNodeDisableService.disableFromAssets(assetsToModify);
            }
        }
    }

    private Collection<ComposableAsset> getComposableResources(Manager manager) {
        Collection<ComposableAsset> assets = new HashSet<>();
        assets.addAll(getProcessors(manager));
        assets.addAll(getVolumes(manager));
        assets.addAll(getDrives(manager));
        assets.addAll(getComputerSystems(manager));
        assets.addAll(getEndpoints(manager));

        return assets;
    }

    private Set<ComposableAsset> getProcessors(Manager manager) {
        return getComputerSystems(manager).stream()
            .flatMap(computerSystem -> computerSystem.getProcessors().stream())
            .collect(toSet());
    }

    private Set<ComposableAsset> getDrives(Manager manager) {
        return getStorageServices(manager).stream()
            .flatMap(storageService -> storageService.getDrives().stream())
            .collect(toSet());
    }

    private Set<ComposableAsset> getVolumes(Manager manager) {
        return getStorageServices(manager).stream()
            .flatMap(storageService -> storageService.getVolumes().stream())
            .collect(toSet());
    }

    private Set<ComposableAsset> getEndpoints(Manager manager) {
        Set<ComposableAsset> endpoints = new HashSet<>();

        endpoints.addAll(getStorageEndpoints(manager));
        endpoints.addAll(getProcessorEndpoints(manager));

        return endpoints;
    }

    private Set<ComposableAsset> getStorageEndpoints(Manager manager) {
        return getStorageServices(manager).stream()
            .flatMap(storageService -> storageService.getEndpoints().stream())
            .collect(toSet());
    }

    private Set<ComposableAsset> getProcessorEndpoints(Manager manager) {
        return getComputerSystems(manager).stream()
            .flatMap(computerSystem -> computerSystem.getProcessors().stream())
            .flatMap(processor -> processor.getEndpoints().stream())
            .collect(toSet());
    }

    private Set<StorageService> getStorageServices(Manager manager) {
        return manager.getStorageServices();
    }

    private Set<ComputerSystem> getComputerSystems(Manager manager) {
        return manager.getComputerSystems();
    }
}
