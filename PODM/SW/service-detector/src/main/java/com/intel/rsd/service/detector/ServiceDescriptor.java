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

package com.intel.rsd.service.detector;

import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.node.NullNode;
import com.intel.rsd.service.detector.data.RemoteServiceType;
import com.intel.rsd.service.detector.endpoint.registration.ServiceDescribingException;
import lombok.val;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Qualifier;
import org.springframework.http.RequestEntity;
import org.springframework.http.ResponseEntity;
import org.springframework.stereotype.Component;
import org.springframework.web.client.RestClientException;
import org.springframework.web.client.RestTemplate;

import java.net.URI;
import java.util.UUID;

import static org.springframework.http.MediaType.APPLICATION_JSON;
import static org.springframework.http.RequestEntity.get;

@Component
public class ServiceDescriptor {
    private RestTemplate restTemplate;

    @Autowired
    public ServiceDescriptor(@Qualifier("trustingAllRestTemplate") RestTemplate restTemplate) {
        this.restTemplate = restTemplate;
    }

    public ServiceEndpoint describe(URI endpointUri) throws ServiceDescribingException {
        val request = get(endpointUri).accept(APPLICATION_JSON).build();
        val result = exchange(request);
        val body = result.getBody();
        if (body == null) {
            throw new ServiceDescribingException("Obtaining UUID failed - service responded with empty JSON body");
        }
        val uuid = getUuid(body);
        val remoteServiceType = determineType(body);
        return new ServiceEndpoint(uuid, endpointUri, remoteServiceType);
    }

    private ResponseEntity<JsonNode> exchange(RequestEntity<Void> request) {
        try {
            return restTemplate.exchange(request, JsonNode.class);
        } catch (RestClientException | IllegalStateException ex) {
            throw new ServiceDescribingException(ex.getMessage());
        }
    }

    private UUID getUuid(JsonNode result) {
        JsonNode uuidJsonNode = result.get("UUID");
        if (uuidJsonNode == null || uuidJsonNode instanceof NullNode) {
            throw new ServiceDescribingException("Obtaining UUID failed - UUID property value is missing on Service Root");
        } else {
            return UUID.fromString(uuidJsonNode.asText());
        }
    }

    private RemoteServiceType determineType(JsonNode result) {
        JsonNode nameJsonNode = result.get("Name");
        String serviceRootName = nameJsonNode != null ? nameJsonNode.asText("") : "";
        return RemoteServiceType.findByName(serviceRootName);
    }
}
