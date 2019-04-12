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

import com.fasterxml.jackson.databind.JsonNode;
import lombok.val;
import org.testng.annotations.BeforeSuite;
import org.testng.annotations.Test;

import java.util.HashMap;

import static com.intel.rsd.json.JsonUtils.readJsonFromFile;
import static java.util.Collections.singleton;
import static org.assertj.core.api.AssertionsForClassTypes.assertThat;

public class EndpointResourceCompleterTest {
    private static final String COMPUTER_SYSTEM_UUID = "e358ab40-7ad2-11e8-b20c-fb5de0316502";
    private EndpointResourceCompleter completer;

    @BeforeSuite
    public void initializeCompleter() {
        this.completer = new EndpointResourceCompleter(new HashMap<>());
    }

    @Test
    public void givenResourceIsInitiatorEndpoint_thenCanComplete() throws Exception {
        val initiatorEndpoint = readJsonFromFile(this, "EndpointInitiator.json");
        val result = completer.canCompleteResource(initiatorEndpoint);

        assertThat(result).isTrue();
    }

    @Test
    public void whenGivenSystemUuidMatchesInitiatorEndpointIdentifier_thenCompleteResponseWithLinkToEndpoint() throws Exception {
        val computerSystem = readJsonFromFile(this, "System.json").put("UUID", COMPUTER_SYSTEM_UUID);
        val computerSystemUri = computerSystem.get("@odata.id");

        val initiatorEndpoint = readJsonFromFile(this, "EndpointInitiator.json");

        completer.onResourcesCrawled(singleton(new ResourceImpl(computerSystem)));
        val result = completer.completeResource(initiatorEndpoint);

        JsonNode endpointEntityLinkUri = result.get("ConnectedEntities").get(0).get("EntityLink").get("@odata.id");
        assertThat(endpointEntityLinkUri).isEqualTo(computerSystemUri);
    }

    @Test
    public void whenGivenSystemUuidDoesNotMatchInitiatorEndpointIdentifier_thenShouldNotComplete() throws Exception {
        val alteredComputerSystemUuid = "aaa8ab40-7ad2-11e8-b20c-fb5de0316511";

        val computerSystem = readJsonFromFile(this, "System.json").put("UUID", alteredComputerSystemUuid);
        val initiatorEndpoint = readJsonFromFile(this, "EndpointInitiator.json");

        completer.onResourcesCrawled(singleton(new ResourceImpl(computerSystem)));
        val result = completer.completeResource(initiatorEndpoint);

        String endpointEntityLinkUri = result.get("ConnectedEntities").get(0).get("EntityLink").asText("");
        assertThat(endpointEntityLinkUri).isEqualToIgnoringCase("");
    }

    @Test
    public void whenGivenSystemUuidMatchesTargetEndpointIdentifier_thenShouldNotComplete() throws Exception {
        val computerSystem = readJsonFromFile(this, "System.json").put("UUID", COMPUTER_SYSTEM_UUID);
        val initiatorEndpoint = readJsonFromFile(this, "EndpointTarget.json");

        completer.onResourcesCrawled(singleton(new ResourceImpl(computerSystem)));
        val result = completer.completeResource(initiatorEndpoint);

        String endpointEntityLinkUri = result.get("ConnectedEntities").get(0).get("EntityLink").asText("");
        assertThat(endpointEntityLinkUri).isEqualToIgnoringCase("");
    }
}
