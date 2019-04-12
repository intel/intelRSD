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

package com.intel.rsd.resourcemanager.layers.unifier.patterns.rewriting.transformers;

import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.node.ArrayNode;
import com.fasterxml.jackson.databind.node.JsonNodeFactory;
import org.testng.annotations.Test;

import static com.intel.rsd.resourcemanager.layers.unifier.patterns.rewriting.transformers.FindIdForEndpoint.DurableNameFormat.IQN;
import static com.intel.rsd.resourcemanager.layers.unifier.patterns.rewriting.transformers.FindIdForEndpoint.DurableNameFormat.NQN;
import static com.intel.rsd.resourcemanager.layers.unifier.patterns.rewriting.transformers.FindIdForEndpoint.DurableNameFormat.UUID;
import static java.util.Arrays.asList;
import static java.util.UUID.randomUUID;
import static org.testng.Assert.assertTrue;

public class FindIdForEndpointTest {

    JsonNode uuidIdentifier = JsonNodeFactory.instance.objectNode()
        .put("DurableNameFormat", UUID.toString())
        .put("DurableName", randomUUID().toString());

    JsonNode iqnIdentifier = JsonNodeFactory.instance.objectNode()
        .put("DurableNameFormat", IQN.toString())
        .put("DurableName", randomUUID().toString());

    JsonNode nqnIdentifier = JsonNodeFactory.instance.objectNode()
        .put("DurableNameFormat", NQN.toString())
        .put("DurableName", randomUUID().toString());

    JsonNode unknownIdentifier = JsonNodeFactory.instance.objectNode()
        .put("DurableNameFormat", "Unknown")
        .put("DurableName", randomUUID().toString());

    FindIdForEndpoint sut = new FindIdForEndpoint();

    @Test
    public void whenEndpointWithoutIdentifiersIsProvided_nullShouldBeReturned() throws Exception {
        ArrayNode endpointBody = JsonNodeFactory.instance.arrayNode();
        assertTrue(sut.invoke(endpointBody) == null);
    }

    @Test
    public void whenEndpointWithUuidIdentifierIsProvided_relatedDurableNameShouldBeReturned() throws Exception {
        ArrayNode endpointBody = JsonNodeFactory.instance.arrayNode().addAll(asList(uuidIdentifier));
        assertTrue(uuidIdentifier.at("/DurableName").asText().equals(sut.invoke(endpointBody)));
    }

    @Test
    public void whenNqNAndUuidIdentifiersAreProvided_uuidIdentifierShouldBePreferred() throws Exception {
        ArrayNode endpointBody = JsonNodeFactory.instance.arrayNode().addAll(asList(nqnIdentifier, uuidIdentifier));
        assertTrue(uuidIdentifier.at("/DurableName").asText().equals(sut.invoke(endpointBody)));
    }

    @Test
    public void whenNqNAndIqnIdentifiersAreProvided_nqnIdentifierShouldBePreferred() throws Exception {
        ArrayNode endpointBody = JsonNodeFactory.instance.arrayNode().addAll(asList(iqnIdentifier, nqnIdentifier));
        assertTrue(nqnIdentifier.at("/DurableName").asText().equals(sut.invoke(endpointBody)));
    }

    @Test
    public void whenUnknownIdentifierIsProvided_itsDurableNameShouldBeReturned() throws Exception {
        ArrayNode endpointBody = JsonNodeFactory.instance.arrayNode().addAll(asList(unknownIdentifier));
        assertTrue(unknownIdentifier.at("/DurableName").asText().equals(sut.invoke(endpointBody)));
    }

    @Test
    public void whenIqnAndUnknownIdentifiersAreProvided_iqnIdentifierShouldBePreferred() throws Exception {
        ArrayNode endpointBody = JsonNodeFactory.instance.arrayNode().addAll(asList(unknownIdentifier, iqnIdentifier));
        assertTrue(iqnIdentifier.at("/DurableName").asText().equals(sut.invoke(endpointBody)));
    }

}
