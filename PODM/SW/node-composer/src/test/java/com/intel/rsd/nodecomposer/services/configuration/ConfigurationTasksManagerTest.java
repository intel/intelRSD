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

package com.intel.rsd.nodecomposer.services.configuration;

import com.intel.rsd.nodecomposer.services.configuration.client.DiscoveryServiceWebClient;
import com.intel.rsd.nodecomposer.types.discovery.DiscoveryServiceEndpoint;
import com.intel.rsd.nodecomposer.utils.beans.NodeComposerBeanFactory;
import lombok.val;
import org.springframework.scheduling.TaskScheduler;
import org.testng.annotations.Test;

import java.net.URI;
import java.util.Date;
import java.util.HashMap;
import java.util.Map;
import java.util.UUID;
import java.util.concurrent.ScheduledFuture;

import static java.util.UUID.randomUUID;
import static org.mockito.Matchers.any;
import static org.mockito.Matchers.anyLong;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;
import static org.testng.Assert.assertFalse;
import static org.testng.Assert.assertTrue;

public class ConfigurationTasksManagerTest {
    @Test
    public void whenProvidedWithDiscoveryEndpoint_shouldStoreConfigurationTaskWithMatchingUuid() {
        val sut = getSystemUnderTest();
        val uuid = randomUUID();
        sut.scheduleConfigurationTasks(getDiscoveryServiceEndpointsMapForUuids(uuid));

        assertTrue(sut.configurationTasks.containsKey(uuid));
    }

    @Test
    void whenProvidedConsecutivelyWithDifferentEndpointUuids_shouldRemoveFirstAndAddSecond() {
        val sut = getSystemUnderTest();

        val firstUuid = randomUUID();
        sut.scheduleConfigurationTasks(getDiscoveryServiceEndpointsMapForUuids(firstUuid));

        val secondUuid = randomUUID();
        sut.scheduleConfigurationTasks(getDiscoveryServiceEndpointsMapForUuids(secondUuid));

        assertFalse(sut.configurationTasks.containsKey(firstUuid));
        assertTrue(sut.configurationTasks.containsKey(secondUuid));
    }

    @Test
    void whenProvidedConsecutivelyWithDisjointSetOfEndpoints_shouldKeepOnlyLatestUuids() {
        val sut = getSystemUnderTest();
        val firstUuid = randomUUID();
        val secondUuid = randomUUID();
        sut.scheduleConfigurationTasks(getDiscoveryServiceEndpointsMapForUuids(firstUuid, secondUuid));

        val thirdUuid = randomUUID();
        val fourthUuid = randomUUID();
        sut.scheduleConfigurationTasks(getDiscoveryServiceEndpointsMapForUuids(thirdUuid, fourthUuid));

        assertFalse(sut.configurationTasks.containsKey(firstUuid));
        assertFalse(sut.configurationTasks.containsKey(secondUuid));
        assertTrue(sut.configurationTasks.containsKey(thirdUuid));
        assertTrue(sut.configurationTasks.containsKey(fourthUuid));
    }

    @Test
    void whenProvidedConsecutivelyWithOverlappingSetOfEndpoints_shouldKeepOnlyLatestUuids() {
        val sut = getSystemUnderTest();
        val firstUuid = randomUUID();
        val secondUuid = randomUUID();
        sut.scheduleConfigurationTasks(getDiscoveryServiceEndpointsMapForUuids(firstUuid, secondUuid));

        val thirdUuid = randomUUID();
        sut.scheduleConfigurationTasks(getDiscoveryServiceEndpointsMapForUuids(secondUuid, thirdUuid));

        assertFalse(sut.configurationTasks.containsKey(firstUuid));
        assertTrue(sut.configurationTasks.containsKey(secondUuid));
        assertTrue(sut.configurationTasks.containsKey(thirdUuid));
    }

    private Map<UUID, DiscoveryServiceEndpoint> getDiscoveryServiceEndpointsMapForUuids(UUID... uuids) {
        val endpoints = new HashMap<UUID, DiscoveryServiceEndpoint>();
        for (UUID uuid : uuids) {
            endpoints.put(uuid, new DiscoveryServiceEndpoint(uuid, URI.create("http://discovery-service-" + uuid + "/redfish/v1")));
        }
        return endpoints;
    }

    @SuppressWarnings({"checkstyle:MagicNumber", "unchecked"})
    private ConfigurationTasksManager getSystemUnderTest() {
        val mockedTaskScheduler = mock(TaskScheduler.class);
        when(mockedTaskScheduler.scheduleWithFixedDelay(any(DiscoveryServiceConfigurationTask.class), any(Date.class), anyLong()))
            .thenReturn(mock(ScheduledFuture.class));
        DiscoveryServiceConfigurationTask task = new DiscoveryServiceConfigurationTask(mock(DiscoveryServiceConfigurer.class),
            mock(DiscoveryServiceWebClient.class));
        val mockedBeanFactory = mock(NodeComposerBeanFactory.class);
        when(mockedBeanFactory.create(DiscoveryServiceConfigurationTask.class)).thenReturn(task);

        return new ConfigurationTasksManager(30L, mockedTaskScheduler, mockedBeanFactory);
    }
}
