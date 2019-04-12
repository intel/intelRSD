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

package com.intel.rsd.nodecomposer.composition.allocation.mappers.localdrive;

import com.intel.rsd.nodecomposer.business.services.redfish.requests.RequestedNode;
import com.intel.rsd.nodecomposer.composition.allocation.strategy.matcher.FilteringCollection;
import com.intel.rsd.nodecomposer.persistence.redfish.base.LocalStorage;
import lombok.extern.slf4j.Slf4j;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import java.util.Collection;
import java.util.Comparator;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Optional;

import static com.intel.rsd.nodecomposer.composition.allocation.mappers.Sorter.ascendingWhereNullsLast;
import static com.intel.rsd.nodecomposer.composition.allocation.mappers.Sorter.sort;
import static com.intel.rsd.nodecomposer.composition.allocation.mappers.Sorter.urisAscendingWhereNullsLast;
import static com.intel.rsd.nodecomposer.composition.allocation.mappers.localdrive.Predicates.byAtLeastCapacityGib;
import static com.intel.rsd.nodecomposer.composition.allocation.mappers.localdrive.Predicates.byAtLeastMinRpm;
import static com.intel.rsd.nodecomposer.composition.allocation.mappers.localdrive.Predicates.byChassisUri;
import static com.intel.rsd.nodecomposer.composition.allocation.mappers.localdrive.Predicates.byExactInterface;
import static com.intel.rsd.nodecomposer.composition.allocation.mappers.localdrive.Predicates.byExactSerialNumber;
import static com.intel.rsd.nodecomposer.composition.allocation.mappers.localdrive.Predicates.byExactType;
import static com.intel.rsd.nodecomposer.composition.allocation.mappers.localdrive.Predicates.byExactUri;
import static com.intel.rsd.nodecomposer.composition.allocation.mappers.localdrive.Predicates.isFromFabricSwitch;
import static com.intel.rsd.nodecomposer.types.EnumeratedType.byPriority;
import static java.util.Comparator.comparing;
import static java.util.Comparator.naturalOrder;
import static java.util.Comparator.nullsFirst;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_SINGLETON;

@Slf4j
@Component
@Scope(SCOPE_SINGLETON)
public class LocalStorageAllocationMapper {
    private static Comparator<LocalStorage> withDriveComparator() {
        return comparing(LocalStorage::getType, nullsFirst(byPriority()))
            .thenComparing(LocalStorage::getCapacityGib, nullsFirst(naturalOrder()))
            .thenComparing(LocalStorage::getRpm, nullsFirst(naturalOrder()))
            .thenComparing(LocalStorage::getSerialNumber, nullsFirst(naturalOrder()))
            .thenComparing(LocalStorage::getProtocol, nullsFirst(byPriority()));
    }

    public Map<RequestedNode.LocalDrive, LocalStorage> map(List<RequestedNode.LocalDrive> requestedDrives, Collection<LocalStorage> availableLocalStorage) {
        Map<RequestedNode.LocalDrive, LocalStorage> mapped = new HashMap<>();

        Collection<RequestedNode.LocalDrive> requestedDrivesQueue =
            sort(requestedDrives, urisAscendingWhereNullsLast());
        Collection<LocalStorage> availableLocalStorageQueue =
            sort(availableLocalStorage, ascendingWhereNullsLast(LocalStorage::getUri));

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
            .filter(byExactUri(requestedDrive.getResourceODataId()), "byExactUri")
            .filter(byAtLeastCapacityGib(requestedDrive.getCapacityGib()), "byAtLeastCapacityGib")
            .filter(byExactType(requestedDrive.getType()), "byExactType")
            .filter(byAtLeastMinRpm(requestedDrive.getMinRpm()), "byAtLeastMinRpm")
            .filter(byExactSerialNumber(requestedDrive.getSerialNumber()), "byExactSerialNumber")
            .filter(byExactInterface(requestedDrive.getInterface()), "byExactInterface")
            .filter(isFromFabricSwitch(requestedDrive.isFromFabricSwitch()), "isFromFabricSwitch")
            .filter(byChassisUri(requestedDrive.getChassisODataId()), "byChassisUri");

        if (filteringCollection.getCollection().isEmpty()) {
            log.debug("Cannot find available drive: {}", filteringCollection.getFilterStatistics());
        }

        return filteringCollection.getCollection().stream().sorted(withDriveComparator()).findFirst();
    }
}
