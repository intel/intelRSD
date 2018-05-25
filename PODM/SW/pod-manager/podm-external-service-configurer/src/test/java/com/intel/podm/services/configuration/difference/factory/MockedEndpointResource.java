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

import com.intel.podm.client.resources.redfish.ConnectedEntityResource;
import com.intel.podm.client.resources.redfish.EndpointResource;
import com.intel.podm.client.resources.redfish.IdentifierObject;
import com.intel.podm.client.resources.redfish.IpTransportDetailsResource;
import com.intel.podm.client.resources.redfish.IpV4AddressObject;
import com.intel.podm.client.resources.redfish.IpV6AddressObject;
import com.intel.podm.common.types.DurableNameFormat;
import com.intel.podm.common.types.EntityRole;
import com.intel.podm.common.types.Protocol;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

public class MockedEndpointResource extends EndpointResource {

    private Set<IdentifierObject> identifierObjects = new HashSet<>();
    private List<ConnectedEntityResource> connectedEntities = new ArrayList<>();
    private List<IpTransportDetailsResource> mockedTransportResources = new ArrayList<>();

    public MockedEndpointResource addIdentifier(String name, DurableNameFormat format) {
        identifierObjects.add(
            new IdentifierObject() {
                @Override
                public DurableNameFormat getDurableNameFormat() {
                    return format;
                }

                @Override
                public String getDurableName() {
                    return name;
                }
            });
        return this;
    }

    public MockedEndpointResource addConnectedEntity(EntityRole role) {
        connectedEntities.add(
            new ConnectedEntityResource() {
                @Override
                public EntityRole getEntityRole() {
                    return role;
                }

            });
        return this;
    }

    @SuppressWarnings({"checkstyle:AnonInnerLength", "checkstyle:MethodLength"})
    public MockedEndpointResource addTransport(Integer port, String v6Address, String v4Address, Protocol protocol, String id) {
        mockedTransportResources.add(
            new IpTransportDetailsResource() {
                @Override
                public Protocol getTransportProtocol() {
                    return protocol;
                }

                @Override
                public IpV4AddressObject getIpv4Address() {
                    return new IpV4AddressObject() {
                        @Override
                        public String getAddress() {
                            return v4Address;
                        }
                    };
                }

                @Override
                public IpV6AddressObject getIpv6Address() {
                    return new IpV6AddressObject() {
                        @Override
                        public String getAddress() {
                            return v6Address;
                        }
                    };
                }

                @Override
                public Integer getPort() {
                    return port;
                }

                @Override
                public String getId() {
                    return id;
                }
            });
        return this;
    }

    @Override
    public List<ConnectedEntityResource> getConnectedEntityResourceList() {
        return connectedEntities;
    }

    @Override
    public Set<IdentifierObject> getIdentifiers() {
        return identifierObjects;
    }

    @Override
    public List<IpTransportDetailsResource> getTransportResourceList() {
        return mockedTransportResources;
    }
}
