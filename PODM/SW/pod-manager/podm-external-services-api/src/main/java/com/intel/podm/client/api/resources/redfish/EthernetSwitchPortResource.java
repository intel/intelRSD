/*
 * Copyright (c) 2016-2017 Intel Corporation
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

package com.intel.podm.client.api.resources.redfish;

import com.intel.podm.client.api.ExternalServiceApiReaderException;
import com.intel.podm.client.api.reader.ResourceSupplier;
import com.intel.podm.client.api.resources.ExternalServiceResource;
import com.intel.podm.common.types.AdministrativeState;
import com.intel.podm.common.types.LinkType;
import com.intel.podm.common.types.NeighborInfo;
import com.intel.podm.common.types.OperationalState;
import com.intel.podm.common.types.PortClass;
import com.intel.podm.common.types.PortMode;
import com.intel.podm.common.types.PortType;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.net.MacAddress;

import java.util.List;

@SuppressWarnings({"checkstyle:ClassFanOutComplexity", "checkstyle:MethodCount"})
public interface EthernetSwitchPortResource extends ExternalServiceResource {
    String getPortId();
    Status getStatus();
    LinkType getLinkType();
    OperationalState getOperationalState();
    AdministrativeState getAdministrativeState();
    Integer getLinkSpeedMbps();
    NeighborInfo getNeighborInfo();
    MacAddress getNeighborMac();
    Integer getFrameSize();
    Boolean getAutosense();
    Boolean getFullDuplex();
    MacAddress getMacAddress();
    List<IpV4AddressObject> getIpV4Addresses();
    List<IpV6AddressObject> getIpV6Addresses();
    PortClass getPortClass();
    PortMode getPortMode();
    PortType getPortType();

    Iterable<ResourceSupplier> getVlans() throws ExternalServiceApiReaderException;
    ResourceSupplier getPrimaryVlan();
    Iterable<ResourceSupplier> getPortMembers() throws ExternalServiceApiReaderException;
}
