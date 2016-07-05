/*
 * Copyright (c) 2016 Intel Corporation
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

package com.intel.podm.mappers.redfish;

import com.intel.podm.business.entities.dao.GenericDao;
import com.intel.podm.business.entities.redfish.EthernetSwitchPort;
import com.intel.podm.business.entities.redfish.properties.IpV4Address;
import com.intel.podm.business.entities.redfish.properties.IpV6Address;
import com.intel.podm.client.api.resources.redfish.EthernetSwitchPortResource;
import com.intel.podm.client.api.resources.redfish.IpV4AddressObject;
import com.intel.podm.client.api.resources.redfish.IpV6AddressObject;
import com.intel.podm.mappers.DomainObjectMapper;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;

@Dependent
public class EthernetSwitchPortMapper extends DomainObjectMapper<EthernetSwitchPortResource, EthernetSwitchPort> {

    @Inject
    private IpAddressMapperHelper ipAddressMapperHelper;

    @Inject
    private GenericDao genericDao;

    public EthernetSwitchPortMapper() {
        super(EthernetSwitchPortResource.class, EthernetSwitchPort.class);
        registerProvider(IpV4Address.class, this::provideIpV4Address);
        registerProvider(IpV6Address.class, this::provideIpV6Address);
    }

    private IpV4Address provideIpV4Address(IpV4AddressObject address) {
        return ipAddressMapperHelper.provideIpV4Address(address, target.getIpV4Addresses());
    }

    private IpV6Address provideIpV6Address(IpV6AddressObject address) {
        return ipAddressMapperHelper.provideIpV6Address(address, target.getIpV6Addresses());
    }

    @Override
    protected void performNotAutomatedMapping(EthernetSwitchPortResource source, EthernetSwitchPort target) {
        super.performNotAutomatedMapping(source, target);
        ipAddressMapperHelper.cleanIpV4Addresses(source.getIpV4Addresses(), target.getIpV4Addresses());
        ipAddressMapperHelper.cleanIpV6Addresses(source.getIpV6Addresses(), target.getIpV6Addresses());
    }
}
