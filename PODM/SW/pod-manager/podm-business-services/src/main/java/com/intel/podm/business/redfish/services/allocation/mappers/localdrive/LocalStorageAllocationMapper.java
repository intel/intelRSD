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

package com.intel.podm.business.redfish.services.allocation.mappers.localdrive;

import com.intel.podm.business.entities.redfish.base.LocalStorage;
import com.intel.podm.business.redfish.services.allocation.strategy.matcher.FilteringCollection;
import com.intel.podm.business.services.redfish.requests.RequestedNode;
import com.intel.podm.common.logger.Logger;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.util.Collection;
import java.util.Comparator;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Optional;

import static com.intel.podm.business.redfish.services.allocation.mappers.Sorter.ascendingWhereNullsLast;
import static com.intel.podm.business.redfish.services.allocation.mappers.Sorter.contextsAscendingWhereNullsLast;
import static com.intel.podm.business.redfish.services.allocation.mappers.Sorter.sort;
import static com.intel.podm.business.redfish.services.allocation.mappers.localdrive.Predicates.byAtLeastCapacityGib;
import static com.intel.podm.business.redfish.services.allocation.mappers.localdrive.Predicates.byAtLeastMinRpm;
import static com.intel.podm.business.redfish.services.allocation.mappers.localdrive.Predicates.byChassisContext;
import static com.intel.podm.business.redfish.services.allocation.mappers.localdrive.Predicates.byExactId;
import static com.intel.podm.business.redfish.services.allocation.mappers.localdrive.Predicates.byExactInterface;
import static com.intel.podm.business.redfish.services.allocation.mappers.localdrive.Predicates.byExactSerialNumber;
import static com.intel.podm.business.redfish.services.allocation.mappers.localdrive.Predicates.byExactType;
import static com.intel.podm.business.redfish.services.allocation.mappers.localdrive.Predicates.isFromFabricSwitch;
import static com.intel.podm.common.types.EnumeratedType.byPriority;
import static java.util.Comparator.comparing;
import static java.util.Comparator.naturalOrder;
import static java.util.Comparator.nullsFirst;

@Dependent
public class LocalStorageAllocationMapper {

    @Inject
    private Logger logger;

    public Map<RequestedNode.LocalDrive, LocalStorage> map(List<RequestedNode.LocalDrive> requestedDrives, Collection<LocalStorage> availableLocalStorage) {
        Map<RequestedNode.LocalDrive, LocalStorage> mapped = new HashMap<>();

        Collection<RequestedNode.LocalDrive> requestedDrivesQueue =
            sort(requestedDrives, contextsAscendingWhereNullsLast());
        Collection<LocalStorage> availableLocalStorageQueue =
            sort(availableLocalStorage, ascendingWhereNullsLast(LocalStorage::getId));

        for (RequestedNode.LocalDrive requestedDrive : requestedDrivesQueue) {
            Optional<LocalStorage> localStorage = findMatchingAvailableDrive(requestedDrive, availableLocalStorageQueue);
            localStorage.ifPresent(storage -> mapped.put(requestedDrive, storage));
            localStorage.ifPresent(availableLocalStorageQueue::remove);
        }
        return mapped;
    }

    private Optional<LocalStorage> findMatchingAvailableDrive(RequestedNode.LocalDrive requestedDrive,
                                                              Collection<LocalStorage> availableLocalStorage) {

        FilteringCollection<LocalStorage> filteringCollection = new FilteringCollection<>(availableLocalStorage);
        filteringCollection
            .filter(byExactId(requestedDrive.getResourceContext()), "byExactId")
            .filter(byAtLeastCapacityGib(requestedDrive.getCapacityGib()), "byAtLeastCapacityGib")
            .filter(byExactType(requestedDrive.getType()), "byExactType")
            .filter(byAtLeastMinRpm(requestedDrive.getMinRpm()), "byAtLeastMinRpm")
            .filter(byExactSerialNumber(requestedDrive.getSerialNumber()), "byExactSerialNumber")
            .filter(byExactInterface(requestedDrive.getInterface()), "byExactInterface")
            .filter(isFromFabricSwitch(requestedDrive.isFromFabricSwitch()), "isFromFabricSwitch")
            .filter(byChassisContext(requestedDrive.getChassisContext()), "byChassisContext");

        if (filteringCollection.getCollection().isEmpty()) {
            logger.d("Cannot find available drive: {}", filteringCollection.getFilterStatistics());
        }

        return filteringCollection.getCollection().stream().sorted(withDriveComparator()).findFirst();
    }

    private static Comparator<LocalStorage> withDriveComparator() {
        return comparing(LocalStorage::getType, nullsFirst(byPriority()))
            .thenComparing(LocalStorage::getCapacityGib, nullsFirst(naturalOrder()))
            .thenComparing(LocalStorage::getRpm, nullsFirst(naturalOrder()))
            .thenComparing(LocalStorage::getSerialNumber, nullsFirst(naturalOrder()))
            .thenComparing(LocalStorage::getProtocol, nullsFirst(byPriority()));
    }
}
