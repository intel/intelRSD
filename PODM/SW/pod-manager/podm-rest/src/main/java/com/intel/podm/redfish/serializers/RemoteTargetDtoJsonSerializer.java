/*
 * Copyright (c) 2015-2017 Intel Corporation
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

package com.intel.podm.redfish.serializers;

import com.intel.podm.business.dto.redfish.RemoteTargetDto;
import com.intel.podm.business.dto.redfish.RemoteTargetIscsiAddressDto;
import com.intel.podm.business.dto.redfish.attributes.ChapDto;
import com.intel.podm.redfish.json.templates.RemoteTargetJson;
import com.intel.podm.redfish.json.templates.attributes.IscsiAddressJson;
import com.intel.podm.business.services.redfish.odataid.ODataId;
import com.intel.podm.business.services.redfish.odataid.ODataIdFromContextHelper;
import com.intel.podm.rest.representation.json.serializers.BaseDtoJsonSerializer;

import java.util.List;

import static com.intel.podm.business.services.redfish.odataid.ODataContextProvider.getContextFromId;
import static com.intel.podm.business.services.redfish.odataid.ODataIdHelper.oDataIdFromUri;
import static java.util.Collections.emptyList;
import static java.util.stream.Collectors.toList;

public class RemoteTargetDtoJsonSerializer extends BaseDtoJsonSerializer<RemoteTargetDto> {
    protected RemoteTargetDtoJsonSerializer() {
        super(RemoteTargetDto.class);
    }

    @Override
    protected RemoteTargetJson translate(RemoteTargetDto dto) {
        RemoteTargetJson remoteTargetJson = new RemoteTargetJson();

        ODataId oDataId = oDataIdFromUri(context.getRequestPath());
        remoteTargetJson.oDataId = oDataId;
        remoteTargetJson.oDataContext = getContextFromId(oDataId);

        remoteTargetJson.id = dto.getId();
        remoteTargetJson.description = dto.getDescription();
        remoteTargetJson.name = dto.getName();
        remoteTargetJson.status = dto.getStatus();
        remoteTargetJson.type = dto.getType();

        remoteTargetJson.initiator.add(mapToIscsiInitiatorIqn(dto.getIscsiInitiatorIqn()));
        remoteTargetJson.addresses.addAll(mapToIscsiAddresses(dto.getIscsiAddresses()));

        return remoteTargetJson;
    }

    private RemoteTargetJson.InitiatorWrapper mapToIscsiInitiatorIqn(String iscsiInitiatorIqn) {
        RemoteTargetJson.InitiatorWrapper initiatorWrapper = new RemoteTargetJson.InitiatorWrapper();
        initiatorWrapper.iscsiInitiatorJson.initiatorIqn = iscsiInitiatorIqn;
        return initiatorWrapper;
    }

    private List<RemoteTargetJson.AddressWrapper> mapToIscsiAddresses(List<RemoteTargetIscsiAddressDto> addresses) {
        if (addresses == null) {
            return emptyList();
        }

        return addresses.stream().map(
            addressDto -> {
                RemoteTargetJson.AddressWrapper wrapper = new RemoteTargetJson.AddressWrapper();
                wrapper.iscsiAddressJson.targetIqn = addressDto.getTargetIqn();
                wrapper.iscsiAddressJson.targetPortalIp = addressDto.getTargetPortalIp();
                wrapper.iscsiAddressJson.targetPortalPort = addressDto.getTargetPortalPort();
                wrapper.iscsiAddressJson.targetLun = mapToTargetLuns(addressDto.getTargetLun());
                ChapDto sourceChap = addressDto.getChap();
                if (sourceChap != null) {
                    mapToChap(sourceChap, wrapper.iscsiAddressJson);
                }
                return wrapper;
            }).collect(toList());
    }

    private void mapToChap(ChapDto chapDto, IscsiAddressJson iscsiAddressJson) {
        iscsiAddressJson.chap = new IscsiAddressJson.Chap();
        iscsiAddressJson.chap.type = chapDto.getType();
        iscsiAddressJson.chap.username = chapDto.getUsername();
        iscsiAddressJson.chap.mutualUsername = chapDto.getMutualUsername();
    }

    private List<IscsiAddressJson.TargetLun> mapToTargetLuns(List<RemoteTargetIscsiAddressDto.TargetLunDto> targetLunCollection) {
        if (targetLunCollection == null) {
            return emptyList();
        }

        return targetLunCollection.stream()
            .map(targetLunDto ->
                new IscsiAddressJson.TargetLun(targetLunDto.getLun(), ODataIdFromContextHelper.asOdataId(targetLunDto.getLogicalDrive()))
            ).collect(toList());
    }
}
