/*
 * Copyright (c) 2017 Intel Corporation
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

package com.intel.podm.business.entities.interceptors;

import com.intel.podm.business.entities.redfish.Chassis;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.Drive;
import com.intel.podm.business.entities.redfish.EthernetSwitchPortVlan;
import com.intel.podm.business.entities.redfish.base.Entity;
import com.intel.podm.business.entities.types.RedfishEvent;
import org.mockito.Mockito;
import org.mockito.internal.util.reflection.Whitebox;
import org.testng.annotations.Test;

import java.net.URI;
import java.util.List;

import static com.google.common.collect.Lists.newArrayList;
import static com.intel.podm.common.types.events.EventType.ALERT;
import static com.intel.podm.common.types.events.EventType.RESOURCE_ADDED;
import static com.intel.podm.common.types.events.EventType.RESOURCE_REMOVED;
import static com.intel.podm.common.types.events.EventType.RESOURCE_UPDATED;
import static com.intel.podm.common.types.events.EventType.STATUS_CHANGE;
import static org.hamcrest.MatcherAssert.assertThat;
import static org.hamcrest.Matchers.allOf;
import static org.hamcrest.Matchers.equalTo;
import static org.hamcrest.Matchers.hasItem;
import static org.hamcrest.Matchers.hasItems;
import static org.hamcrest.Matchers.hasSize;
import static org.hamcrest.beans.HasPropertyWithValue.hasProperty;
import static org.mockito.Mockito.CALLS_REAL_METHODS;

@SuppressWarnings({"checkstyle:MethodName", "checkstyle:MagicNumber"})
public class EntityEventCollectorTest {

    private static final String URI_PROPERTY = "uri";
    private static final String EVENT_TYPE_PROPERTY = "eventType";

    private EntityEventCollector collector = new EntityEventCollector();
    private EventTypeConverter eventTypeConverter = new EventTypeConverter();
    private ByEventSourceContextGroupingConverter converter = new ByEventSourceContextGroupingConverter();

    @Test
    public void whenThereWasSingleResourceAddedEventCollected_shouldBeIncludedInOutputResult() {
        Chassis chassis = entity(Chassis.class, 1);
        collector.add(chassis, RESOURCE_ADDED);

        collector.convertEventTypes(eventTypeConverter);
        List<RedfishEvent> output = collector.convertThenEvict(converter);
        assertThat(output, hasSize(1));
        assertThat(
            output,
            hasItem(
                allOf(
                    hasProperty(URI_PROPERTY, equalTo(chassis.getEventSourceContext())),
                    hasProperty(EVENT_TYPE_PROPERTY, equalTo(RESOURCE_ADDED)))
            )
        );
    }

    @Test
    public void whenThereIsSingleResourceRemovedEventCollected_shouldBeIncludedInOutputResult() {
        ComputerSystem computerSystem = entity(ComputerSystem.class, 1);
        collector.add(computerSystem, RESOURCE_REMOVED);
        collector.convertEventTypes(eventTypeConverter);
        List<RedfishEvent> output = collector.convertThenEvict(converter);
        assertThat(output, hasSize(1));
        assertThat(
            output,
            hasItem(
                allOf(
                    hasProperty(URI_PROPERTY, equalTo(computerSystem.getEventSourceContext())),
                    hasProperty(EVENT_TYPE_PROPERTY, equalTo(RESOURCE_REMOVED))
                )
            )
        );
    }

    @Test
    public void whenThereWereManyEventsCollectedTogetherWithResourceRemoved_shouldBeProperlyModifiedInOutputResult() {
        ComputerSystem computerSystem = entity(ComputerSystem.class, 1);
        collector.add(computerSystem, RESOURCE_REMOVED);
        collector.add(computerSystem, RESOURCE_UPDATED);
        collector.add(computerSystem, ALERT);
        collector.convertEventTypes(eventTypeConverter);
        List<RedfishEvent> output = collector.convertThenEvict(converter);
        assertThat(output, hasSize(1));
        assertThat(
            output,
            hasItem(
                allOf(hasProperty(URI_PROPERTY, equalTo(computerSystem.getEventSourceContext())), hasProperty(EVENT_TYPE_PROPERTY, equalTo(RESOURCE_REMOVED)))
            )
        );
    }

    @Test
    public void whenThereWereManyEventsCollectedTogetherWithResourceAdded_shouldBeProperlyModifiedInOutputResult() {
        Chassis chassis = entity(Chassis.class, 1);
        collector.add(chassis, RESOURCE_ADDED);
        collector.add(chassis, RESOURCE_UPDATED);
        collector.add(chassis, ALERT);
        collector.convertEventTypes(eventTypeConverter);
        List<RedfishEvent> output = collector.convertThenEvict(converter);
        assertThat(output, hasSize(1));
        assertThat(output,
            hasItem(
                allOf(hasProperty(URI_PROPERTY, equalTo(chassis.getEventSourceContext())), hasProperty(EVENT_TYPE_PROPERTY, equalTo(RESOURCE_ADDED))))
        );
    }

    @Test
    public void whenThereWereMultipleEvents_shouldProperlyAppearInOutputResult() {
        Chassis chassis = entity(Chassis.class, 1);
        EthernetSwitchPortVlan vlan = entity(EthernetSwitchPortVlan.class, 1);
        Drive drive = entity(Drive.class, 1);
        ComputerSystem system = entity(ComputerSystem.class, 1);

        collector.add(chassis, newArrayList(RESOURCE_ADDED, RESOURCE_UPDATED, ALERT));
        collector.add(vlan, newArrayList(RESOURCE_UPDATED, RESOURCE_REMOVED, STATUS_CHANGE));
        collector.add(drive, newArrayList(ALERT, RESOURCE_UPDATED, STATUS_CHANGE));
        collector.add(system, RESOURCE_UPDATED);
        collector.convertEventTypes(eventTypeConverter);
        List<RedfishEvent> output = collector.convertThenEvict(converter);
        assertThat(output, hasSize(6));
        assertThat(
            output,
            hasItems(
                allOf(hasProperty(URI_PROPERTY, equalTo(chassis.getEventSourceContext())), hasProperty(EVENT_TYPE_PROPERTY, equalTo(RESOURCE_ADDED))),
                allOf(hasProperty(URI_PROPERTY, equalTo(vlan.getEventSourceContext())), hasProperty(EVENT_TYPE_PROPERTY, equalTo(RESOURCE_REMOVED))),
                allOf(hasProperty(URI_PROPERTY, equalTo(drive.getEventSourceContext())), hasProperty(EVENT_TYPE_PROPERTY, equalTo(ALERT))),
                allOf(hasProperty(URI_PROPERTY, equalTo(drive.getEventSourceContext())), hasProperty(EVENT_TYPE_PROPERTY, equalTo(RESOURCE_UPDATED))),
                allOf(hasProperty(URI_PROPERTY, equalTo(drive.getEventSourceContext())), hasProperty(EVENT_TYPE_PROPERTY, equalTo(STATUS_CHANGE))),
                allOf(hasProperty(URI_PROPERTY, equalTo(system.getEventSourceContext())), hasProperty(EVENT_TYPE_PROPERTY, equalTo(RESOURCE_UPDATED)))
            ));
    }

    private static <A extends Entity> A entity(Class<A> aClass, long id) {
        A mock = Mockito.mock(aClass, CALLS_REAL_METHODS);
        Whitebox.setInternalState(mock, "id", id);
        Whitebox.setInternalState(mock, "eventSourceContext", URI.create(aClass.getSimpleName()));

        return mock;
    }
}
