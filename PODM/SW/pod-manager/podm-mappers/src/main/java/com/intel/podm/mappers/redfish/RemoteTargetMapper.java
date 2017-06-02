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

package com.intel.podm.mappers.redfish;

import com.intel.podm.business.entities.redfish.RemoteTarget;
import com.intel.podm.client.api.resources.redfish.RemoteTargetResource;
import com.intel.podm.mappers.EntityMapper;
import com.intel.podm.mappers.subresources.RemoteTargetIscsiAddressMapper;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;

@Dependent
public class RemoteTargetMapper extends EntityMapper<RemoteTargetResource, RemoteTarget> {
    @Inject
    private RemoteTargetIscsiAddressMapper remoteTargetIscsiAddressMapper;

    public RemoteTargetMapper() {
        super(RemoteTargetResource.class, RemoteTarget.class);
    }

    @Override
    public void performNotAutomatedMapping(RemoteTargetResource sourceRemoteTarget, RemoteTarget targetRemoteTarget) {
        super.performNotAutomatedMapping(source, target);
        remoteTargetIscsiAddressMapper.map(sourceRemoteTarget.getAddresses(),
            targetRemoteTarget.getRemoteTargetIscsiAddresses(), targetRemoteTarget::addRemoteTargetIscsiAddress);
    }
}
