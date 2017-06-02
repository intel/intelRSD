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

package com.intel.podm.redfish.serializers;

import com.intel.podm.business.dto.redfish.NetworkInterfaceDto;
import com.intel.podm.redfish.json.templates.NetworkInterfaceJson;
import com.intel.podm.business.services.redfish.odataid.ODataContextProvider;
import com.intel.podm.business.services.redfish.odataid.ODataId;
import com.intel.podm.rest.representation.json.serializers.BaseDtoJsonSerializer;

import static com.intel.podm.business.services.redfish.odataid.ODataIdHelper.oDataIdFromUri;
import static java.net.URI.create;

public class NetworkInterfaceDtoJsonSerializer extends BaseDtoJsonSerializer<NetworkInterfaceDto> {

    protected NetworkInterfaceDtoJsonSerializer() {
        super(NetworkInterfaceDto.class);
    }

    @Override
    protected NetworkInterfaceJson translate(NetworkInterfaceDto dto) {
        NetworkInterfaceJson networkInterfaceJson = new NetworkInterfaceJson();
        ODataId oDataId = oDataIdFromUri(context.getRequestPath());
        networkInterfaceJson.oDataId = oDataId;
        networkInterfaceJson.oDataContext = ODataContextProvider.getContextFromId(oDataId);
        networkInterfaceJson.id = dto.getId();
        networkInterfaceJson.name = dto.getName();
        networkInterfaceJson.description = dto.getDescription();
        networkInterfaceJson.status = dto.getStatus();
        networkInterfaceJson.networkDeviceFunctions = oDataIdFromUri(create(networkInterfaceJson.oDataId + "/NetworkDeviceFunctions"));
        return networkInterfaceJson;
    }
}
