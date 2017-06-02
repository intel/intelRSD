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

package com.intel.podm.mappers.subresources;

import com.intel.podm.business.entities.redfish.RemoteTargetIscsiAddress;
import com.intel.podm.business.entities.redfish.embeddables.Chap;
import com.intel.podm.client.api.resources.redfish.ChapObject;
import com.intel.podm.client.api.resources.redfish.IscsiAddressObject;
import com.intel.podm.client.api.resources.redfish.TargetLunObject;
import com.intel.podm.mappers.subresources.strategies.EntityCleanAndCreateStrategy;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.util.List;
import java.util.Objects;

import static java.util.stream.Collectors.toList;

@Dependent
public class RemoteTargetIscsiAddressMapper extends SubresourceMapper<IscsiAddressObject, RemoteTargetIscsiAddress> {

    @Inject
    private ChapMapper chapMapper;

    @Inject
    public RemoteTargetIscsiAddressMapper(EntityCleanAndCreateStrategy<RemoteTargetIscsiAddress> entityCleanAndCreateStrategy) {
        super(RemoteTargetIscsiAddress.class, entityCleanAndCreateStrategy);
    }

    @Override
    public void map(IscsiAddressObject sourceIscsiAddress, RemoteTargetIscsiAddress targetIscsiAddress) {
        targetIscsiAddress.setTargetIqn(sourceIscsiAddress.getTargetIqn());
        getLuns(sourceIscsiAddress.getTargetLuns()).forEach(targetIscsiAddress::addTargetLun);
        targetIscsiAddress.setTargetPortalIp(sourceIscsiAddress.getTargetPortalIp());
        targetIscsiAddress.setTargetPortalPort(sourceIscsiAddress.getTargetPortalPort());
        mapChap(sourceIscsiAddress, targetIscsiAddress);
    }

    private void mapChap(IscsiAddressObject sourceIscsiAddress, RemoteTargetIscsiAddress targetIscsiAddress) {
        Chap targetChap = provideChap(targetIscsiAddress.getChap());
        targetIscsiAddress.setChap(targetChap);

        ChapObject sourceChap = sourceIscsiAddress.getChap();
        if (sourceChap != null) {
            chapMapper.map(sourceChap, targetChap);
        }
    }

    @Override
    public boolean equals(IscsiAddressObject sourceIscsiAddress, RemoteTargetIscsiAddress targetIscsiAddress) {
        return areTargetsEqual(sourceIscsiAddress, targetIscsiAddress)
            && Objects.equals(sourceIscsiAddress.getChap(), targetIscsiAddress.getChap());
    }

    private boolean areTargetsEqual(IscsiAddressObject sourceIscsiAddress, RemoteTargetIscsiAddress targetIscsiAddress) {
        return Objects.equals(sourceIscsiAddress.getTargetIqn(), targetIscsiAddress.getTargetIqn())
            && Objects.equals(getLuns(sourceIscsiAddress.getTargetLuns()), targetIscsiAddress.getTargetLuns())
            && Objects.equals(sourceIscsiAddress.getTargetPortalIp(), targetIscsiAddress.getTargetPortalIp())
            && Objects.equals(sourceIscsiAddress.getTargetPortalPort(), targetIscsiAddress.getTargetPortalPort());
    }

    private List<Integer> getLuns(List<TargetLunObject> targetLuns) {
        return targetLuns.stream().map(TargetLunObject::getLun).collect(toList());
    }

    private Chap provideChap(Chap target) {
        return target == null ? new Chap() : target;
    }
}
