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
import com.intel.podm.client.WebClientRequestException;
import com.intel.podm.client.resources.redfish.EndpointResource;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.types.Id;
import com.intel.podm.services.configuration.difference.factory.MockedEndpointResource;
import org.mockito.InjectMocks;
import org.mockito.Mock;
import org.mockito.MockitoAnnotations;
import org.testng.annotations.BeforeTest;
import org.testng.annotations.Test;

import java.util.List;

import static com.intel.podm.common.types.Protocol.NVME;
import static com.intel.podm.services.configuration.difference.factory.MockedEndpointBuilder.V_4_ADDRESS_1;
import static com.intel.podm.services.configuration.difference.factory.MockedEndpointBuilder.V_4_ADDRESS_2;
import static com.intel.podm.services.configuration.difference.factory.MockedEndpointBuilder.V_4_ADDRESS_3;
import static com.intel.podm.services.configuration.difference.factory.MockedEndpointBuilder.V_6_ADDRESS_1;
import static com.intel.podm.services.configuration.difference.factory.MockedEndpointBuilder.V_6_ADDRESS_2;
import static com.intel.podm.services.configuration.difference.factory.MockedEndpointBuilder.V_6_ADDRESS_3;
import static com.intel.podm.services.configuration.difference.factory.MockedEndpointBuilder.createDefaultEndpointBuilder;
import static com.intel.podm.services.configuration.difference.factory.MockedEndpointBuilder.createEmptyEndpointBuilder;
import static com.intel.podm.services.configuration.difference.factory.MockedEndpointResourcesBuilder.createDefaultEndpointResourceBuilder;
import static com.intel.podm.services.configuration.difference.factory.MockedEndpointResourcesBuilder.createEmptyEndpointResourceBuilder;
import static java.util.Arrays.asList;
import static java.util.Collections.singletonList;
import static org.apache.commons.net.ftp.FTPCommand.PORT;
import static org.testng.Assert.assertEquals;

@SuppressWarnings({"checkstyle:MethodName", "MagicNumber"})
public class EndpointCollectionsDifferenceExtractorTest {
    @InjectMocks
    EndpointCollectionsDifferenceExtractor differenceExtractor;

    @Mock
    private Logger logger;


    @BeforeTest
    public void initMocks() throws WebClientRequestException {
        MockitoAnnotations.initMocks(this);
    }

    @Test
    public void checkEndpointMatcher_emptyLists_noChanges() {
        List<Endpoint> expectedComposedNodeEndpoints = singletonList(createDefaultEndpointBuilder().build());
        List<EndpointResource> existingEndpointResources = singletonList(createDefaultEndpointResourceBuilder().build());

        List<Endpoint> endpointsToAdd = differenceExtractor.getEndpointsToAdd(expectedComposedNodeEndpoints, existingEndpointResources);
        List<Endpoint> endpointsToRemove = differenceExtractor.getEndpointsToAdd(expectedComposedNodeEndpoints, existingEndpointResources);

        assertEquals(endpointsToAdd.size(), 0, "endpointsToAdd");
        assertEquals(endpointsToRemove.size(), 0, "endpointsToRemove");
    }

    @Test
    public void checkEndpointMatcher_modifyOneEndpoint_addOneEndpointAndOneEndpointToRemove() {

        List<Endpoint> expectedComposedNodeEndpoints = asList(
            createEmptyEndpointBuilder().addTransport(NVME, V_6_ADDRESS_1, V_4_ADDRESS_1, PORT, Id.id("100")).build(),
            createEmptyEndpointBuilder().addTransport(NVME, V_6_ADDRESS_2, V_4_ADDRESS_1, PORT, Id.id("100")).build(),
            createDefaultEndpointBuilder().build());
        List<EndpointResource> existingEndpointResources = asList(
            createEmptyEndpointResourceBuilder().addTransport(NVME, V_6_ADDRESS_1, V_4_ADDRESS_1, PORT, "100").build(),
            createEmptyEndpointResourceBuilder().addTransport(NVME, V_6_ADDRESS_1, V_4_ADDRESS_2, PORT, "100").build(),
            createDefaultEndpointResourceBuilder().build());

        List<Endpoint> endpointsToAdd = differenceExtractor.getEndpointsToAdd(expectedComposedNodeEndpoints, existingEndpointResources);
        List<Endpoint> endpointsToRemove = differenceExtractor.getEndpointsToRemove(expectedComposedNodeEndpoints, existingEndpointResources);

        assertEquals(endpointsToAdd.size(), 1, "endpointsToAdd");
        assertEquals(endpointsToRemove.size(), 1, "endpointsToRemove");
    }

    @Test
    public void checkEndpointMatcher_duplicateListOfEndpoints_threeOneEndpointAndTwoEndpointToRemove() {
        Endpoint endpoint = createEmptyEndpointBuilder().addTransport(NVME, V_6_ADDRESS_1, V_4_ADDRESS_1, PORT, Id.id("100")).build();
        List<Endpoint> expectedComposedNodeEndpoints = asList(endpoint, endpoint,
            createEmptyEndpointBuilder().addTransport(NVME, V_6_ADDRESS_2, V_4_ADDRESS_1, PORT, Id.id("100")).build(),
            createEmptyEndpointBuilder().addTransport(NVME, V_6_ADDRESS_3, V_4_ADDRESS_1, PORT, Id.id("100")).build());

        MockedEndpointResource mockedEndpointResource = createEmptyEndpointResourceBuilder().
            addTransport(NVME, V_6_ADDRESS_1, V_4_ADDRESS_2, PORT, "100").build();
        List<EndpointResource> existingEndpointResources = asList(mockedEndpointResource, mockedEndpointResource,
            createEmptyEndpointResourceBuilder().addTransport(NVME, V_6_ADDRESS_1, V_4_ADDRESS_3, PORT, "100").build());

        List<Endpoint> endpointsToAdd = differenceExtractor.getEndpointsToAdd(expectedComposedNodeEndpoints, existingEndpointResources);
        List<Endpoint> endpointsToRemove = differenceExtractor.getEndpointsToRemove(expectedComposedNodeEndpoints, existingEndpointResources);

        assertEquals(endpointsToAdd.size(), 3, "endpointsToAdd");
        assertEquals(endpointsToRemove.size(), 2, "endpointsToRemove");
    }
}
