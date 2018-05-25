/*
 * Copyright (c) 2016-2018 Intel Corporation
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

package com.intel.podm.mappers.subresources;

import com.intel.podm.business.entities.redfish.embeddables.IpV6AddressPolicy;
import com.intel.podm.client.resources.redfish.IpV6AddressPolicyObject;
import com.intel.podm.mappers.subresources.strategies.EmbeddableCleanAndCreateStrategy;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.util.Objects;

@Dependent
public class IpV6AddressPolicyTableMapper extends SubresourceMapper<IpV6AddressPolicyObject, IpV6AddressPolicy> {

    @Inject
    public IpV6AddressPolicyTableMapper(EmbeddableCleanAndCreateStrategy<IpV6AddressPolicy> entityCleanAndCreateStrategy) {
        super(IpV6AddressPolicy.class, entityCleanAndCreateStrategy);
    }

    @Override
    public void map(IpV6AddressPolicyObject source, IpV6AddressPolicy target) {
        target.setLabel(source.getLabel());
        target.setPrecedence(source.getPrecedence());
        target.setPrefix(source.getPrefix());
    }

    @Override
    protected boolean equals(IpV6AddressPolicyObject source, IpV6AddressPolicy target) {
        return Objects.equals(source.getLabel(), target.getLabel())
                && Objects.equals(source.getPrecedence(), target.getPrecedence())
                && Objects.equals(source.getPrefix(), target.getPrefix());
    }
}
