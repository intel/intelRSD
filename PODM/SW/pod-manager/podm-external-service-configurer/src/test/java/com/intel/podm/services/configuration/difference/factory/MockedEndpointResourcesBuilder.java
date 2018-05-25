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

package com.intel.podm.services.configuration.difference.factory;

import com.intel.podm.common.types.DurableNameFormat;
import com.intel.podm.common.types.EntityRole;
import com.intel.podm.common.types.Protocol;

import static com.intel.podm.common.types.DurableNameFormat.NQN;
import static com.intel.podm.common.types.EntityRole.TARGET;
import static com.intel.podm.common.types.Protocol.NVME;

public class MockedEndpointResourcesBuilder {

    private static final String V_4_ADDRESS = "1.1.1.1";
    private static final int PORT = 666;
    private static final String V_6_ADDRESS = "FFFF::FFFF:FFFF:FFFF:FFFF";
    private MockedEndpointResource mockedEndpointResource = new MockedEndpointResource();

    public static MockedEndpointResourcesBuilder createEmptyEndpointResourceBuilder() {
        return new MockedEndpointResourcesBuilder();
    }

    public static MockedEndpointResourcesBuilder createDefaultEndpointResourceBuilder() {
        return new MockedEndpointResourcesBuilder()
            .addConnectedEntityWithRole(TARGET)
            .addIdentifier("nqn.2014-08.org.nvmexpress:1234", NQN)
            .addTransport(NVME, V_6_ADDRESS, V_4_ADDRESS, PORT, "100");
    }

    public MockedEndpointResource build() {
        return mockedEndpointResource;
    }

    public MockedEndpointResourcesBuilder addConnectedEntityWithRole(EntityRole role) {
        mockedEndpointResource.addConnectedEntity(role);
        return this;
    }

    public MockedEndpointResourcesBuilder addIdentifier(String durableName, DurableNameFormat durableFormat) {
        mockedEndpointResource.addIdentifier(durableName, durableFormat);
        return this;
    }

    public MockedEndpointResourcesBuilder addTransport(Protocol protocol, String v6Address, String v4Address, Integer port, String id) {
        mockedEndpointResource.addTransport(port, v6Address, v4Address, protocol, id);
        return this;
    }
}
