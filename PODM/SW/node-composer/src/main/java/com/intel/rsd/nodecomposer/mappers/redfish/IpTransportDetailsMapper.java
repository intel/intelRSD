/*
 * Copyright (c) 2016-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.mappers.redfish;

import com.intel.rsd.nodecomposer.externalservices.resources.redfish.IpTransportDetailsResource;
import com.intel.rsd.nodecomposer.mappers.EntityMapper;
import com.intel.rsd.nodecomposer.mappers.subresources.IpV4AddressMapper;
import com.intel.rsd.nodecomposer.mappers.subresources.IpV6AddressMapper;
import com.intel.rsd.nodecomposer.persistence.redfish.IpTransportDetails;
import com.intel.rsd.nodecomposer.persistence.redfish.embeddables.IpV4Address;
import com.intel.rsd.nodecomposer.persistence.redfish.embeddables.IpV6Address;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_PROTOTYPE;

@Component
@Scope(SCOPE_PROTOTYPE)
public class IpTransportDetailsMapper extends EntityMapper<IpTransportDetailsResource, IpTransportDetails> {
    private final IpV4AddressMapper ipV4AddressMapper;
    private final IpV6AddressMapper ipV6AddressMapper;

    @Autowired
    public IpTransportDetailsMapper(IpV4AddressMapper ipV4AddressMapper, IpV6AddressMapper ipV6AddressMapper) {
        super(IpTransportDetailsResource.class, IpTransportDetails.class);
        ignoredProperties("ipv4Address", "ipv6Address");
        this.ipV4AddressMapper = ipV4AddressMapper;
        this.ipV6AddressMapper = ipV6AddressMapper;
    }

    @Override
    protected void performNotAutomatedMapping(IpTransportDetailsResource source, IpTransportDetails target) {
        super.performNotAutomatedMapping(this.source, this.target);

        mapIpv4Address(source, target);
        mapIpv6Address(source, target);
    }

    private void mapIpv4Address(IpTransportDetailsResource source, IpTransportDetails target) {
        if (source.getIpv4Address() != null) {
            IpV4Address ipV4Address = new IpV4Address();
            ipV4AddressMapper.map(source.getIpv4Address(), ipV4Address);
            target.setIpV4Address(ipV4Address);
        }
    }

    private void mapIpv6Address(IpTransportDetailsResource source, IpTransportDetails targetObject) {
        if (source.getIpv6Address() != null) {
            IpV6Address ipV6Address = new IpV6Address();
            ipV6AddressMapper.map(source.getIpv6Address(), ipV6Address);
            targetObject.setIpV6Address(ipV6Address);
        }
    }
}
