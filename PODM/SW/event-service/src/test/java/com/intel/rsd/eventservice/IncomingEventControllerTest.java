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
import com.google.common.collect.ImmutableList;
import com.intel.rsd.dto.events.Event;
import com.intel.rsd.dto.events.EventRecord;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.test.context.SpringBootTest;
import org.springframework.boot.test.web.client.TestRestTemplate;
import org.springframework.cloud.client.discovery.EnableDiscoveryClient;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.test.context.testng.AbstractTestNGSpringContextTests;
import org.testng.annotations.Test;

import static com.intel.rsd.dto.events.EventFactory.createEvent;
import static com.intel.rsd.dto.events.EventFactory.createEventRecord;
import static com.intel.rsd.dto.events.EventType.Alert;
import static org.assertj.core.api.Java6Assertions.assertThat;

@EnableDiscoveryClient(autoRegister = false)
@SpringBootTest(webEnvironment = SpringBootTest.WebEnvironment.RANDOM_PORT)
public class IncomingEventControllerTest extends AbstractTestNGSpringContextTests {
    @Autowired
    private TestRestTemplate restTemplate;

    @Test
    public void postNewInternalEvent_shouldReturnNoContent() {
        EventRecord eventRecord = createEventRecord(Alert, "1", "OK", "context", "/o/r/i/g/i/n/o/f/c/o/n/d/i/t/i/o/n");
        Event eventArray = createEvent(ImmutableList.of(eventRecord));
        ResponseEntity<JsonNode> postEventResponse = restTemplate.postForEntity("/redfish/v1/EventService/Events", eventArray, JsonNode.class);
        assertThat(postEventResponse.getStatusCode()).isEqualTo(HttpStatus.NO_CONTENT);
    }

    @Test
    public void postBrokenInternalEvent_shouldReturnBadRequest() {
        Event envelope = new Event();
        ResponseEntity<JsonNode> postEventResponse = restTemplate.postForEntity("/redfish/v1/EventService/Events", envelope, JsonNode.class);
        assertThat(postEventResponse.getStatusCode()).isEqualTo(HttpStatus.BAD_REQUEST);
    }
}
