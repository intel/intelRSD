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

package com.intel.rsd.eventservice;

import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.node.ArrayNode;
import com.fasterxml.jackson.databind.node.ObjectNode;
import com.intel.rsd.dto.events.SubscriptionRequest;
import lombok.extern.slf4j.Slf4j;
import lombok.val;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.DeleteMapping;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PathVariable;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

import javax.validation.Valid;
import java.io.IOException;
import java.net.URI;
import java.util.Objects;

import static com.intel.rsd.json.JsonUtils.readJsonFromFile;
import static com.intel.rsd.redfish.RedfishUtils.toODataIdArrayNode;
import static java.util.stream.Collectors.toList;

@RestController
@Slf4j
@RequestMapping("/redfish/v1")
@SuppressWarnings("checkstyle:ClassFanOutComplexity")
public class EventServiceController {
    private final ObjectNode eventServiceResource;
    private final ObjectNode subscriptionsResourceTemplate;
    private final SubscriptionService subscriptionService;

    public EventServiceController(EventServiceConfiguration eventServiceConfiguration, SubscriptionService subscriptionService) throws IOException {
        this.subscriptionService = subscriptionService;
        eventServiceResource = readJsonFromFile(this, "EventService.json");
        eventServiceResource.put("DeliveryRetryAttempts", eventServiceConfiguration.getDeliveryRetryAttempts());
        eventServiceResource.put("DeliveryRetryIntervalSeconds", eventServiceConfiguration.getDeliveryRetryIntervalSeconds());
        subscriptionsResourceTemplate = readJsonFromFile(this, "Subscriptions.json");
    }

    @GetMapping(value = "/EventService")
    public JsonNode eventService() {
        return eventServiceResource;
    }

    @GetMapping("/EventService/Subscriptions")
    public JsonNode subscriptions() {
        ArrayNode members = getSubscriptions();
        return subscriptionsResourceTemplate.deepCopy()
            .put("Members@odata.count", members.size())
            .set("Members", members);
    }

    private ArrayNode getSubscriptions() {
        val subscriptions = subscriptionService.getAllSubscriptions();
        return toODataIdArrayNode(subscriptions.stream().map(Subscription::getOdataId).collect(toList()));
    }

    @PostMapping("/EventService/Subscriptions")
    public ResponseEntity<?> subscriptionRegistration(@Valid @RequestBody SubscriptionRequest subscriptionRequest) {
        if (subscriptionRequest.getOriginResources() != null && subscriptionRequest.getOriginResources().stream().anyMatch(Objects::isNull)) {
            throw new BadRequestException("Elements in OriginResources array cannot be null");
        }
        URI subscriptionUri = subscriptionService.createSubscription(subscriptionRequest);
        return ResponseEntity.created(subscriptionUri).build();
    }

    @GetMapping("/EventService/Subscriptions/{id}")
    public Subscription getSubscription(@PathVariable Integer id) {
        return subscriptionService.getOrThrowNotFoundException(id);
    }

    @DeleteMapping("/EventService/Subscriptions/{id}")
    public ResponseEntity<?> subscriptionRemoval(@PathVariable Integer id) {
        subscriptionService.removeOrThrowNotFoundException(id);
        return ResponseEntity.noContent().build();
    }
}
