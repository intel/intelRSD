/*
 * Copyright (c) 2018 Intel Corporation
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

package com.intel.podm.services.configuration.client;

import com.intel.podm.client.WebClient;
import com.intel.podm.client.resources.redfish.EndpointResource;
import com.intel.podm.client.resources.redfish.ZoneResource;
import com.intel.podm.services.configuration.ComputerSystemUuidExtractor;
import org.apache.commons.collections4.BidiMap;
import org.apache.commons.collections4.bidimap.DualHashBidiMap;
import org.mockito.InjectMocks;
import org.mockito.MockitoAnnotations;
import org.mockito.Spy;
import org.testng.annotations.BeforeMethod;
import org.testng.annotations.BeforeTest;
import org.testng.annotations.Test;

import java.net.URI;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.UUID;

import static com.intel.podm.common.types.DurableNameFormat.NQN;
import static com.intel.podm.common.types.EntityRole.INITIATOR;
import static com.intel.podm.common.utils.IterableHelper.single;
import static com.intel.podm.services.configuration.ComputerSystemUuidExtractor.NQN_PREFIX;
import static com.intel.podm.services.configuration.MockedWebClientBuilder.asResourceSupplierIterable;
import static com.intel.podm.services.configuration.MockedWebClientBuilder.createMockedWebClientWithZones;
import static com.intel.podm.services.configuration.difference.factory.MockedEndpointResourcesBuilder.createEmptyEndpointResourceBuilder;
import static java.lang.String.format;
import static java.util.Arrays.asList;
import static java.util.Collections.emptyList;
import static java.util.UUID.randomUUID;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;
import static org.testng.Assert.assertEquals;
import static org.testng.Assert.assertTrue;

@SuppressWarnings({"checkstyle:MethodName", "checkstyle:ClassFanOutComplexity"})
public class InitiatorFinderTest {
    @Spy
    private ComputerSystemUuidExtractor computerSystemUuidExtractor;

    @InjectMocks
    private InitiatorFinder initiatorFinder;

    private List<UUID> initiatorsUuids = new ArrayList<>();

    @BeforeTest
    public void initMocks() {
        MockitoAnnotations.initMocks(this);
    }

    @BeforeMethod
    public void configureTest() {
        initiatorsUuids.clear();
    }

    @Test
    public void createUuidToZoneResourceMappingWithoutInitiatorEndpoint_shouldReturnEmptyMap() throws Exception {
        ZoneResource zoneOne = mock(ZoneResource.class);
        when(zoneOne.getUri()).thenReturn(URI.create("/redfish/v1/Fabrics/1/Zones/1"));
        when(zoneOne.getEndpoints()).thenReturn(emptyList());

        WebClient mockedClient = createMockedWebClientWithZones(zoneOne);
        Map<UUID, ZoneResource> map = initiatorFinder.createComputerSystemUuidWithInitiatorEndpointMapping(mockedClient);

        assertTrue(map.isEmpty());
    }

    @Test
    public void createUuidToZoneResourceMappingForOneZone_shouldReturnCorrectMap() throws Exception {
        ZoneResource zoneOne = mock(ZoneResource.class);
        when(zoneOne.getUri()).thenReturn(URI.create("/redfish/v1/Fabrics/1/Zones/1"));
        when(zoneOne.getEndpoints()).thenReturn(asResourceSupplierIterable(createInitiatorEndpointResource()));

        WebClient mockedClient = createMockedWebClientWithZones(zoneOne);
        Map<UUID, ZoneResource> map = initiatorFinder.createComputerSystemUuidWithInitiatorEndpointMapping(mockedClient);
        assertEquals(map.size(), 1);

        UUID systemUuid = single(map.keySet());
        assertTrue(initiatorsUuids.contains(systemUuid));
        assertTrue(map.get(systemUuid).equals(zoneOne));
    }

    @Test
    @SuppressWarnings({"checkstyle:MethodLength", "checkstyle:ExecutableStatementCount"})
    public void createUuidToZoneResourceMappingForMultipleZones_shouldReturnCorrectMap() throws Exception {
        ZoneResource zoneOne = mock(ZoneResource.class);
        when(zoneOne.getUri()).thenReturn(URI.create("/redfish/v1/Fabrics/1/Zones/1"));
        when(zoneOne.getEndpoints()).thenReturn(asResourceSupplierIterable(createInitiatorEndpointResource()));

        ZoneResource zoneTwo = mock(ZoneResource.class);
        when(zoneOne.getUri()).thenReturn(URI.create("/redfish/v1/Fabrics/1/Zones/2"));
        when(zoneTwo.getEndpoints()).thenReturn(asResourceSupplierIterable(createInitiatorEndpointResource()));

        ZoneResource zoneThree = mock(ZoneResource.class);
        when(zoneOne.getUri()).thenReturn(URI.create("/redfish/v1/Fabrics/1/Zones/3"));
        when(zoneThree.getEndpoints()).thenReturn(asResourceSupplierIterable(createInitiatorEndpointResource()));

        List<ZoneResource> zoneResources = asList(zoneOne, zoneTwo, zoneThree);
        WebClient mockedClient = createMockedWebClientWithZones(zoneOne, zoneTwo, zoneThree);

        Map<UUID, ZoneResource> map = initiatorFinder.createComputerSystemUuidWithInitiatorEndpointMapping(mockedClient);
        assertEquals(map.size(), zoneResources.size());
        assertTrue(map.keySet().containsAll(initiatorsUuids));

        BidiMap<UUID, ZoneResource> bidirectionalMap = new DualHashBidiMap<>(map);

        for (ZoneResource resource : zoneResources) {
            UUID uuid = bidirectionalMap.inverseBidiMap().get(resource);
            assertTrue(initiatorsUuids.contains(uuid));
            initiatorsUuids.remove(uuid);
        }
        assertEquals(initiatorsUuids.size(), 0);
    }

    @Test
    public void createUuidToZoneResourceMappingWithoutInitiatorEndpointInOneZone_shouldReturnCorrectMap() throws Exception {
        ZoneResource zoneOne = mock(ZoneResource.class);
        when(zoneOne.getUri()).thenReturn(URI.create("/redfish/v1/Fabrics/1/Zones/1"));
        when(zoneOne.getEndpoints()).thenReturn(emptyList());

        ZoneResource zoneTwo = mock(ZoneResource.class);
        when(zoneOne.getUri()).thenReturn(URI.create("/redfish/v1/Fabrics/1/Zones/2"));
        when(zoneTwo.getEndpoints()).thenReturn(asResourceSupplierIterable(createInitiatorEndpointResource()));

        List<ZoneResource> zoneResources = asList(zoneOne, zoneTwo);
        WebClient mockedClient = createMockedWebClientWithZones(zoneOne, zoneTwo);

        Map<UUID, ZoneResource> map = initiatorFinder.createComputerSystemUuidWithInitiatorEndpointMapping(mockedClient);
        assertEquals(map.size(), 1);

        UUID systemUuid = single(map.keySet());
        assertTrue(initiatorsUuids.contains(systemUuid));
        assertTrue(zoneResources.contains(map.get(systemUuid)));
    }

    private EndpointResource createInitiatorEndpointResource() {
        return createEmptyEndpointResourceBuilder()
            .addIdentifier(getNvmeDurableNameWithRandomUuid(), NQN)
            .addConnectedEntityWithRole(INITIATOR)
            .build();
    }

    private String getNvmeDurableNameWithRandomUuid() {
        UUID uuid = randomUUID();
        initiatorsUuids.add(uuid);

        return format("%s:%s", NQN_PREFIX, uuid);
    }
}
