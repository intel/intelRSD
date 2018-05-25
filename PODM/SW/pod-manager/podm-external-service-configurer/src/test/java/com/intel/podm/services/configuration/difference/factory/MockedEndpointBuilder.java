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

import com.intel.podm.business.entities.redfish.Endpoint;
import com.intel.podm.business.entities.redfish.Fabric;
import com.intel.podm.business.entities.redfish.IpTransportDetails;
import com.intel.podm.business.entities.redfish.base.ConnectedEntity;
import com.intel.podm.business.entities.redfish.embeddables.Identifier;
import com.intel.podm.business.entities.redfish.embeddables.IpV4Address;
import com.intel.podm.business.entities.redfish.embeddables.IpV6Address;
import com.intel.podm.common.types.DurableNameFormat;
import com.intel.podm.common.types.EntityRole;
import com.intel.podm.common.types.Id;
import com.intel.podm.common.types.Protocol;

import static com.intel.podm.common.types.Id.id;

import java.util.Objects;

import static com.intel.podm.common.types.DurableNameFormat.NQN;
import static com.intel.podm.common.types.EntityRole.TARGET;
import static com.intel.podm.common.types.Protocol.NVME;
import static com.intel.podm.services.configuration.ComputerSystemUuidExtractor.NQN_PREFIX;
import static java.lang.String.format;
import static java.util.Objects.hash;
import static java.util.UUID.randomUUID;
import static org.apache.commons.lang3.RandomUtils.nextLong;

@SuppressWarnings({"checkstyle:MagicNumber", "checkstyle:ClassFanOutComplexity"})
public final class MockedEndpointBuilder {

    public static final String V_6_ADDRESS_1 = "FE80::0202:B3FF:FE1E:8320";
    public static final String V_6_ADDRESS_2 = "FE80::0202:B3FF:FE1E:8321";
    public static final String V_6_ADDRESS_3 = "FE80::0202:B3FF:FE1E:8323";
    public static final String V_4_ADDRESS_1 = "1.1.1.4";
    public static final String V_4_ADDRESS_2 = "2.1.1.4";
    public static final String V_4_ADDRESS_3 = "3.1.1.4";
    private static final String V_4_ADDRESS = "1.1.1.1";
    private static final String V_6_ADDRESS = "FFFF::FFFF:FFFF:FFFF:FFFF";
    private static final int PORT = 666;
    private Endpoint endpoint;

    @SuppressWarnings({"checkstyle:AnonInnerLength"})
    private MockedEndpointBuilder() {
        endpoint = new Endpoint() {
            @Override
            public int hashCode() {
                return hash(getId());
            }

            @Override
            public boolean equals(Object o) {
                if (this == o) {
                    return true;
                }
                if (o == null || (!(o instanceof Endpoint))) {
                    return false;
                }
                Endpoint that = (Endpoint) o;
                return Objects.equals(getId(), that.getId());
            }
        };
        endpoint.setId(id(nextLong()));
    }

    public static MockedEndpointBuilder createEmptyEndpointBuilder() {
        return new MockedEndpointBuilder();
    }

    public static MockedEndpointBuilder createDefaultEndpointBuilder() {
        return new MockedEndpointBuilder()
            .addConnectedEntityWithRole(TARGET)
            .addIdentifier("nqn.2014-08.org.nvmexpress:1234", NQN)
            .addTransport(NVME, V_6_ADDRESS, V_4_ADDRESS, PORT, Id.id(100));
    }

    public Endpoint build() {
        return endpoint;
    }

    public static MockedEndpointBuilder createNvmeTargetEndpointWithRandomIdentifierUuid() {
        return new MockedEndpointBuilder()
            .addConnectedEntityWithRole(TARGET)
            .addIdentifier(format("%s:%s", NQN_PREFIX, randomUUID()), NQN);
    }

    public MockedEndpointBuilder addConnectedEntityWithRole(EntityRole role) {
        ConnectedEntity connectedEntityExample = new ConnectedEntity();
        connectedEntityExample.setEntityRole(role);
        endpoint.addConnectedEntity(connectedEntityExample);
        return this;
    }

    public MockedEndpointBuilder addIdentifier(String durableName, DurableNameFormat durableFormat) {
        Identifier identifier = new Identifier();
        identifier.setDurableNameFormat(durableFormat);
        identifier.setDurableName(durableName);
        endpoint.addIdentifier(identifier);
        return this;
    }

    public MockedEndpointBuilder addTransport(Protocol protocol, String v6Address, String v4Address, Integer port, Id id) {
        IpTransportDetails transportExample = new IpTransportDetails();
        transportExample.setTransportProtocol(protocol);
        IpV6Address ipV6Address = new IpV6Address();
        ipV6Address.setAddress(v6Address);
        IpV4Address ipV4Address = new IpV4Address();
        ipV4Address.setAddress(v4Address);
        transportExample.setIpv4Address(ipV4Address);
        transportExample.setIpv6Address(ipV6Address);
        transportExample.setPort(port);
        transportExample.setId(id);
        endpoint.addTransport(transportExample);
        return this;
    }

    public MockedEndpointBuilder addFabric(Protocol fabricProtocol) {
        Fabric fabric = new Fabric();
        fabric.addEndpoint(this.endpoint);
        fabric.setFabricType(fabricProtocol);
        return this;
    }

}


