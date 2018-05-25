/*
 * Copyright (c) 2017-2018 Intel Corporation
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

package com.intel.podm.eventing.observers;

import com.intel.podm.business.entities.redfish.EventSubscription;
import com.intel.podm.business.entities.types.RedfishEvent;
import com.intel.podm.common.types.Pair;
import com.intel.podm.common.types.events.EventType;
import org.testng.annotations.DataProvider;
import org.testng.annotations.Test;

import java.net.URI;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;

import static com.google.common.collect.Lists.newArrayList;
import static com.google.common.collect.Sets.newHashSet;
import static com.intel.podm.common.types.Pair.pairOf;
import static com.intel.podm.common.types.events.EventType.ALERT;
import static com.intel.podm.common.types.events.EventType.RESOURCE_ADDED;
import static com.intel.podm.common.types.events.EventType.RESOURCE_REMOVED;
import static com.intel.podm.eventing.observers.EventDispatcher.dispatch;
import static java.util.Collections.emptyList;
import static java.util.Collections.emptySet;
import static java.util.Collections.singletonList;
import static org.hamcrest.MatcherAssert.assertThat;
import static org.hamcrest.Matchers.everyItem;
import static org.hamcrest.Matchers.isIn;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;
import static org.testng.Assert.assertEquals;

@SuppressWarnings({"checkstyle:MagicNumber", "checkstyle:MethodName"})
public class EventDispatcherTest {

    @DataProvider
    @SuppressWarnings({"checkstyle:MethodLength"})
    public static Object[][] dataProvider() {
        RedfishEvent addedChassis3 = event(RESOURCE_ADDED, URI.create("/redfish/v1/Chassis/3"));
        RedfishEvent alertNode7 = event(ALERT, URI.create("/redfish/v1/Nodes/7"));
        RedfishEvent alertChassis7 = event(ALERT, URI.create("/redfish/v1/Chassis/7"));
        RedfishEvent removedChassis6 = event(RESOURCE_REMOVED, URI.create("/redfish/v1/Chassis/6"));
        RedfishEvent addedNode1 = event(RESOURCE_ADDED, URI.create("/redfish/v1/Nodes/1"));
        RedfishEvent addedNode22 = event(RESOURCE_ADDED, URI.create("/redfish/v1/Nodes/22"));

        EventSubscription addedRemovedConsumer = subscription(newArrayList(RESOURCE_ADDED, RESOURCE_REMOVED), emptyList());
        EventSubscription alertConsumer = subscription(newArrayList(ALERT), emptyList());
        EventSubscription addedNode1Consumer = subscription(newArrayList(RESOURCE_ADDED), singletonList("/redfish/v1/Nodes/1"));
        EventSubscription removedChassis6Consumer = subscription(newArrayList(RESOURCE_REMOVED), singletonList("/redfish/v1/Chassis/6"));
        return new Object[][]{
            {
                pairOf(
                    newHashSet(addedChassis3, alertNode7, alertChassis7, removedChassis6, addedNode1, addedNode22),
                    newHashSet(addedRemovedConsumer, alertConsumer, addedNode1Consumer, removedChassis6Consumer)
                ),
                new HashMap<EventSubscription, Set<RedfishEvent>>() {{
                    put(alertConsumer, newHashSet(alertChassis7, alertNode7));
                    put(addedRemovedConsumer, newHashSet(addedNode1, addedNode22, removedChassis6, addedChassis3));
                    put(addedNode1Consumer, newHashSet(addedNode1));
                    put(removedChassis6Consumer, newHashSet(removedChassis6));
                }}
            }
        };
    }

    private static RedfishEvent event(EventType eventType, URI uri) {
        return new RedfishEvent(uri, eventType);
    }

    private static EventSubscription subscription(List<EventType> eventTypes, List<String> originResources) {
        EventSubscription mock = mock(EventSubscription.class);
        when(mock.getEventTypes()).thenReturn(eventTypes);
        when(mock.getOriginResources()).thenReturn(originResources);
        return mock;
    }

    @Test(dataProvider = "dataProvider")
    public void whenThereAreRegisteredConsumers_shouldProperlyDispatchEvents(
        Pair<Set<RedfishEvent>, Set<EventSubscription>> testData, Map<EventSubscription, Set<RedfishEvent>> expectedResults
    ) {
        Map<EventSubscription, Set<RedfishEvent>> dispatched = dispatch(testData.first(), testData.second());

        assertThat(dispatched.entrySet(), everyItem(isIn(expectedResults.entrySet())));
        assertThat(expectedResults.entrySet(), everyItem(isIn(dispatched.entrySet())));
    }

    @Test
    public void whenThereAreNoConsumers_shouldReturnEmptyMap() {
        Map<EventSubscription, Set<RedfishEvent>> dispatched = dispatch(
            newHashSet(
                event(RESOURCE_ADDED, URI.create("/redfish/v1/Chassis/2"))
            ),
            emptySet()
        );

        assertEquals(dispatched.entrySet().size(), 0);
    }
}
