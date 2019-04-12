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
import com.hazelcast.core.IAtomicReference;
import com.intel.rsd.json.JsonUtils;
import org.springframework.stereotype.Component;

import java.net.URI;
import java.util.Collection;
import java.util.Map;

import static com.intel.rsd.json.JsonUtils.stringToJsonNode;
import static java.lang.String.format;
import static java.util.Optional.ofNullable;

@Component
public class DetectedManagersCache {

    private final Map<String, URI> routableManagerUuidToUriMapping;

    private final Map<String, String> managerUriToBodyMapping;

    private final IAtomicReference<String> managersCollection;

    public DetectedManagersCache(Map<String, String> managerUriToBodyMapping,
                                 Map<String, URI> routableManagerUuidToUriMapping,
                                 IAtomicReference<String> managerCollectionReference) {

        this.managerUriToBodyMapping = managerUriToBodyMapping;
        this.routableManagerUuidToUriMapping = routableManagerUuidToUriMapping;
        this.managersCollection = managerCollectionReference;
    }

    /**
     * @return true if information about requested manager is already cached, false otherwise
     */
    public boolean contains(String managerUri) {
        return managerUriToBodyMapping.containsKey(managerUri);
    }

    /**
     * @return body of requested manager
     * @throws RuntimeException if requested manager is unknown
     */
    public JsonNode get(String managerUri) {
        return ofNullable(managerUriToBodyMapping.get(managerUri))
            .map(JsonUtils::stringToJsonNode)
            .orElseThrow(() -> new RuntimeException(format("Requested manager(%s) is unknown", managerUri)));
    }

    /**
     * @return body of managers collection if already cached, NullNode instance otherwise.
     * Returned collection contains managers of ResourceServices and DiscoveryServices
     */
    public JsonNode getCollection() {
        return stringToJsonNode(managersCollection.get());
    }

    /**
     * Routable manager is manger which represents managable service.
     * Non routable manager is manager of DiscoveryService
     *
     * @return collection of known routable managers
     */
    public Collection<String> getIdsOfRoutableManagers() {
        return routableManagerUuidToUriMapping.keySet();
    }

    /**
     * @return URI of requested routable manager
     */
    public URI findUriOfRoutableManagers(String routableManagerId) {
        return routableManagerUuidToUriMapping.get(routableManagerId);
    }

    public void update(CacheUpdater cacheUpdater) {
        cacheUpdater.update(managerUriToBodyMapping, routableManagerUuidToUriMapping, managersCollection);
    }
}
