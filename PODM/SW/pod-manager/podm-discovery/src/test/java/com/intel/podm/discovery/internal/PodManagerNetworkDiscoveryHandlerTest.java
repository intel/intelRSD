/*
 * Copyright (c) 2015-2017 Intel Corporation
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

package com.intel.podm.discovery.internal;

import com.intel.podm.business.entities.redfish.Manager;
import org.testng.annotations.Test;

import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.verify;

@SuppressWarnings({"checkstyle:MethodName"})
public class PodManagerNetworkDiscoveryHandlerTest {

    @Test
    public void initializeNetworking_shouldAddNetworkInterfaces() {
        PodManagerNetworkDiscoveryHandler sut = getPodManagerNetworkDiscoveryHandler();
        Manager manager = mock(Manager.class);
        sut.initializeNetworking(manager);
        verify(sut.podManagerEthernetInterfaceDiscoveryHandler).addNetworkInterfaces(manager);
    }

    @Test
    public void initializeNetworking_shouldAddNetworkService() {
        PodManagerNetworkDiscoveryHandler sut = getPodManagerNetworkDiscoveryHandler();

        Manager manager = mock(Manager.class);
        sut.initializeNetworking(manager);

        verify(sut.podManagerNetworkServiceDiscoveryHandler).addNetworkService(manager);
    }

    private PodManagerNetworkDiscoveryHandler getPodManagerNetworkDiscoveryHandler() {
        PodManagerNetworkDiscoveryHandler handler = new PodManagerNetworkDiscoveryHandler();
        handler.podManagerEthernetInterfaceDiscoveryHandler = mock(PodManagerEthernetInterfaceDiscoveryHandler.class);
        handler.podManagerNetworkServiceDiscoveryHandler = mock(PodManagerNetworkServiceDiscoveryHandler.class);
        return handler;
    }
}
