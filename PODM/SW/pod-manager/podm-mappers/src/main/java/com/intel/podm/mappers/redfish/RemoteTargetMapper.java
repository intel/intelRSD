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
import com.intel.podm.business.entities.redfish.RemoteTarget;
import com.intel.podm.business.entities.redfish.properties.RemoteTargetIscsiAddress;
import com.intel.podm.client.api.resources.redfish.IscsiAddressObject;
import com.intel.podm.client.api.resources.redfish.RemoteTargetResource;
import com.intel.podm.client.api.resources.redfish.TargetLunObject;
import com.intel.podm.mappers.DomainObjectMapper;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.util.List;

import static java.util.stream.Collectors.toList;

@Dependent
public class RemoteTargetMapper extends DomainObjectMapper<RemoteTargetResource, RemoteTarget> {

    @Inject
    private GenericDao genericDao;

    public RemoteTargetMapper() {
        super(RemoteTargetResource.class, RemoteTarget.class);
    }

    @Override
    public void performNotAutomatedMapping(RemoteTargetResource source, RemoteTarget target) {
        target.getRemoteTargetIscsiAddresses().forEach(genericDao::remove);
        createIscsiAddresses(source, target);
    }

    private void createIscsiAddresses(RemoteTargetResource source, RemoteTarget target) {
        for (IscsiAddressObject iscsiAddressObject : source.getAddresses()) {
            RemoteTargetIscsiAddress remoteTargetIscsiAddress = genericDao.create(RemoteTargetIscsiAddress.class);
            remoteTargetIscsiAddress.setTargetIqn(iscsiAddressObject.getTargetIqn());
            remoteTargetIscsiAddress.setTargetLun(getLuns(iscsiAddressObject.getTargetLuns()));
            remoteTargetIscsiAddress.setTargetPortalIp(iscsiAddressObject.getTargetPortalIp());
            remoteTargetIscsiAddress.setTargetPortalPort(iscsiAddressObject.getTargetPortalPort());

            target.linkRemoteTargetIscsiAddresses(remoteTargetIscsiAddress);
        }
    }

    private List<Integer> getLuns(List<TargetLunObject> targetLuns) {
        return targetLuns.stream().map(TargetLunObject::getLun).collect(toList());
    }
}
