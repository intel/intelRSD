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

package com.intel.rsd.resourcemanager.runner.events;

import com.fasterxml.jackson.databind.JsonNode;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Qualifier;
import org.springframework.http.ResponseEntity;
import org.springframework.stereotype.Component;
import org.springframework.web.client.ResourceAccessException;
import org.springframework.web.client.RestOperations;

import java.net.URI;

@Slf4j
@Component
class RestCallRouteVerifier {

    private final RestOperations restOperations;

    RestCallRouteVerifier(@Qualifier("northbound") RestOperations restTemplate) {
        this.restOperations = restTemplate;
    }

    boolean isReachable(final URI target) {
        try {
            ResponseEntity<JsonNode> response = restOperations.getForEntity(target, JsonNode.class);
            if (response.getStatusCode().is2xxSuccessful()) {
                return true;
            } else {
                String responseBody = response.getBody() != null ? response.getBody().toString() : null;
                log.warn("{} returned non 2xx status code: {} with body: {}", target, response.getStatusCode().value(), responseBody);
            }
        } catch (
            ResourceAccessException e) {
            log.warn("{} is unavailable due to {}", target, e.getMessage());
        }
        return false;
    }
}
