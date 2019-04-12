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
import com.fasterxml.jackson.databind.node.ObjectNode;
import com.fasterxml.jackson.databind.node.TextNode;
import com.intel.rsd.dto.events.EventType;
import com.intel.rsd.json.ArrayNodeCollector;
import com.intel.rsd.resourcemanager.layers.EventHandler;
import lombok.val;
import org.mockito.ArgumentCaptor;
import org.testng.annotations.BeforeMethod;
import org.testng.annotations.Test;

import java.util.Arrays;

import static com.intel.rsd.dto.events.EventType.ResourceAdded;
import static com.intel.rsd.json.JsonUtils.newObjectNode;
import static java.util.UUID.randomUUID;
import static org.assertj.core.api.Java6Assertions.assertThat;
import static org.mockito.Matchers.any;
import static org.mockito.Mockito.never;
import static org.mockito.Mockito.reset;
import static org.mockito.Mockito.spy;
import static org.mockito.Mockito.times;
import static org.mockito.Mockito.verify;
import static org.testng.Assert.assertFalse;

public class EventsUpstreamControllerTest {

    private EventsUpstreamController sut;

    private EventHandler eventHandler;

    @BeforeMethod
    public void setUp() {
        this.eventHandler = spy(EventHandler.class);
        this.sut = new EventsUpstreamController(eventHandler);
    }

    @Test
    public void odataIdExposedOnProvidedEventShouldBeRemoved() {
        val event = createEvent();
        sut.onEventReceived(randomServiceId(), event);
        assertFalse(event.has("@odata.id"));
    }

    @Test
    public void eventWithMissingEventRecordsCollectionShouldPassedToEventHandler() {
        val event = newObjectNode();
        sut.onEventReceived(randomServiceId(), event);
        assertThatEventWasNotPassedToEventHandler();
    }

    @Test
    public void eventRecordWithoutOdataIdShouldBeFiltered() {
        val record = newObjectNode().set("anyField", TextNode.valueOf("anyValue"));
        val event = createEvent(record);
        sut.onEventReceived(randomServiceId(), event);
        assertThat(event.withArray("Events")).doesNotContain(record);
        assertThatEventWasNotPassedToEventHandler();
    }

    @Test
    public void eventRecordWithoutEventTypeShouldBeFiltered() {
        val record = newObjectNode().set("@odata.id", TextNode.valueOf("/foo/bar"));
        val event = createEvent(record);
        sut.onEventReceived(randomServiceId(), event);
        assertThat(event.withArray("Events")).doesNotContain(record);
        assertThatEventWasNotPassedToEventHandler();
    }

    @Test
    public void completeEventRecordShouldNotBeFiltered() {
        val record = createEventRecord("/foo/bar", ResourceAdded);
        val event = createEvent(record);
        sut.onEventReceived(randomServiceId(), event);
        assertThat(event.withArray("Events")).contains(record);
        assertThatEventWasPassedToEventHandler();
    }

    private JsonNode createEventRecord(String odataId, EventType eventType) {
        val eventRecord = newObjectNode();
        eventRecord.set("OriginOfCondition", newObjectNode().set("@odata.id", TextNode.valueOf(odataId)));
        eventRecord.set("EventType", TextNode.valueOf(eventType.name()));
        return eventRecord;
    }

    private ObjectNode createEvent(JsonNode... records) {
        val eventRecordsArray = Arrays.stream(records).collect(new ArrayNodeCollector());
        val event = newObjectNode();
        event.set("@odata.id", TextNode.valueOf("/some/uri"));
        event.set("Events", eventRecordsArray);
        return event;
    }

    private String randomServiceId() {
        return randomUUID().toString();
    }

    private void assertThatEventWasPassedToEventHandler() {
        val captor = ArgumentCaptor.forClass(JsonNode.class);
        verify(eventHandler, times(1)).onEvent(any(), captor.capture());
        reset(eventHandler);
    }

    private void assertThatEventWasNotPassedToEventHandler() {
        val captor = ArgumentCaptor.forClass(JsonNode.class);
        verify(eventHandler, never()).onEvent(any(), captor.capture());
        reset(eventHandler);
    }
}
