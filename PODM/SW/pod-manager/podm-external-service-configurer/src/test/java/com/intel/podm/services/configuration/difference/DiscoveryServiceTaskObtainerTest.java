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

package com.intel.podm.services.configuration.difference;

import com.intel.podm.business.entities.redfish.Endpoint;
import com.intel.podm.client.WebClient;
import com.intel.podm.client.WebClientRequestException;
import com.intel.podm.client.reader.ExternalServiceReader;
import com.intel.podm.client.reader.ExternalServiceReaderFactory;
import com.intel.podm.client.reader.ResourceSupplier;
import com.intel.podm.client.resources.ExternalServiceResource;
import com.intel.podm.client.resources.redfish.EndpointResource;
import com.intel.podm.client.resources.redfish.ZoneResource;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.types.Id;
import com.intel.podm.services.configuration.tasks.CreateZoneTask;
import com.intel.podm.services.configuration.tasks.DeleteZoneTask;
import com.intel.podm.services.configuration.tasks.DiscoveryServiceTask;
import com.intel.podm.services.configuration.tasks.DiscoveryServiceTaskObtainer;
import com.intel.podm.services.configuration.tasks.UpdateZoneTask;
import org.mockito.InjectMocks;
import org.mockito.Mock;
import org.mockito.MockitoAnnotations;
import org.testng.annotations.BeforeTest;
import org.testng.annotations.Test;

import java.util.ArrayList;
import java.util.Collection;
import java.util.List;
import java.util.Map;
import java.util.UUID;

import static com.google.common.collect.Maps.newHashMap;
import static com.intel.podm.common.types.Protocol.NVME;
import static com.intel.podm.services.configuration.MockedWebClientBuilder.asResourceSupplierIterable;
import static com.intel.podm.services.configuration.MockedWebClientBuilder.createMockedWebClientWithZones;
import static com.intel.podm.services.configuration.difference.factory.MockedEndpointBuilder.V_4_ADDRESS_1;
import static com.intel.podm.services.configuration.difference.factory.MockedEndpointBuilder.V_4_ADDRESS_2;
import static com.intel.podm.services.configuration.difference.factory.MockedEndpointBuilder.V_6_ADDRESS_1;
import static com.intel.podm.services.configuration.difference.factory.MockedEndpointBuilder.V_6_ADDRESS_2;
import static com.intel.podm.services.configuration.difference.factory.MockedEndpointBuilder.createDefaultEndpointBuilder;
import static com.intel.podm.services.configuration.difference.factory.MockedEndpointBuilder.createEmptyEndpointBuilder;
import static com.intel.podm.services.configuration.difference.factory.MockedEndpointResourcesBuilder.createDefaultEndpointResourceBuilder;
import static com.intel.podm.services.configuration.difference.factory.MockedEndpointResourcesBuilder.createEmptyEndpointResourceBuilder;
import static java.util.Arrays.asList;
import static java.util.Collections.singletonList;
import static java.util.UUID.randomUUID;
import static org.apache.commons.net.ftp.FTPCommand.PORT;
import static org.mockito.Matchers.anyObject;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.spy;
import static org.mockito.Mockito.when;
import static org.testng.Assert.assertEquals;

@SuppressWarnings({"checkstyle:MethodName", "MagicNumber", "checkstyle:ClassFanOutComplexity"})
public class DiscoveryServiceTaskObtainerTest {
    @InjectMocks
    DiscoveryServiceTaskObtainer discoveryServiceTaskObtainer;

    @InjectMocks
    EndpointCollectionsDifferenceExtractor endpointCollectionsDifferenceExtractor = spy(EndpointCollectionsDifferenceExtractor.class);

    @Mock
    ExternalServiceReaderFactory externalServiceReaderFactory;

    @Mock
    private Logger logger;

    @BeforeTest
    public void initMocks() {
        WebClient httpClient = mock(WebClient.class);
        ExternalServiceReader externalServiceReader = new ExternalServiceReader(httpClient);
        MockitoAnnotations.initMocks(this);
        when(externalServiceReaderFactory.createExternalServiceReaderWithCacheAndRetries(anyObject())).thenReturn(externalServiceReader);
    }

    @Test
    public void checkChangesToBeApplied_zonesAreEqual_noChangesToBeApplied() throws WebClientRequestException {
        UUID uuid = randomUUID();
        Map<UUID, Collection<Endpoint>> expectedComputerSystemsWithEndpoints = newHashMap();
        Map<UUID, ZoneResource> existingComputerSystemsWithZones = newHashMap();

        expectedComputerSystemsWithEndpoints.put(uuid, singletonList(createDefaultEndpointBuilder().build()));
        ZoneResource zoneResource = buildZoneWithEndpoints(singletonList(createDefaultEndpointResourceBuilder().build()));
        existingComputerSystemsWithZones.put(uuid, zoneResource);

        List<DiscoveryServiceTask> result = discoveryServiceTaskObtainer.getListOfTasks(
            createMockedWebClientWithZones(zoneResource),
            expectedComputerSystemsWithEndpoints,
            existingComputerSystemsWithZones
        );

        assertEquals(result.size(), 0);
    }

    @Test
    public void checkChangesToBeApplied_moveDefaultEndpointToDifferentComputerSystem_twoChangesToBeApplied() throws WebClientRequestException {
        UUID someSystemUuid = randomUUID();
        UUID differentSystemUuid = randomUUID();
        Map<UUID, Collection<Endpoint>> expectedComputerSystemsWithEndpoints = newHashMap();
        Map<UUID, ZoneResource> existingComputerSystemsWithZones = newHashMap();

        expectedComputerSystemsWithEndpoints.put(someSystemUuid, singletonList(createDefaultEndpointBuilder().build()));
        ZoneResource zoneResource = buildZoneWithEndpoints(singletonList(createDefaultEndpointResourceBuilder().build()));
        existingComputerSystemsWithZones.put(differentSystemUuid, zoneResource);

        List<DiscoveryServiceTask> result = discoveryServiceTaskObtainer.getListOfTasks(
            createMockedWebClientWithZones(zoneResource),
            expectedComputerSystemsWithEndpoints,
            existingComputerSystemsWithZones
        );

        assertEquals(result.size(), 2);
        assertEquals(getActionTypeCount(result, CreateZoneTask.class), 1);
        assertEquals(getActionTypeCount(result, DeleteZoneTask.class), 1);
    }

    @Test
    public void checkChangesToBeApplied_addDefaultEndpointToComputerSystem_oneChangeToBeApplied() throws WebClientRequestException {
        UUID uuid = randomUUID();
        Map<UUID, Collection<Endpoint>> expectedComputerSystemsWithEndpoints = newHashMap();
        Map<UUID, ZoneResource> existingComputerSystemsWithZones = newHashMap();

        expectedComputerSystemsWithEndpoints.put(uuid, singletonList(createDefaultEndpointBuilder().build()));
        ZoneResource zoneResource = buildZoneWithEndpoints(new ArrayList<>());
        existingComputerSystemsWithZones.put(uuid, zoneResource);

        List<DiscoveryServiceTask> result = discoveryServiceTaskObtainer.getListOfTasks(
            createMockedWebClientWithZones(zoneResource),
            expectedComputerSystemsWithEndpoints,
            existingComputerSystemsWithZones
        );

        assertEquals(result.size(), 1);
        assertEquals(getActionTypeCount(result, UpdateZoneTask.class), 1);
    }

    @Test
    public void checkChangesToBeApplied_modifyEndpointInComputerSystem_oneChangeToBeApplied() throws WebClientRequestException {
        UUID uuid = randomUUID();
        Map<UUID, Collection<Endpoint>> expectedComputerSystemsWithEndpoints = newHashMap();
        Map<UUID, ZoneResource> existingComputerSystemsWithZones = newHashMap();

        expectedComputerSystemsWithEndpoints.put(
            uuid,
            singletonList(createEmptyEndpointBuilder().addTransport(NVME, V_6_ADDRESS_1, V_4_ADDRESS_1, PORT, Id.id("100")).build())
        );
        ZoneResource zoneResource = buildZoneWithEndpoints(
            singletonList(createEmptyEndpointResourceBuilder().addTransport(NVME, V_6_ADDRESS_2, V_4_ADDRESS_2, PORT, "100").build())
        );
        existingComputerSystemsWithZones.put(uuid, zoneResource);

        List<DiscoveryServiceTask> result = discoveryServiceTaskObtainer.getListOfTasks(
            createMockedWebClientWithZones(zoneResource),
            expectedComputerSystemsWithEndpoints,
            existingComputerSystemsWithZones
        );

        assertEquals(result.size(), 1);
        assertEquals(getActionTypeCount(result, UpdateZoneTask.class), 1);
    }

    @Test
    public void checkChangesToBeApplied_addNewEndpointToComputerSystem_oneChangeToBeApplied() throws WebClientRequestException {
        UUID uuid = randomUUID();
        Map<UUID, Collection<Endpoint>> expectedComputerSystemsWithEndpoints = newHashMap();
        Map<UUID, ZoneResource> existingComputerSystemsWithZones = newHashMap();

        expectedComputerSystemsWithEndpoints.put(
            uuid,
            singletonList(createDefaultEndpointBuilder().build())
        );
        ZoneResource zoneResource = buildZoneWithEndpoints(asList(
            createDefaultEndpointResourceBuilder().build(),
            createEmptyEndpointResourceBuilder().addTransport(NVME, V_6_ADDRESS_1, V_4_ADDRESS_1, PORT, "100").build()
        ));
        existingComputerSystemsWithZones.put(uuid, zoneResource);

        List<DiscoveryServiceTask> result = discoveryServiceTaskObtainer.getListOfTasks(
            createMockedWebClientWithZones(zoneResource),
            expectedComputerSystemsWithEndpoints,
            existingComputerSystemsWithZones
        );

        assertEquals(result.size(), 1);
        assertEquals(getActionTypeCount(result, UpdateZoneTask.class), 1);
    }

    @Test
    public void checkChangesToBeApplied_addTwoEndpointsToDifferentComputerSystems_twoChangeToBeApplied() throws WebClientRequestException {
        Map<UUID, Collection<Endpoint>> expectedComputerSystemsWithEndpoints = newHashMap();
        Map<UUID, ZoneResource> existingComputerSystemsWithZones = newHashMap();

        UUID someSystemUuid = randomUUID();
        expectedComputerSystemsWithEndpoints.put(
            someSystemUuid,
            singletonList(createDefaultEndpointBuilder().build())
        );

        UUID differentSystemUuid = randomUUID();
        expectedComputerSystemsWithEndpoints.put(
            differentSystemUuid,
            singletonList(createEmptyEndpointBuilder().addTransport(NVME, V_6_ADDRESS_1, V_4_ADDRESS_1, PORT, Id.id("100")).build())
        );

        List<DiscoveryServiceTask> result = discoveryServiceTaskObtainer.getListOfTasks(
            createMockedWebClientWithZones(),
            expectedComputerSystemsWithEndpoints,
            existingComputerSystemsWithZones
        );
        assertEquals(result.size(), 2);
        assertEquals(getActionTypeCount(result, CreateZoneTask.class), 2);
    }

    @Test
    @SuppressWarnings({"checkstyle:MethodLength"})
    public void checkChangesToBeApplied_modifyThreeComputerSystemsWithEndpoints_threeChangeToBeApplied() throws Exception {
        Map<UUID, Collection<Endpoint>> expectedComputerSystemsWithEndpoints = newHashMap();
        Map<UUID, ZoneResource> existingComputerSystemsWithZones = newHashMap();

        UUID firstSystemUuid = randomUUID();
        expectedComputerSystemsWithEndpoints.put(
            firstSystemUuid,
            singletonList(createDefaultEndpointBuilder().build())
        );

        UUID secondSystemUuid = randomUUID();
        expectedComputerSystemsWithEndpoints.put(
            secondSystemUuid,
            singletonList(createEmptyEndpointBuilder().addTransport(NVME, V_6_ADDRESS_1, V_4_ADDRESS_1, PORT, Id.id("100")).build())
        );

        UUID thirdSystemUuid = randomUUID();
        ZoneResource zoneResourceOne = buildZoneWithEndpoints(
            singletonList(createEmptyEndpointResourceBuilder().addTransport(NVME, V_6_ADDRESS_1, V_4_ADDRESS_2, PORT, "100").build())
        );
        existingComputerSystemsWithZones.put(firstSystemUuid, zoneResourceOne);
        ZoneResource zoneResourceTwo = buildZoneWithEndpoints(
            singletonList(createEmptyEndpointResourceBuilder().addTransport(NVME, V_6_ADDRESS_2, V_4_ADDRESS_1, PORT, "100").build())
        );
        existingComputerSystemsWithZones.put(thirdSystemUuid, zoneResourceTwo);

        List<DiscoveryServiceTask> result = discoveryServiceTaskObtainer.getListOfTasks(
            createMockedWebClientWithZones(zoneResourceOne, zoneResourceTwo),
            expectedComputerSystemsWithEndpoints,
            existingComputerSystemsWithZones
        );

        assertEquals(result.size(), 3);
        assertEquals(getActionTypeCount(result, CreateZoneTask.class), 1);
        assertEquals(getActionTypeCount(result, UpdateZoneTask.class), 1);
        assertEquals(getActionTypeCount(result, DeleteZoneTask.class), 1);
    }

    @Test
    @SuppressWarnings({"checkstyle:MethodLength"})
    public void checkChangesToBeApplied_checkBuildingHashFromIdInEquator_noChangeToBeApplied() throws Exception {
        UUID uuid = randomUUID();
        Map<UUID, Collection<Endpoint>> expectedComputerSystemsWithEndpoints = newHashMap();
        expectedComputerSystemsWithEndpoints.put(
            uuid,
            asList(
                createEmptyEndpointBuilder().addTransport(NVME, V_6_ADDRESS_1, V_4_ADDRESS_1, PORT, Id.id("102")).build(),
                createEmptyEndpointBuilder().addTransport(NVME, V_6_ADDRESS_1, V_4_ADDRESS_2, PORT, Id.id("101")).build()
            )
        );

        Map<UUID, ZoneResource> existingComputerSystemsWithZones = newHashMap();
        ZoneResource zoneResource = buildZoneWithEndpoints(
            asList(
                createEmptyEndpointResourceBuilder().addTransport(NVME, V_6_ADDRESS_1, V_4_ADDRESS_1, PORT, "103").build(),
                createEmptyEndpointResourceBuilder().addTransport(NVME, V_6_ADDRESS_1, V_4_ADDRESS_2, PORT, "104").build()
            )
        );
        existingComputerSystemsWithZones.put(uuid, zoneResource);

        List<DiscoveryServiceTask> result = discoveryServiceTaskObtainer.getListOfTasks(
            createMockedWebClientWithZones(zoneResource),
            expectedComputerSystemsWithEndpoints,
            existingComputerSystemsWithZones
        );

        assertEquals(result.size(), 0);
    }

    private int getActionTypeCount(List<DiscoveryServiceTask> result, Class<?> createZoneTaskClass) {
        return Math.toIntExact(result.stream().filter(obj -> obj.getClass().equals(createZoneTaskClass)).count());
    }

    private ZoneResource buildZoneWithEndpoints(List<EndpointResource> endpoints) {
        return new ZoneResource() {
            @Override
            public Iterable<ResourceSupplier> getEndpoints() throws WebClientRequestException {
                return asResourceSupplierIterable(endpoints.toArray(new ExternalServiceResource[endpoints.size()]));
            }
        };
    }
}
