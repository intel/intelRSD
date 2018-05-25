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

import com.intel.podm.business.entities.dao.GenericDao;
import com.intel.podm.business.entities.redfish.ComposedNode;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.Endpoint;
import com.intel.podm.business.entities.redfish.Zone;
import com.intel.podm.services.configuration.ComposedNodeEndpointsFinder;
import com.intel.podm.services.configuration.ComputerSystemUuidExtractor;
import org.mockito.InjectMocks;
import org.mockito.Mock;
import org.mockito.MockitoAnnotations;
import org.mockito.Spy;
import org.testng.annotations.BeforeMethod;
import org.testng.annotations.BeforeTest;
import org.testng.annotations.Test;

import java.util.ArrayList;
import java.util.Collection;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.UUID;

import static com.intel.podm.common.types.DurableNameFormat.NQN;
import static com.intel.podm.common.types.EntityRole.INITIATOR;
import static com.intel.podm.common.types.EntityRole.TARGET;
import static com.intel.podm.common.types.Protocol.NVME_OVER_FABRICS;
import static com.intel.podm.common.utils.Collector.toSingle;
import static com.intel.podm.common.utils.IterableHelper.single;
import static com.intel.podm.services.configuration.ComputerSystemUuidExtractor.NQN_PREFIX;
import static com.intel.podm.services.configuration.difference.factory.MockedEndpointBuilder.createEmptyEndpointBuilder;
import static com.intel.podm.services.configuration.difference.factory.MockedEndpointBuilder.createNvmeTargetEndpointWithRandomIdentifierUuid;
import static java.lang.String.format;
import static java.util.Arrays.asList;
import static java.util.Collections.singletonList;
import static java.util.UUID.randomUUID;
import static java.util.stream.Collectors.toSet;
import static org.mockito.Mockito.doReturn;
import static org.mockito.Mockito.mock;
import static org.testng.Assert.assertEquals;
import static org.testng.Assert.assertTrue;

@SuppressWarnings({"checkstyle:MethodName", "checkstyle:MagicNumber"})
public class ComposedNodeEndpointsFinderTest {
    @Mock
    private GenericDao genericDao;

    @Spy
    private ComputerSystemUuidExtractor computerSystemUuidExtractor;

    @InjectMocks
    private ComposedNodeEndpointsFinder composedNodeEndpointsFinder;

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
    public void createUuidToZoneMappingWithOneZoneOnComputerSystem_shouldReturnCorrectMap() {
        ComposedNode composedNode = createComposedNodeWithNumberOfZonesOnComputerSystem(1);
        Set<Endpoint> endpoints = composedNode.getComputerSystem().getEndpoints().stream()
            .map(Endpoint::getZone)
            .flatMap(zone -> zone.getEndpoints().stream())
            .collect(toSet());
        doReturn(singletonList(composedNode)).when(genericDao).findAll(ComposedNode.class);

        Map<UUID, Collection<Endpoint>> map = composedNodeEndpointsFinder.createComputerSystemUuidWithEndpointCollectionMapping();

        UUID systemUuid = single(map.keySet());
        assertTrue(initiatorsUuids.contains(systemUuid));
        assertTrue(endpoints.containsAll(map.get(systemUuid)));
    }

    @Test
    public void createUuidToZoneMappingWithMultipleZonesOnComputerSystem_shouldReturnCorrectMap() {
        ComposedNode composedNode = createComposedNodeWithNumberOfZonesOnComputerSystem(3);
        doReturn(singletonList(composedNode)).when(genericDao).findAll(ComposedNode.class);

        Set<Endpoint> targetEndpoints = composedNode.getComputerSystem().getEndpoints().stream()
            .map(Endpoint::getZone)
            .flatMap(zone -> zone.getEndpoints().stream())
            .filter(endpoint -> endpoint.hasRole(TARGET))
            .collect(toSet());

        Map<UUID, Collection<Endpoint>> map = composedNodeEndpointsFinder.createComputerSystemUuidWithEndpointCollectionMapping();

        UUID systemUuid = single(map.keySet());
        Collection<Endpoint> endpoints = map.get(systemUuid);
        assertTrue(initiatorsUuids.contains(systemUuid));
        assertEquals(endpoints.size(), targetEndpoints.size() + 1);
        assertTrue(endpoints.containsAll(targetEndpoints));
    }

    @Test
    public void createUuidToZoneMappingWithMultipleZonesOnMultipleComputerSystems_shouldReturnCorrectMap() {
        List<ComposedNode> nodes = createNumberOfComposedNodesWithNumberOfZonesOnComputerSystem(4, 4);
        nodes.add(createComposedNodeWithNumberOfZonesOnComputerSystem(1));
        doReturn(nodes).when(genericDao).findAll(ComposedNode.class);

        Set<Endpoint> targetEndpoints = nodes.stream()
            .map(ComposedNode::getComputerSystem)
            .flatMap(computerSystem -> computerSystem.getEndpoints().stream())
            .map(Endpoint::getZone)
            .flatMap(zone -> zone.getEndpoints().stream())
            .filter(endpoint -> endpoint.hasRole(TARGET))
            .collect(toSet());

        Map<UUID, Collection<Endpoint>> map = composedNodeEndpointsFinder.createComputerSystemUuidWithEndpointCollectionMapping();

        assertEquals(map.size(), nodes.size());
        assertTrue(initiatorsUuids.containsAll(map.keySet()));

        Set<Endpoint> endpointsInMap = map.values().stream()
            .flatMap(Collection::stream)
            .collect(toSet());
        assertEquals(endpointsInMap.size(), targetEndpoints.size() + map.size());
        assertTrue(endpointsInMap.containsAll(targetEndpoints));
    }

    private List<ComposedNode> createNumberOfComposedNodesWithNumberOfZonesOnComputerSystem(int numberOfZones, int numberOfNodes) {
        List<ComposedNode> nodes = new ArrayList<>();
        for (int i = 0; i < numberOfNodes; i++) {
            ComposedNode composedNode = mock(ComposedNode.class);
            doReturn(mockComputerSystemWithNumberOfZones(numberOfZones)).when(composedNode).getComputerSystem();
            nodes.add(composedNode);
        }

        return nodes;
    }

    private ComposedNode createComposedNodeWithNumberOfZonesOnComputerSystem(int numberOfZones) {
        ComposedNode composedNode = mock(ComposedNode.class);
        doReturn(mockComputerSystemWithNumberOfZones(numberOfZones)).when(composedNode).getComputerSystem();

        return composedNode;
    }

    private ComputerSystem mockComputerSystemWithNumberOfZones(int numberOfZones) {
        ComputerSystem computerSystem = mock(ComputerSystem.class);
        Set<Endpoint> initiators = new HashSet<>();

        for (int i = 0; i < numberOfZones; i++) {
            initiators.add(retrieveInitiatorFromZone(mockZoneWithEndpoints()));
        }
        doReturn(initiators).when(computerSystem).getEndpoints();

        return computerSystem;
    }

    private Zone mockZoneWithEndpoints() {
        Zone zone = mock(Zone.class);

        Endpoint initiator = createEmptyEndpointBuilder()
            .addFabric(NVME_OVER_FABRICS)
            .addIdentifier(getNvmeDurableNameWithRandomUuid(), NQN)
            .addConnectedEntityWithRole(INITIATOR)
            .build();

        initiator.setZone(zone);
        Set<Endpoint> endpoints = new HashSet<>(asList(
            initiator,
            createNvmeTargetEndpointWithRandomIdentifierUuid().build(),
            createNvmeTargetEndpointWithRandomIdentifierUuid().build()));
        doReturn(endpoints).when(zone).getEndpoints();

        return zone;
    }

    private Endpoint retrieveInitiatorFromZone(Zone zone) {
        return zone.getEndpoints().stream()
            .filter(endpoint -> endpoint.hasRole(INITIATOR))
            .collect(toSingle());
    }

    private String getNvmeDurableNameWithRandomUuid() {
        UUID uuid = randomUUID();
        initiatorsUuids.add(uuid);

        return format("%s:%s", NQN_PREFIX, uuid);
    }
}
