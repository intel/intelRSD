/*
 * Copyright (c) 2017 Intel Corporation
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

package com.intel.podm.business.redfish.services.mappers;

import com.intel.podm.business.dto.RemoteTargetDto;
import com.intel.podm.business.entities.redfish.RemoteTarget;
import com.intel.podm.business.entities.redfish.RemoteTargetIscsiAddress;
import com.intel.podm.business.entities.redfish.embeddables.Chap;

import javax.enterprise.context.Dependent;

@Dependent
class RemoteTargetDtoMapper extends DtoMapper<RemoteTarget, RemoteTargetDto> {
    RemoteTargetDtoMapper() {
        super(RemoteTarget.class, RemoteTargetDto.class);
        this.ignoredProperties("addresses", "initiator", "links", "oem");
    }

    @Override
    protected void performNotAutomatedMapping(RemoteTarget source, RemoteTargetDto target) {
        super.performNotAutomatedMapping(source, target);

        target.getInitiators().add(wrapInitiator(source.getIscsiInitiatorIqn()));
        for (RemoteTargetIscsiAddress remoteTargetIscsiAddress : source.getRemoteTargetIscsiAddresses()) {
            target.getAddresses().add(wrapIscsiAddress(remoteTargetIscsiAddress));
        }
    }

    private RemoteTargetDto.InitiatorWrapper wrapInitiator(String iscsiInitiatorIqn) {
        RemoteTargetDto.InitiatorWrapper initiatorWrapper = new RemoteTargetDto.InitiatorWrapper();
        initiatorWrapper.getIscsiInitiatorDto().setInitiatorIqn(iscsiInitiatorIqn);
        return initiatorWrapper;
    }

    private RemoteTargetDto.AddressWrapper wrapIscsiAddress(RemoteTargetIscsiAddress remoteTargetIscsiAddress) {
        RemoteTargetDto.AddressWrapper wrapper = new RemoteTargetDto.AddressWrapper();
        RemoteTargetDto.AddressWrapper.IscsiAddressDto iscsiAddressDto = wrapper.getIscsiAddressDto();
        for (Integer lun : remoteTargetIscsiAddress.getTargetLuns()) {
            RemoteTargetDto.AddressWrapper.IscsiAddressDto.TargetLunDto dto = new RemoteTargetDto.AddressWrapper.IscsiAddressDto.TargetLunDto();
            dto.setLun(lun);
            iscsiAddressDto.getTargetLuns().add(dto);
        }
        iscsiAddressDto.setTargetPortalIp(remoteTargetIscsiAddress.getTargetPortalIp());
        iscsiAddressDto.setTargetPortalPort(remoteTargetIscsiAddress.getTargetPortalPort());
        iscsiAddressDto.setTargetIqn(remoteTargetIscsiAddress.getTargetIqn());
        mapToChap(remoteTargetIscsiAddress.getChap(), iscsiAddressDto.getChap());
        return wrapper;
    }

    private void mapToChap(Chap chapSource, RemoteTargetDto.AddressWrapper.IscsiAddressDto.Chap chapTarget) {
        if (chapSource != null) {
            chapTarget.setType(chapSource.getType());
            chapTarget.setUsername(chapSource.getUsername());
            chapTarget.setMutualUsername(chapSource.getMutualUsername());
        }
    }
}
