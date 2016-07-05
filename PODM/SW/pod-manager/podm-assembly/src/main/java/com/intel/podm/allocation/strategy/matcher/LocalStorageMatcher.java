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

package com.intel.podm.allocation.strategy.matcher;

import com.intel.podm.allocation.mappers.localdrive.LocalStorageAllocationMapper;
import com.intel.podm.business.dto.redfish.RequestedLocalDrive;
import com.intel.podm.business.dto.redfish.RequestedNode;
import com.intel.podm.business.entities.redfish.Adapter;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.SimpleStorage;
import com.intel.podm.business.entities.redfish.base.LocalStorage;

import java.util.Collection;
import java.util.List;
import java.util.Map;
import java.util.Objects;

import static java.util.stream.Collectors.toList;
import static org.apache.commons.collections.CollectionUtils.isNotEmpty;

public class LocalStorageMatcher {
    public boolean matches(RequestedNode requestedNode, ComputerSystem computerSystem) {
        List<LocalStorage> storageUnderComputerSystem = getStorageUnderComputerSystem(computerSystem);
        List<RequestedLocalDrive> requestedLocalDrives = requestedNode.getLocalDrives();

        if (isNotEmpty(requestedLocalDrives)) {
            return areMatched(requestedLocalDrives, storageUnderComputerSystem);
        }

        return true;
    }

    private boolean areMatched(List<RequestedLocalDrive> requestedDrives, List<LocalStorage> availableLocalStorage) {
        LocalStorageAllocationMapper mapper = new LocalStorageAllocationMapper();
        Map<RequestedLocalDrive, LocalStorage> mapped = mapper.map(requestedDrives, availableLocalStorage);
        return Objects.equals(requestedDrives.size(), mapped.size());
    }

    private static List<LocalStorage> getStorageUnderComputerSystem(ComputerSystem computerSystem) {
        List<LocalStorage> localStorage = computerSystem.getAdapters().stream()
                .map(Adapter::getDevices)
                .flatMap(Collection::stream)
                .collect(toList());

        localStorage.addAll(computerSystem.getSimpleStorages().stream()
                .map(SimpleStorage::getDevices)
                .flatMap(Collection::stream)
                .collect(toList()));

        return localStorage;
    }
}
