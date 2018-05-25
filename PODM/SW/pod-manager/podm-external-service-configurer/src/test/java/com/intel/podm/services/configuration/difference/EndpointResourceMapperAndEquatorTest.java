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

package com.intel.podm.services.configuration.difference;

import com.intel.podm.business.entities.redfish.Endpoint;
import com.intel.podm.client.resources.redfish.EndpointResource;
import org.mockito.InjectMocks;
import org.mockito.MockitoAnnotations;
import org.testng.annotations.BeforeTest;
import org.testng.annotations.Test;

import static com.intel.podm.common.types.DurableNameFormat.NQN;
import static com.intel.podm.services.configuration.difference.EndpointBuilder.asEndpoint;
import static com.intel.podm.services.configuration.difference.factory.MockedEndpointBuilder.createDefaultEndpointBuilder;
import static com.intel.podm.services.configuration.difference.factory.MockedEndpointBuilder.createEmptyEndpointBuilder;
import static com.intel.podm.services.configuration.difference.factory.MockedEndpointResourcesBuilder.createDefaultEndpointResourceBuilder;
import static com.intel.podm.services.configuration.difference.factory.MockedEndpointResourcesBuilder.createEmptyEndpointResourceBuilder;
import static java.util.Collections.singleton;
import static org.apache.commons.collections4.CollectionUtils.isEqualCollection;
import static org.testng.Assert.assertFalse;
import static org.testng.Assert.assertTrue;

@SuppressWarnings({
    "checkstyle:MethodName", "checkstyle:MethodLength", "checkstyle:MagicNumber"
})
public class EndpointResourceMapperAndEquatorTest {

    private static final int NEW_PORT = 9996;
    private static final EndpointEquator EQUATOR = new EndpointEquator();
    @InjectMocks
    private EndpointBuilder mapper;

    @BeforeTest
    public void initMocks() {
        MockitoAnnotations.initMocks(this);
    }

    @Test
    public void checkExamplePropertiesFromMapper_shouldBeEqual() {
        EndpointResource resource = createDefaultEndpointResourceBuilder().build();
        Endpoint mappedEndpoint = asEndpoint(resource);

        String durableName = resource.getIdentifiers().stream().findFirst().get().getDurableName();
        String mappedDurableName = mappedEndpoint.getIdentifiers().stream().findFirst().get().getDurableName();

        assertTrue(durableName.equals(mappedDurableName));
    }

    @Test
    public void checkEndpointEquator_endpointsShouldBeEqual() {
        Endpoint endpointExample = createDefaultEndpointBuilder().build();

        assertTrue(isEqualCollection(
            singleton(endpointExample),
            singleton(endpointExample),
            EQUATOR));
    }

    @Test
    public void checkEndpointMapperAndEndpointEquator_resourcesShouldBeEqual() {
        Endpoint endpoint = createDefaultEndpointBuilder().build();
        EndpointResource resource = createDefaultEndpointResourceBuilder().build();

        Endpoint mappedEndpoint = asEndpoint(resource);

        assertTrue(isEqualCollection(
            singleton(mappedEndpoint),
            singleton(endpoint),
            EQUATOR));
    }

    @Test
    public void checkEndpointMapperAndEndpointEquator_resourcesShouldDifferent() {
        EndpointResource resource = createEmptyEndpointResourceBuilder().addIdentifier("iqn", NQN).build();
        Endpoint endpoint = createEmptyEndpointBuilder().addIdentifier("different_iqn", NQN).build();

        Endpoint mappedEndpoint = asEndpoint(resource);

        assertFalse(isEqualCollection(
            singleton(mappedEndpoint),
            singleton(endpoint),
            EQUATOR));
    }
}
