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

import lombok.val;
import org.testng.annotations.BeforeSuite;
import org.testng.annotations.Test;

import java.util.HashMap;

import static com.intel.rsd.json.JsonUtils.readJsonFromFile;
import static java.util.Collections.singleton;
import static org.assertj.core.api.AssertionsForClassTypes.assertThat;

public class ComputerSystemEndpointResourceCompleterTest {
    private static final String COMPUTER_SYSTEM_UUID = "e358ab40-7ad2-11e8-b20c-fb5de0316502";
    private ComputerSystemEndpointResourceCompleter completer;

    @BeforeSuite
    public void initializeCompleter() {
        this.completer = new ComputerSystemEndpointResourceCompleter(new HashMap<>());
    }

    @Test
    public void whenGivenSystemIsPhysical_thenCompleterCanComplete() throws Exception {
        val computerSystem = readJsonFromFile(this, "System.json");
        val result = completer.canCompleteResource(computerSystem);

        assertThat(result).isTrue();
    }

    @Test
    public void whenGivenSystemIsVirtual_thenCompleterCanNotComplete() throws Exception {
        val computerSystem = readJsonFromFile(this, "System.json").put("SystemType", "Virtual");
        val result = completer.canCompleteResource(computerSystem);

        assertThat(result).isFalse();
    }

    @Test
    public void whenGivenSystemUuidMatchesInitiatorEndpointIdentifier_thenCompleteResponseWithLinkToEndpoint() throws Exception {
        val computerSystem = readJsonFromFile(this, "System.json").put("UUID", COMPUTER_SYSTEM_UUID);
        val initiatorEndpoint = readJsonFromFile(this, "EndpointInitiator.json");

        completer.onResourcesCrawled(singleton(new ResourceImpl(initiatorEndpoint)));
        val result = completer.completeResource(computerSystem);

        val endpointInSystem = result.get("Links").get("Endpoints");
        assertThat(endpointInSystem.size()).isGreaterThan(0);

        val endpointUri = endpointInSystem.get(0).get("@odata.id").asText("");
        val endpointUuidFromUri = endpointUri.substring(endpointUri.lastIndexOf(":") + 1).trim();
        assertThat(endpointUuidFromUri).isEqualTo(COMPUTER_SYSTEM_UUID);
    }

    @Test
    public void whenGivenSystemUuidDoesNotMatchInitiatorEndpointIdentifier_thenShouldNotComplete() throws Exception {
        val alteredComputerSystemUuid = "aaa8ab40-7ad2-11e8-b20c-fb5de0316511";

        val computerSystem = readJsonFromFile(this, "System.json").put("UUID", alteredComputerSystemUuid);
        val initiatorEndpoint = readJsonFromFile(this, "EndpointInitiator.json");

        completer.onResourcesCrawled(singleton(new ResourceImpl(initiatorEndpoint)));
        val result = completer.completeResource(computerSystem);

        val endpointInSystem = result.get("Links").get("Endpoints");
        assertThat(endpointInSystem.size()).isZero();
    }

    @Test
    public void whenGivenSystemUuidMatchesTargetEndpointIdentifier_thenShouldNotComplete() throws Exception {
        val computerSystem = readJsonFromFile(this, "System.json").put("UUID", COMPUTER_SYSTEM_UUID);
        val targetEndpoint = readJsonFromFile(this, "EndpointTarget.json");

        completer.onResourcesCrawled(singleton(new ResourceImpl(targetEndpoint)));
        val result = completer.completeResource(computerSystem);

        val endpointInSystem = result.get("Links").get("Endpoints");
        assertThat(endpointInSystem.size()).isZero();
    }
}
