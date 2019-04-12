/*
 * Copyright (c) 2015-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.composition.allocation.strategy.matcher;

import com.intel.rsd.nodecomposer.business.services.redfish.requests.RequestedNode;
import com.intel.rsd.nodecomposer.composition.allocation.mappers.localdrive.LocalStorageAllocationMapper;
import com.intel.rsd.nodecomposer.persistence.redfish.base.LocalStorage;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import java.util.List;
import java.util.Map;
import java.util.Objects;
import java.util.Set;

import static org.apache.commons.collections.CollectionUtils.isNotEmpty;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_SINGLETON;

@Component
@Scope(SCOPE_SINGLETON)
public class LocalStorageMatcher {
    protected final LocalStorageAllocationMapper mapper;

    @Autowired
    public LocalStorageMatcher(LocalStorageAllocationMapper mapper) {
        this.mapper = mapper;
    }

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
