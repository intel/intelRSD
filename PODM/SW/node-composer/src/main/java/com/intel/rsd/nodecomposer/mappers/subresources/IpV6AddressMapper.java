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

import com.intel.rsd.nodecomposer.externalservices.resources.redfish.IpV6AddressObject;
import com.intel.rsd.nodecomposer.mappers.subresources.strategies.EmbeddableCleanAndCreateStrategy;
import com.intel.rsd.nodecomposer.persistence.redfish.embeddables.IpV6Address;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import java.util.Objects;

import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_PROTOTYPE;

@Component
@Scope(SCOPE_PROTOTYPE)
public class IpV6AddressMapper extends SubresourceMapper<IpV6AddressObject, IpV6Address> {
    @Autowired
    public IpV6AddressMapper(EmbeddableCleanAndCreateStrategy<IpV6Address> entityCleanAndCreateStrategy) {
        super(IpV6Address.class, entityCleanAndCreateStrategy);
    }

    @Override
    public void map(IpV6AddressObject source, IpV6Address target) {
        target.setAddress(source.getAddress());
        target.setAddressOrigin(source.getAddressOrigin());
        target.setAddressState(source.getAddressState());
        target.setPrefixLength(source.getPrefixLength());
        target.setOem(source.getOem());
    }

    @Override
    protected boolean equals(IpV6AddressObject source, IpV6Address target) {
        return addressDetailsEquals(source, target)
            && Objects.equals(source.getPrefixLength(), target.getPrefixLength())
            && Objects.equals(source.getOem(), target.getOem());
    }

    private boolean addressDetailsEquals(IpV6AddressObject source, IpV6Address target) {
        return Objects.equals(source.getAddress(), target.getAddress())
            && Objects.equals(source.getAddressOrigin(), target.getAddressOrigin())
            && Objects.equals(source.getAddressState(), target.getAddressState());
    }
}
