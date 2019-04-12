/*
 * Copyright (c) 2016-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.business.redfish;

import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.persistence.redfish.Drive;
import com.intel.rsd.nodecomposer.persistence.redfish.EthernetInterface;
import com.intel.rsd.nodecomposer.persistence.redfish.Memory;
import com.intel.rsd.nodecomposer.persistence.redfish.Processor;
import com.intel.rsd.nodecomposer.persistence.redfish.SimpleStorage;
import com.intel.rsd.nodecomposer.persistence.redfish.Volume;

import java.util.Objects;
import java.util.Set;
import java.util.function.Function;

import static java.util.Collections.emptySet;
import static java.util.stream.Collectors.toSet;
import static java.util.stream.StreamSupport.stream;

public final class ODataIdCollections {
    private ODataIdCollections() {
    }

    public static Set<ODataId> asSimpleStorageODataIds(Set<SimpleStorage> simpleStorages) {
        return transform(simpleStorages, ODataIds::toODataId);
    }

    public static Set<ODataId> asMemoryODataIds(Set<Memory> memoryModules) {
        return transform(memoryModules, ODataIds::toODataId);
    }

    public static Set<ODataId> asProcessorODataIds(Set<Processor> processors) {
        return transform(processors, ODataIds::toODataId);
    }

    public static Set<ODataId> asEthernetInterfaceODataIds(Set<EthernetInterface> ethernetInterfaces) {
        return transform(ethernetInterfaces, ODataIds::toODataId);
    }

    public static Set<ODataId> asDriveODataIds(Set<Drive> drives) {
        return transform(drives, ODataIds::toODataId);
    }

    public static Set<ODataId> asVolumeODataIds(Set<Volume> volumes) {
        return transform(volumes, ODataIds::toODataId);
    }

    private static <T, R> Set<R> transform(Iterable<T> items, Function<T, R> transformFunction) {
        if (items == null) {
            return emptySet();
        }

        return stream(items.spliterator(), false)
            .map(transformFunction)
            .filter(Objects::nonNull)
            .collect(toSet());
    }
}
