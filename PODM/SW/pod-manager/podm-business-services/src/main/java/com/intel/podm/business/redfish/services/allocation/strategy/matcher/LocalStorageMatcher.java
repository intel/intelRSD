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

package com.intel.podm.business.redfish.services.allocation.strategy.matcher;

import com.intel.podm.business.entities.redfish.base.LocalStorage;
import com.intel.podm.business.redfish.services.allocation.mappers.localdrive.LocalStorageAllocationMapper;
import com.intel.podm.business.services.redfish.requests.RequestedNode;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.util.List;
import java.util.Map;
import java.util.Objects;
import java.util.Set;

import static org.apache.commons.collections.CollectionUtils.isNotEmpty;

@Dependent
public class LocalStorageMatcher {
    @Inject
    protected LocalStorageAllocationMapper mapper;

    public boolean matches(RequestedNode requestedNode, Set<LocalStorage> availableLocalStorage) {
        List<RequestedNode.LocalDrive> requestedLocalDrives = requestedNode.getLocalDrives();

        if (isNotEmpty(requestedLocalDrives)) {
            return areMatched(requestedLocalDrives, availableLocalStorage);
        }

        return true;
    }

    private boolean areMatched(List<RequestedNode.LocalDrive> requestedDrives, Set<LocalStorage> availableLocalStorage) {
        Map<RequestedNode.LocalDrive, LocalStorage> mapped = mapper.map(requestedDrives, availableLocalStorage);
        return Objects.equals(requestedDrives.size(), mapped.size());
    }
}
