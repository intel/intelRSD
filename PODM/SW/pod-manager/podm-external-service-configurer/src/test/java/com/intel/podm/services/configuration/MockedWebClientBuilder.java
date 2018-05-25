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

package com.intel.podm.services.configuration;

import com.intel.podm.client.WebClient;
import com.intel.podm.client.WebClientRequestException;
import com.intel.podm.client.reader.ResourceSupplier;
import com.intel.podm.client.resources.ExternalServiceResource;
import com.intel.podm.client.resources.redfish.FabricResource;
import com.intel.podm.client.resources.redfish.ServiceRootResource;
import com.intel.podm.client.resources.redfish.ZoneResource;

import java.net.URI;

import static java.util.Arrays.stream;
import static java.util.stream.Collectors.toList;
import static org.mockito.Matchers.anyObject;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;

public final class MockedWebClientBuilder {
    private MockedWebClientBuilder() {
    }

    public static WebClient createMockedWebClientWithZones(ZoneResource... zones) throws WebClientRequestException {
        FabricResource mockedFabric = mock(FabricResource.class);
        when(mockedFabric.getUri()).thenReturn(URI.create("/redfish/v1/Fabrics/1"));
        when(mockedFabric.getZones()).thenReturn(asResourceSupplierIterable(zones));

        ServiceRootResource mockedRoot = mock(ServiceRootResource.class);
        when(mockedRoot.getFabrics()).thenReturn(asResourceSupplierIterable(mockedFabric));

        WebClient mockedClient = mock(WebClient.class);
        when(mockedClient.getBaseUri()).thenReturn(URI.create("/redfish/v1"));
        when(mockedClient.get(anyObject())).thenReturn(mockedRoot);

        return mockedClient;
    }

    public static Iterable<ResourceSupplier> asResourceSupplierIterable(ExternalServiceResource... zoneResources) {
        return stream(zoneResources).map(zoneResource -> new ResourceSupplier() {
            @Override
            public ExternalServiceResource get() throws WebClientRequestException {
                return zoneResource;
            }

            @Override
            public URI getUri() {
                return zoneResource.getUri();
            }
        }).collect(toList());
    }
}
