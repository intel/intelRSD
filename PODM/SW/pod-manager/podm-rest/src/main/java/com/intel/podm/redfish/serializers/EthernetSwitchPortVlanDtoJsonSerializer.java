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

import com.intel.podm.business.dto.redfish.VlanNetworkInterfaceDto;
import com.intel.podm.redfish.json.templates.VlanJson;
import com.intel.podm.business.services.redfish.odataid.ODataContextProvider;
import com.intel.podm.business.services.redfish.odataid.ODataId;
import com.intel.podm.rest.representation.json.serializers.BaseDtoJsonSerializer;

import java.util.Objects;

import static com.intel.podm.business.services.redfish.odataid.ODataIdHelper.oDataIdFromUri;
import static java.lang.Boolean.TRUE;

public class EthernetSwitchPortVlanDtoJsonSerializer extends BaseDtoJsonSerializer<VlanNetworkInterfaceDto> {
    public EthernetSwitchPortVlanDtoJsonSerializer() {
        super(VlanNetworkInterfaceDto.class);
    }

    @Override
    protected VlanJson translate(VlanNetworkInterfaceDto dto) {
        VlanJson vlanJson = new VlanJson();

        ODataId oDataId = oDataIdFromUri(context.getRequestPath());
        vlanJson.oDataId = oDataId;
        vlanJson.oDataContext = ODataContextProvider.getContextFromId(oDataId);

        vlanJson.id = dto.getId();
        vlanJson.name = dto.getName();
        vlanJson.description = dto.getDescription();
        vlanJson.vlanEnable = dto.getVlanEnable();
        vlanJson.vlanId = dto.getVlanId();
        vlanJson.oem.rackScaleOem.oDataType = "#Intel.Oem.VLanNetworkInterface";
        vlanJson.oem.rackScaleOem.tagged = Objects.equals(TRUE, dto.getTagged());
        vlanJson.oem.rackScaleOem.status = dto.getStatus();

        return vlanJson;
    }
}
