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

public class EthernetSwitchPortResourceCompleterTest {
    @Test
    public void whenGivenObjectNodeRepresentingEthernetSwitchPort_thenShouldBeAbleToComplete() throws Exception {
        val sut = new EthernetSwitchPortResourceCompleter(new HashMap<>());
        boolean result = sut.canCompleteResource(readJsonFromFile(this, "EthernetSwitchPort.json"));
        assertTrue(result);
    }

    @Test
    public void whenGivenObjectNodeRepresentingAclOfEthernetSwitch_thenShouldNotBeAbleToComplete() throws Exception {
        val sut = new EthernetSwitchPortResourceCompleter(new HashMap<>());
        boolean result = sut.canCompleteResource(readJsonFromFile(this, "ACL.json"));
        assertFalse(result);
    }

    @Test
    public void whenGivenEthernetSwitchPortLinkedToEthernetInterface_thenCompleteResponseWithLinkToEthernetInterface() throws Exception {
        val sut = new EthernetSwitchPortResourceCompleter(new HashMap<>());
        val mac = "AA:BB:CC:DD:EE:FF";
        val ethernetInterface = readJsonFromFile(this, "ComputerSystemEthernetInterface.json").put("MACAddress", mac);

        sut.onResourcesCrawled(singleton(new ResourceImpl(ethernetInterface)));

        val ethernetSwitchPort = readJsonFromFile(this, "EthernetSwitchPort.json").put("NeighborMAC", mac);
        val result = sut.completeResource(ethernetSwitchPort);

        assertThat(result)
            .matches(actual -> !actual.at("/NeighborInterface/@odata.id").isMissingNode());

        assertThat(result.at("/NeighborInterface/@odata.id"))
            .isEqualTo(ethernetInterface.at("/@odata.id"));
    }

    @Test
    public void whenGivenEthernetSwitchPortLinkedToMultipleEthernetInterfaces_thenCompleteWithExtendedInfo() throws Exception {
        val sut = new EthernetSwitchPortResourceCompleter(new HashMap<>());
        val firstEthernetInterface = readJsonFromFile(this, "ComputerSystemEthernetInterface.json")
            .put("@odata.id", "/redfish/v1/Systems/db046a90-d610-11e7-9661-419a3eff5c1a/EthernetInterfaces/87:5B:1B:11:28:F9");
        val secondEthernetInterface = readJsonFromFile(this, "ComputerSystemEthernetInterface.json");
        sut.onResourcesCrawled(ImmutableSet.of(new ResourceImpl(firstEthernetInterface), new ResourceImpl(secondEthernetInterface)));

        val result = sut.completeResource(readJsonFromFile(this, "EthernetSwitchPort.json"));

        assertThat(result)
            .matches(
                actual -> actual.at("/NeighborInterface").isMissingNode(),
                "NeighborInterface should not be included in response"
            )
            .matches(
                actual -> !actual.at("/NeighborInterface@Message.ExtendedInfo").isMissingNode(),
                "NeighborInterface@Message.ExtendedInfo should be included in response"
            );

        String extendedMessage = "Couldn't match neighbor Ethernet Interface. "
            + "Couldn't find single, enabled and healthy Ethernet Interface with MAC Address: '87:5B:1B:11:28:F9'. Found 2 Ethernet Interfaces.";

        ArrayNode expected = createArrayNodeWith(newExtendedInfoBuilder(extendedMessage).build());

        assertThat(result.at("/NeighborInterface@Message.ExtendedInfo"))
            .isEqualTo(expected);
    }

    @Test
    public void whenGivenEthernetSwitchPortWithEmptyMacLinkedToEthernetInterface_thenShouldNotComplete() throws Exception {
        val sut = new EthernetSwitchPortResourceCompleter(new HashMap<>());
        val computerSystemEthernetInterface = readJsonFromFile(this, "ComputerSystemEthernetInterface.json").put("MACAddress", "");
        val ethernetSwitchPort = readJsonFromFile(this, "EthernetSwitchPort.json").put("NeighborMAC", "");

        sut.onResourcesCrawled(ImmutableSet.of(new ResourceImpl(ethernetSwitchPort), new ResourceImpl(computerSystemEthernetInterface)));

        val result = sut.completeResource(ethernetSwitchPort);

        assertThat(result)
            .matches(
                actual -> actual.at("/NeighborInterface").isMissingNode(),
                "NeighborInterface should not be included in response"
            );
    }

    @Test
    public void whenGivenEthernetSwitchPortWithNullMacLinkedToEthernetInterface_thenShouldNotComplete() throws Exception {
        val sut = new EthernetSwitchPortResourceCompleter(new HashMap<>());
        val computerSystemEthernetInterface = readJsonFromFile(this, "ComputerSystemEthernetInterface.json").putNull("MACAddress");
        val ethernetSwitchPort = readJsonFromFile(this, "EthernetSwitchPort.json").putNull("NeighborMAC");

        sut.onResourcesCrawled(ImmutableSet.of(new ResourceImpl(ethernetSwitchPort), new ResourceImpl(computerSystemEthernetInterface)));

        val result = sut.completeResource(ethernetSwitchPort);

        assertThat(result)
            .matches(
                actual -> actual.at("/NeighborInterface").isMissingNode(),
                "NeighborInterface should not be included in response"
            );
    }

    @Test
    public void whenGivenEthernetSwitchPortNotLinkedToAnyEthernetInterface_thenShouldNotComplete() throws Exception {
        val sut = new EthernetSwitchPortResourceCompleter(new HashMap<>());
        val computerSystemEthernetInterface = readJsonFromFile(this, "ComputerSystemEthernetInterface.json");
        val ethernetSwitchPort = readJsonFromFile(this, "EthernetSwitchPort.json").put("NeighborMAC", "AA:BB:CC:DD:EE:FF");

        sut.onResourcesCrawled(ImmutableSet.of(new ResourceImpl(ethernetSwitchPort), new ResourceImpl(computerSystemEthernetInterface)));

        val result = sut.completeResource(ethernetSwitchPort);

        assertThat(result)
            .matches(
                actual -> actual.at("/NeighborInterface").isMissingNode(),
                "NeighborInterface should not be included in response"
            );
    }
}
