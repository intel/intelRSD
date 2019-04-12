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
import com.fasterxml.jackson.databind.node.JsonNodeFactory;
import com.fasterxml.jackson.databind.node.ObjectNode;
import com.intel.rsd.crawler.Resource;
import com.intel.rsd.services.common.Shared;
import lombok.val;
import org.springframework.stereotype.Component;

import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.regex.Pattern;

import static com.intel.rsd.ResourcePatterns.ENDPOINT_PATTERN;
import static com.intel.rsd.resourcemanager.layers.completer.CollectionContentsUpdater.updateMapContents;
import static java.util.Arrays.asList;
import static java.util.Collections.emptySet;
import static java.util.Collections.singletonList;
import static java.util.stream.Collectors.toMap;
import static java.util.stream.Collectors.toSet;
import static java.util.stream.StreamSupport.stream;

@Component
public class ComputerSystemEndpointResourceCompleter implements ResourceCompleter {
    private static final String ENTITY_ROLE_INITIATOR = "Initiator";
    private static final List<String> SUPPORTED_DURABLE_NAME_FORMATS = asList("IQN", "NQN", "UUID");

    private final Map<String, Set<String>> endpointUriToEndpointUuidsMapping;

    @SuppressWarnings("unchecked")
    public ComputerSystemEndpointResourceCompleter(@Shared("ComputerSystemEndpointResourceCompleter.endpointUriToEndpointUuidsMapping")
                                                       Map<?, ?> endpointUriToEndpointUuidCollectionMap) {
        this.endpointUriToEndpointUuidsMapping = (Map<String, Set<String>>) endpointUriToEndpointUuidCollectionMap;
    }

    @Override
    public boolean canCompleteResource(ObjectNode objectNode) {

        return isNotVirtualComputerSystem(objectNode);
    }

    @Override
    public ObjectNode completeResource(ObjectNode computerSystem) {
        val endpoints = JsonNodeFactory.instance.arrayNode().addAll(getEndpointsMatchingSystemUuid(computerSystem));

        computerSystem
            .with("Links")
            .set("Endpoints", endpoints);

        return computerSystem;
    }

    private Set<ObjectNode> getEndpointsMatchingSystemUuid(ObjectNode computerSystem) {
        return endpointUriToEndpointUuidsMapping.entrySet().stream()
            .filter(entry -> entry.getValue().contains(computerSystem.get("UUID").asText("")))
            .map(entry -> JsonNodeFactory.instance.objectNode().put("@odata.id", entry.getKey()))
            .collect(toSet());
    }

    @Override
    public List<Pattern> interestedInPatterns() {
        return singletonList(ENDPOINT_PATTERN);
    }

    @Override
    public void onResourcesCrawled(Set<Resource> resources) {
        Map<String, Set<String>> uriToUuidMapping = resources.stream()
            .collect(
                toMap(
                    Resource::getOdataId,
                    this::getUuidsFromEndpointDurableNames
                )
            );

        updateMapContents(endpointUriToEndpointUuidsMapping, uriToUuidMapping);
    }

    private boolean isNotVirtualComputerSystem(ObjectNode objectNode) {
        val odataType = objectNode.path("@odata.type").asText();
        val systemType = objectNode.path("SystemType").asText();

        return odataType.contains("#ComputerSystem.")
            && !"Virtual".equals(systemType);
    }

    private Set<String> getUuidsFromEndpointDurableNames(Resource resource) {
        if (hasRoleInitiator(resource)) {
            return stream(resource.get("/Identifiers").spliterator(), false)
                .filter(identifier -> SUPPORTED_DURABLE_NAME_FORMATS.contains(identifier.get("DurableNameFormat").asText("")))
                .map(this::getUuidFromDurableName)
                .collect(toSet());
        }

        return emptySet();
    }

    private boolean hasRoleInitiator(Resource resource) {
        val resourceIterator = resource.get("/ConnectedEntities");

        for (JsonNode connectedEntity : resourceIterator) {
            if (ENTITY_ROLE_INITIATOR.equals(connectedEntity.get("EntityRole").asText(""))) {
                return true;
            }
        }

        return false;
    }

    private String getUuidFromDurableName(JsonNode identifier) {
        val endpointDurableName = identifier.get("DurableName").asText("");

        return endpointDurableName.substring(endpointDurableName.lastIndexOf(":") + 1).trim();
    }
}
