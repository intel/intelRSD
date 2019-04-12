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
import lombok.extern.slf4j.Slf4j;
import lombok.val;
import org.springframework.http.HttpStatus;
import org.springframework.http.RequestEntity;
import org.springframework.http.ResponseEntity;
import org.springframework.web.client.ResourceAccessException;
import org.springframework.web.client.RestTemplate;

import java.net.URI;

import static java.net.URI.create;
import static org.springframework.http.ResponseEntity.status;
import static org.springframework.web.util.UriComponentsBuilder.fromUriString;

@Slf4j
public class ServiceDetectorApiConsumer {

    private static final URI MANAGERS_COLLECTION_ON_SERVICE_DETECTOR_URI = create("http://service-detector/redfish/v1/Managers");
    private static final URI EXPANDED_MANAGERS_COLLECTION_ON_SERVICE_DETECTOR_URI = create("http://service-detector/redfish/v1/Managers?$expand=.($levels=1)");
    private static final String SERVICE_DETECTOR_URI = "http://service-detector";

    private final RestTemplate restTemplate;

    public ServiceDetectorApiConsumer(RestTemplate restTemplate) {
        this.restTemplate = restTemplate;
    }

    public ResponseEntity<JsonNode> getExpandedManagers() {
        val expandedManagersRequest = RequestEntity.get(EXPANDED_MANAGERS_COLLECTION_ON_SERVICE_DETECTOR_URI).build();
        return execute(expandedManagersRequest);
    }

    public ResponseEntity<JsonNode> getManagers() {
        val expandedmanagersRequest = RequestEntity.get(MANAGERS_COLLECTION_ON_SERVICE_DETECTOR_URI).build();
        return execute(expandedmanagersRequest);
    }

    public ResponseEntity<JsonNode> deleteManager(String managerUri) {
        val uriOfManagerUnderDeletion = fromUriString(SERVICE_DETECTOR_URI).pathSegment(managerUri).build().toUri();
        val deletionRequest = RequestEntity.delete(uriOfManagerUnderDeletion).build();
        return execute(deletionRequest);
    }

    public ResponseEntity<JsonNode> registerManager(JsonNode registrationJson) {
        val requestEntity = RequestEntity.post(MANAGERS_COLLECTION_ON_SERVICE_DETECTOR_URI).body(registrationJson);
        return execute(requestEntity);
    }

    private <T> ResponseEntity<JsonNode> execute(RequestEntity<T> expandedManagersCollectionRequest) {
        try {
            return restTemplate.exchange(expandedManagersCollectionRequest, JsonNode.class);
        } catch (IllegalStateException | ResourceAccessException e) {
            log.warn("Cannot connect to service-detector: {}", e.getMessage());
            return status(HttpStatus.SERVICE_UNAVAILABLE).build();
        }
    }
}
