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

import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.regex.Pattern;

import static com.google.common.base.Strings.isNullOrEmpty;
import static com.intel.rsd.ResourcePatterns.COMPUTER_SYSTEM_PATTERN;
import static com.intel.rsd.collections.IterableHelper.singleOrNull;
import static com.intel.rsd.resourcemanager.layers.completer.CollectionContentsUpdater.updateMapContents;
import static java.util.Arrays.asList;
import static java.util.Collections.singletonList;
import static java.util.stream.Collectors.toList;
import static java.util.stream.Collectors.toMap;

@Component
public class EndpointResourceCompleter implements ResourceCompleter {
    private static final String ENTITY_ROLE_INITIATOR = "Initiator";
    private static final List<String> SUPPORTED_DURABLE_NAME_FORMATS = asList("IQN", "NQN", "UUID");

    private final Map<String, String> systemUuidToUriMapping;

    @SuppressWarnings("unchecked")
    public EndpointResourceCompleter(@Shared("EndpointResourceCompleter.systemUuidToUriMapping") Map<?, ?> systemUuidToUriMap) {
        this.systemUuidToUriMapping = (Map<String, String>) systemUuidToUriMap;
    }

    @Override
    public boolean canCompleteResource(ObjectNode objectNode) {
        return isEndpoint(objectNode)
            && hasInitiatorRole(objectNode.path("ConnectedEntities"));
    }

    @Override
    public ObjectNode completeResource(ObjectNode initiatorEndpoint) {
        val systemUri = singleOrNull(getEndpointUuid(initiatorEndpoint).stream()
            .filter(systemUuidToUriMapping::containsKey)
            .map(systemUuidToUriMapping::get)
            .collect(toList()));

        if (isNullOrEmpty(systemUri)) {
            return initiatorEndpoint;
        }

        val connectedEntities = initiatorEndpoint.get("ConnectedEntities");
        updateEntityLink(connectedEntities, systemUri);

        return initiatorEndpoint;
    }

    @Override
    public List<Pattern> interestedInPatterns() {
        return singletonList(COMPUTER_SYSTEM_PATTERN);
    }

    @Override
    public void onResourcesCrawled(Set<Resource> systemResources) {
        Map<String, String> uuidToUriMapping = systemResources.stream()
            .collect(
                toMap(
                    systemResource -> systemResource.get("/UUID").asText(""),
                    Resource::getOdataId
                )
            );

        updateMapContents(systemUuidToUriMapping, uuidToUriMapping);
    }

    private boolean isEndpoint(ObjectNode objectNode) {
        val odataType = objectNode.path("@odata.type").asText();

        return odataType.contains("#Endpoint.");
    }

    private boolean hasInitiatorRole(JsonNode connectedEntities) {
        for (JsonNode connectedEntity : connectedEntities) {
            if (hasConnectedEntityRoleInitiator(connectedEntity)) {
                return true;
            }
        }
        return false;
    }

    private void updateEntityLink(JsonNode connectedEntities, String systemUri) {
        for (JsonNode connectedEntity : connectedEntities) {
            if (hasConnectedEntityRoleInitiator(connectedEntity)) {
                val odataIdObject = JsonNodeFactory.instance.objectNode().put("@odata.id", systemUri);

                ObjectNode objectNode = (ObjectNode) connectedEntity;
                objectNode.replace("EntityLink", odataIdObject);
            }
        }
    }

    private boolean hasConnectedEntityRoleInitiator(JsonNode connectedEntity) {
        return ENTITY_ROLE_INITIATOR.equals(connectedEntity.get("EntityRole").asText(""));
    }

    private Set<String> getEndpointUuid(ObjectNode endpointNode) {
        Set<String> endpointUuids = new HashSet<>();

        for (JsonNode identifier : endpointNode.get("Identifiers")) {
            if (SUPPORTED_DURABLE_NAME_FORMATS.contains(identifier.get("DurableNameFormat").asText(""))) {
                endpointUuids.add(getUuidFromIdentifier(identifier));
            }
        }

        return endpointUuids;
    }

    private String getUuidFromIdentifier(JsonNode identifier) {
        val endpointDurableName = identifier.get("DurableName").asText("");

        return endpointDurableName.substring(endpointDurableName.lastIndexOf(":") + 1).trim();
    }
}
