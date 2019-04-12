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

package com.intel.rsd.resourcemanager.runner.events;

import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.node.ObjectNode;
import com.intel.rsd.dto.events.EventType;
import com.intel.rsd.json.ArrayNodeCollector;
import com.intel.rsd.resourcemanager.layers.EventHandler;
import com.intel.rsd.resourcemanager.layers.ServiceId;
import com.intel.rsd.resourcemanager.runner.Pipeline;
import lombok.extern.slf4j.Slf4j;
import lombok.val;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.PathVariable;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RestController;

import javax.validation.Valid;
import java.util.Optional;

import static java.util.Optional.ofNullable;
import static java.util.stream.StreamSupport.stream;
import static org.springframework.http.ResponseEntity.noContent;

@RestController
@Slf4j
public class EventsUpstreamController {
    private final EventHandler eventsUpstreamPipeline;

    @Autowired
    public EventsUpstreamController(@Pipeline EventHandler eventHandler) {
        this.eventsUpstreamPipeline = eventHandler;
    }

    @PostMapping("${events.receiver.endpoint}/{serviceId}")
    public ResponseEntity<?> onEventReceived(@PathVariable String serviceId,
                                             @Valid @RequestBody ObjectNode incomingEvent) {

        log.info("New incomingEvent received for service({}): {}", serviceId, incomingEvent);
        normalize(incomingEvent);
        filterNonPassThroughEvents(incomingEvent).ifPresent(outgoingEvent -> sendEvent(serviceId, outgoingEvent));
        return noContent().build();
    }

    private void sendEvent(String serviceId, JsonNode e) {
        eventsUpstreamPipeline.onEvent(new ServiceId(serviceId), e);
    }

    private Optional<JsonNode> filterNonPassThroughEvents(ObjectNode event) {
        val records = event.withArray("Events");
        val filteredRecords = stream(records.spliterator(), false)
            .filter(this::hasOriginOfConditionField)
            .filter(this::hasValidEventTypeField)
            .collect(new ArrayNodeCollector());

        event.set("Events", filteredRecords);

        return (filteredRecords.size() == 0) ? Optional.empty() : Optional.of(event);
    }

    private boolean hasValidEventTypeField(JsonNode eventRecord) {
        if (!eventRecord.has("EventType")) {
            return false;
        }

        EventType.valueOf(eventRecord.get("EventType").asText());
        return true;
    }

    private boolean hasOriginOfConditionField(JsonNode eventRecord) {
        return ofNullable(eventRecord.get("OriginOfCondition")).map(ooc -> ooc.has("@odata.id")).orElse(false);
    }

    private void normalize(ObjectNode event) {
        event.without("@odata.id");
    }
}
