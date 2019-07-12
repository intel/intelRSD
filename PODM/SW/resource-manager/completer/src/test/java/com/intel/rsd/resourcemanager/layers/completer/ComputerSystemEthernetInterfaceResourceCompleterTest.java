/*
 * Copyright (c) 2017-2019 Intel Corporation
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

package com.intel.rsd.resourcemanager.layers.completer;

import com.fasterxml.jackson.databind.node.ArrayNode;
import com.google.common.collect.ImmutableSet;
import lombok.val;
import org.testng.annotations.Test;

import java.util.HashMap;

import static com.intel.rsd.json.JsonUtils.createArrayNodeWith;
import static com.intel.rsd.json.JsonUtils.readJsonFromFile;
import static com.intel.rsd.redfish.ExtendedInfo.ExtendedInfoBuilder.newExtendedInfoBuilder;
import static java.util.Collections.singleton;
import static org.assertj.core.api.AssertionsForClassTypes.assertThat;
import static org.testng.Assert.assertFalse;
import static org.testng.Assert.assertTrue;

public class ComputerSystemEthernetInterfaceResourceCompleterTest {
    @Test
    public void whenGivenObjectNodeRepresentingEthernetInterfaceOfComputerSystem_thenShouldBeAbleToComplete() throws Exception {
        val sut = new ComputerSystemEthernetInterfaceResourceCompleter(new HashMap<>());
        boolean result = sut.canCompleteResource(readJsonFromFile(this, "ComputerSystemEthernetInterface.json"));
        assertTrue(result);
    }

    @Test
    public void whenGivenObjectNodeRepresentingEthernetInterfaceOfManager_thenShouldNotBeAbleToComplete() throws Exception {
        val sut = new ComputerSystemEthernetInterfaceResourceCompleter(new HashMap<>());
        boolean result = sut.canCompleteResource(readJsonFromFile(this, "ManagerEthernetInterface.json"));
        assertFalse(result);
    }

    @Test
    public void whenGivenObjectNodeRepresentingProcessorOfComputerSystem_thenShouldNotBeAbleToComplete() throws Exception {
        val sut = new ComputerSystemEthernetInterfaceResourceCompleter(new HashMap<>());
        boolean result = sut.canCompleteResource(readJsonFromFile(this, "Processor.json"));
        assertFalse(result);
    }

    @Test
    public void whenGivenEthernetInterfaceLinkedToEthernetSwitchPort_thenCompleteResponseWithLinkToSwitchPort() throws Exception {
        val sut = new ComputerSystemEthernetInterfaceResourceCompleter(new HashMap<>());
        val mac = "AA:BB:CC:DD:EE:FF";
        val ethernetSwitchPort = readJsonFromFile(this, "EthernetSwitchPort.json").put("NeighborMAC", mac);

        sut.onResourcesCrawled(singleton(new ResourceImpl(ethernetSwitchPort)));

        val computerSystemEthernetInterface = readJsonFromFile(this, "ComputerSystemEthernetInterface.json").put("MACAddress", mac);
        val result = sut.completeResource(computerSystemEthernetInterface);

        assertThat(result)
            .matches(actual -> !actual.at("/Links/Oem/Intel_RackScale/NeighborPort/@odata.id").isMissingNode());

        assertThat(result.at("/Links/Oem/Intel_RackScale/NeighborPort/@odata.id"))
            .isEqualTo(ethernetSwitchPort.at("/@odata.id"));
    }

    @Test
    public void whenGivenEthernetInterfaceLinkedToMultipleEthernetSwitchPorts_thenCompleteWithExtendedInfo() throws Exception {
        val sut = new ComputerSystemEthernetInterfaceResourceCompleter(new HashMap<>());
        val firstPort = readJsonFromFile(this, "EthernetSwitchPort.json").put("@odata.id", "/redfish/v1/EthernetSwitches/1/Ports/5");
        val secondPort = readJsonFromFile(this, "EthernetSwitchPort.json");
        sut.onResourcesCrawled(ImmutableSet.of(new ResourceImpl(firstPort), new ResourceImpl(secondPort)));

        val result = sut.completeResource(readJsonFromFile(this, "ComputerSystemEthernetInterface.json"));

        assertThat(result)
            .matches(
                actual -> actual.at("/Links/Oem/Intel_RackScale/NeighborPort").isMissingNode(),
                "NeighborPort should not be included in response"
            )
            .matches(
                actual -> !actual.at("/Links/Oem/Intel_RackScale/NeighborPort@Message.ExtendedInfo").isMissingNode(),
                "NeighborPort@Message.ExtendedInfo should be included in response"
            );

        String extendedMessage = "Couldn't match neighbor Ethernet Switch Port. "
            + "Couldn't find single, enabled and healthy Ethernet Switch Port with neighbor MAC Address: '87:5B:1B:11:28:F9'. Found 2 Ethernet Switch Ports.";

        ArrayNode expected = createArrayNodeWith(newExtendedInfoBuilder(extendedMessage).build());

        assertThat(result.at("/Links/Oem/Intel_RackScale/NeighborPort@Message.ExtendedInfo"))
            .isEqualTo(expected);
    }

    @Test
    public void whenGivenEthernetInterfaceWithEmptyMacLinkedToEthernetSwitchPort_thenShouldNotComplete() throws Exception {
        val sut = new ComputerSystemEthernetInterfaceResourceCompleter(new HashMap<>());
        val ethernetSwitchPort = readJsonFromFile(this, "EthernetSwitchPort.json").put("NeighborMAC", "");
        val computerSystemEthernetInterface = readJsonFromFile(this, "ComputerSystemEthernetInterface.json").put("MACAddress", "");

        sut.onResourcesCrawled(ImmutableSet.of(new ResourceImpl(ethernetSwitchPort), new ResourceImpl(computerSystemEthernetInterface)));

        val result = sut.completeResource(computerSystemEthernetInterface);

        assertThat(result)
            .matches(
                actual -> actual.at("/Links/Oem/Intel_RackScale/NeighborPort").isMissingNode(),
                "NeighborPort should not be included in response"
            );
    }

    @Test
    public void whenGivenEthernetInterfaceWithNullMacLinkedToEthernetSwitchPort_thenShouldNotComplete() throws Exception {
        val sut = new ComputerSystemEthernetInterfaceResourceCompleter(new HashMap<>());
        val ethernetSwitchPort = readJsonFromFile(this, "EthernetSwitchPort.json").putNull("NeighborMAC");
        val computerSystemEthernetInterface = readJsonFromFile(this, "ComputerSystemEthernetInterface.json").putNull("MACAddress");

        sut.onResourcesCrawled(ImmutableSet.of(new ResourceImpl(ethernetSwitchPort), new ResourceImpl(computerSystemEthernetInterface)));

        val result = sut.completeResource(computerSystemEthernetInterface);

        assertThat(result)
            .matches(
                actual -> actual.at("/Links/Oem/Intel_RackScale/NeighborPort").isMissingNode(),
                "NeighborPort should not be included in response"
            );
    }

    @Test
    public void whenGivenEthernetInterfaceNotLinkedToAnyEthernetSwitchPort_thenShouldNotComplete() throws Exception {
        val sut = new ComputerSystemEthernetInterfaceResourceCompleter(new HashMap<>());
        val ethernetSwitchPort = readJsonFromFile(this, "EthernetSwitchPort.json").put("NeighborMAC", "AA:BB:CC:DD:EE:FF");
        val computerSystemEthernetInterface = readJsonFromFile(this, "ComputerSystemEthernetInterface.json");

        sut.onResourcesCrawled(ImmutableSet.of(new ResourceImpl(ethernetSwitchPort), new ResourceImpl(computerSystemEthernetInterface)));

        val result = sut.completeResource(computerSystemEthernetInterface);

        assertThat(result)
            .matches(
                actual -> actual.at("/Links/Oem/Intel_RackScale/NeighborPort").isMissingNode(),
                "NeighborPort should not be included in response"
            );
    }

    @Test
    public void interestedInPatterns_shouldMatchEthernetSwitchPortUnifiedId() {
        val sut = new ComputerSystemEthernetInterfaceResourceCompleter(new HashMap<>());
        val interestedInPatterns = sut.interestedInPatterns();
        val unifiedEthernetSwitchPortId = "/redfish/v1/EthernetSwitches/547ca796-ee50-11e8-addc-6b3291d8f5b5_1/Ports/Ethernet23/4";

        assertThat(
            interestedInPatterns.stream()
                .anyMatch(p -> p.matcher(unifiedEthernetSwitchPortId).matches())
        ).isTrue();
    }

}
