/*
 * Copyright (c) 2016-2018 Intel Corporation
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

package com.intel.podm.client;

import com.intel.podm.common.types.redfish.ResourceNames;
import org.testng.annotations.DataProvider;
import org.testng.annotations.Test;

import java.net.URI;
import java.util.Map;

import static com.google.common.collect.ImmutableSet.copyOf;
import static java.util.stream.Collectors.toMap;
import static org.testng.Assert.assertEquals;

public class IdFromUriGeneratorTest {

    @Test(dataProvider = "testGetIdFromUriProvider")
    public void testGetIdFromUri(String uri, String expectedId) throws Exception {
        assertEquals(IdFromUriGenerator.instance().getIdFromUri(URI.create(uri), "SRV_ID").toString(), expectedId);
    }


    /**
     * Test checks if acronym generate method provides unique acronyms for all resources in ResourceNames.
     * Note: "Drives" resource can be used in different context and get the same acronym.
     */
    @Test
    public void checkResourceAcronymsUniqness() throws IllegalAccessException {
        IdFromUriGenerator generator = IdFromUriGenerator.instance();
        Map<String, String> acronyms = ResourceNames.ALL_RESOURCE_NAMES.stream()
            .collect(toMap(resourceName -> resourceName, resourceName -> generator.get(resourceName).apply(resourceName)));

        int acronymsSize = acronyms.size();
        int uniqueAcronymsSize = copyOf(acronyms.values()).size();

        if (acronyms.size() != uniqueAcronymsSize) {
            acronyms.entrySet().
                removeIf(entry -> acronyms.values().stream().filter(a -> a.equals(entry.getValue())).count() == 1);

            acronyms.forEach((from, acronym) -> System.err.println("NOT UNIQUE ACRONYM: " + from + " -> " + acronym));

            if (!acronyms.isEmpty()) {
                System.err.println("Add custom acronym to IRREGULAR_ACRONYMS map in IdFromUriGenerator.");
            }
        }
        assertEquals(acronymsSize, uniqueAcronymsSize);
    }

    @DataProvider
    @SuppressWarnings({"checkstyle:MethodLength"})
    public Object[][] testGetIdFromUriProvider() {
        return new Object[][]{
            {"/redfish/v1/Chassis/sled2/Drives/Drive1", "SRV_ID-c-sled2-d-drive1"},
            {"/redfish/v1/Systems/1/EthernetInterfaces/1/VLANs/1", "SRV_ID-s-1-ei-1-vlan-1"},
            {"/redfish/v1/Chassis/module3/Thermal", "SRV_ID-c-module3-th"},
            {"/redfish/v1/Chassis/drawer1/Power#/Redundancy/0", "SRV_ID-c-drawer1-pw+r-0"},
            {"/redfish/v1/Managers/1", "SRV_ID-m-1"},
            {"/redfish/v1/Chassis/sled2/Power", "SRV_ID-c-sled2-pw"},
            {"/redfish/v1/EthernetSwitches/1/Ports/2", "SRV_ID-es-1-pt-2"},
            {"/redfish/v1/EthernetSwitches/1/Ports/1/VLANs/2", "SRV_ID-es-1-pt-1-vlan-2"},
            {"/redfish/v1/Chassis/drawer1/Power#/PowerSupplies/0", "SRV_ID-c-drawer1-pw+ps-0"},
            {"/redfish/v1/Managers/3/EthernetInterfaces/1", "SRV_ID-m-3-ei-1"},
            {"/redfish/v1/Chassis/drawer1/Thermal", "SRV_ID-c-drawer1-th"},
            {"/redfish/v1/Chassis/drawer1", "SRV_ID-c-drawer1"},
            {"/redfish/v1/Chassis/Rack1-Block1-Sled1/Power", "SRV_ID-c-rack1-block1-sled1-pw"},
            {"/redfish/v1/Chassis/Rack1-PowerBay1/Power#/PowerControl/0", "SRV_ID-c-rack1-powerbay1-pw+pc-0"},
            {"/redfish/v1/Chassis/Rack1/Power#/PowerControl/0", "SRV_ID-c-rack1-pw+pc-0"},
            {"/redfish/v1/Managers/pod-manager/EthernetInterfaces/ens33/VLANs", "SRV_ID-m-pod-manager-ei-ens33-vlan"},
            {"/redfish/v1/Chassis/Rack1-Block1-Sled1", "SRV_ID-c-rack1-block1-sled1"},
            {"/redfish/v1/Fabrics/Fabric1/Switches/Switch1/Ports", "SRV_ID-ff-fabric1-fs-switch1-pt"},
            {"/redfish/v1/EventService/", "SRV_ID-ev"},
            {"/redfish/v1/TelemetryService/", "SRV_ID-ts"},
            {"/redfish/v1/TelemetryService/MetricDefinitions", "SRV_ID-md"},
            {"/redfish/v1/TelemetryService/MetricDefinitions/1", "SRV_ID-md-1"}
        };
    }
}
