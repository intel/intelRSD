/*
 * Copyright (c) 2016-2017 Intel Corporation
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

package com.intel.podm.business.entities;

import com.intel.podm.common.types.Id;

import javax.enterprise.context.Dependent;
import java.net.URI;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.function.Function;

import static com.intel.podm.common.types.Id.id;
import static com.intel.podm.common.types.redfish.ResourceNames.EVENT_SERVICE_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.EVENT_SUBSCRIPTION_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.FABRIC_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.MEMORY_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.PCIE_DEVICES_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.PCIE_DEVICE_FUNCTIONS_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.PORTS_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.POWER_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.STORAGE_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.STORAGE_SERVICES_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.SWITCHES_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.THERMAL_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.THERMAL_TEMPERATURE_RESOURCE_NAME;
import static com.intel.podm.common.utils.Contracts.requiresNonNull;
import static java.util.Collections.unmodifiableMap;
import static java.util.stream.Collectors.joining;

@Dependent
public final class IdFromUriGenerator {

    static final Map<String, Function<String, String>> IRREGULAR_ACRONYMS;

    private static final String SERVICE_AND_ID_SEPARATOR = "-";
    private static final String ID_SEGMENT_SEPARATOR = "-";
    private static final String FRAGMENT_SEPARATOR = "+";

    static {
        Map<String, Function<String, String>> map = new HashMap<>();
        map.put(PCIE_DEVICE_FUNCTIONS_RESOURCE_NAME, acronym -> "fn");
        map.put(MEMORY_RESOURCE_NAME, acronym -> "mm");
        map.put(POWER_RESOURCE_NAME, acronym -> "pw");
        map.put(PORTS_RESOURCE_NAME, acronym -> "pt");
        map.put(STORAGE_SERVICES_RESOURCE_NAME, acronym -> "sv");
        map.put(STORAGE_RESOURCE_NAME, acronym -> "sr");
        map.put(THERMAL_RESOURCE_NAME, acronym -> "th");
        map.put(THERMAL_TEMPERATURE_RESOURCE_NAME, acronym -> "tp");
        map.put(PCIE_DEVICES_RESOURCE_NAME, acronym -> "dv");
        map.put(FABRIC_RESOURCE_NAME, acronym -> "ff");
        map.put(SWITCHES_RESOURCE_NAME, acronym -> "fs");
        map.put(EVENT_SERVICE_RESOURCE_NAME, acronym -> "ev");
        map.put(EVENT_SUBSCRIPTION_RESOURCE_NAME, acronym -> "sb");

        IRREGULAR_ACRONYMS = unmodifiableMap(map);
    }

    public Id getIdFromUri(URI uri, String prefix) {
        StringBuilder sb = new StringBuilder();
        sb.append(prefix).append(SERVICE_AND_ID_SEPARATOR);
        sb.append(processUri(uri.getPath().replace("/redfish/v1/", ""), false));
        sb.append(processUri(uri.getFragment() != null ? uri.getFragment().substring(1) : "", true));
        return id(sb.toString());
    }

    private String processUri(String uri, boolean withFragment) {
        boolean idPart = false;
        List<String> parts = new ArrayList<>();
        for (String part : uri.split("/")) {
            parts.add(idPart ? part.toLowerCase() : get(part).apply(part));
            idPart = !idPart;
        }

        return (!uri.isEmpty() && withFragment ? FRAGMENT_SEPARATOR : "")
            + parts.stream().collect(joining(ID_SEGMENT_SEPARATOR));
    }

    public Function<String, String> get(String part) {
        requiresNonNull(part, "part");
        return IRREGULAR_ACRONYMS.getOrDefault(part, IdFromUriGenerator::shorten);
    }

    private static String shorten(String arg) {
        return arg.replaceAll("[a-z]", "").toLowerCase();
    }
}
