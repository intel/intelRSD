/*
 * Copyright (c) 2019 Intel Corporation
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

package com.intel.rsd.resourcemanager.common;

import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.node.ObjectNode;
import com.fasterxml.jackson.databind.node.TextNode;
import com.hazelcast.core.IAtomicReference;
import com.hazelcast.spi.exception.PartitionMigratingException;
import com.intel.rsd.json.JsonUtils;
import lombok.NonNull;
import lombok.val;
import org.springframework.retry.annotation.Retryable;

import java.net.URI;
import java.util.Map;
import java.util.Optional;

import static com.intel.rsd.json.JsonUtils.jsonNodeAsString;
import static com.intel.rsd.optional.OptionalUtils.select;
import static com.intel.rsd.resourcemanager.common.MapUpdater.updateValues;
import static java.util.stream.Collectors.toMap;
import static java.util.stream.StreamSupport.stream;

public class CacheUpdater {

    private static final TextNode RESOURCE_SERVICE = new TextNode("ResourceService");

    private final ServiceDetectorApiConsumer serviceDetectorApiConsumer;

    public CacheUpdater(ServiceDetectorApiConsumer serviceDetectorApiConsumer) {
        this.serviceDetectorApiConsumer = serviceDetectorApiConsumer;
    }

    @Retryable(value = PartitionMigratingException.class)
    public void update(Map<String, String> managerUriToBodyMapping,
                       Map<String, URI> routableManagerUuidToUriMapping,
                       IAtomicReference<String> managerCollectionReference) {

        val response = serviceDetectorApiConsumer.getExpandedManagers();
        if (response.getStatusCode().is2xxSuccessful()) {
            val expandedManagers = response.getBody();
            updateRoutableManagers(routableManagerUuidToUriMapping, expandedManagers.deepCopy());
            updateManagersCollection(managerCollectionReference, expandedManagers.deepCopy());
            updateUriToBodyMapping(managerUriToBodyMapping, expandedManagers.deepCopy());
        }
    }

    private void updateManagersCollection(IAtomicReference<String> managerCollectionReference, JsonNode freshState) {
        freshState.withArray("Members").forEach(member -> ((ObjectNode) member).retain("@odata.id"));
        managerCollectionReference.set(jsonNodeAsString(freshState));
    }

    private void updateRoutableManagers(Map<String, URI> cached, JsonNode last) {
        val members = last.withArray("Members");
        val currentState = stream(members.spliterator(), false)
            .filter(member -> RESOURCE_SERVICE.equals(member.get("RemoteRedfishServiceType")))
            .collect(
                toMap(
                    member -> member.get("ServiceEntryPointUUID").asText(),
                    member -> URI.create(member.get("RemoteRedfishServiceUri").asText())
                )
            );
        updateValues(cached, currentState);
    }

    private void updateUriToBodyMapping(Map<String, String> cached, JsonNode last) {
        val members = last.withArray("Members");
        val currentState = stream(members.spliterator(), false)
            .map(this::suppressRemoteRedfishServiceType)
            .collect(
                toMap(
                    member -> member.get("@odata.id").asText(),
                    JsonUtils::jsonNodeAsString
                )
            );
        updateValues(cached, currentState);
    }

    private JsonNode suppressRemoteRedfishServiceType(@NonNull JsonNode member) {
        Optional.of(member)
            .map(select(ObjectNode.class))
            .ifPresent(m -> m.remove("RemoteRedfishServiceType"));

        return member;
    }
}
