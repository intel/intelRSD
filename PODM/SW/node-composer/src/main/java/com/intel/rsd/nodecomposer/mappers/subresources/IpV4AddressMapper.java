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

package com.intel.rsd.nodecomposer.mappers.subresources;

import com.intel.rsd.nodecomposer.externalservices.resources.redfish.IpV4AddressObject;
import com.intel.rsd.nodecomposer.mappers.subresources.strategies.EmbeddableCleanAndCreateStrategy;
import com.intel.rsd.nodecomposer.persistence.redfish.embeddables.IpV4Address;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import java.util.Objects;

import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_PROTOTYPE;

@Component
@Scope(SCOPE_PROTOTYPE)
public class IpV4AddressMapper extends SubresourceMapper<IpV4AddressObject, IpV4Address> {
    @Autowired
    public IpV4AddressMapper(EmbeddableCleanAndCreateStrategy<IpV4Address> entityCleanAndCreateStrategy) {
        super(IpV4Address.class, entityCleanAndCreateStrategy);
    }

    @Override
    public void map(IpV4AddressObject source, IpV4Address target) {
        target.setAddress(source.getAddress());
        target.setGateway(source.getGateway());
        target.setSubnetMask(source.getSubnetMask());
        target.setAddressOrigin(source.getAddressOrigin());
        target.setOem(source.getOem());
    }

    @Override
    protected boolean equals(IpV4AddressObject source, IpV4Address target) {
        return addressDetailsEquals(source, target)
            && Objects.equals(source.getGateway(), target.getGateway())
            && Objects.equals(source.getSubnetMask(), target.getSubnetMask())
            && Objects.equals(source.getOem(), target.getOem());
    }

    private boolean addressDetailsEquals(IpV4AddressObject source, IpV4Address target) {
        return Objects.equals(source.getAddress(), target.getAddress())
            && Objects.equals(source.getAddressOrigin(), target.getAddressOrigin());
    }
}
