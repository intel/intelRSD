/*
 * Copyright (c) 2015 Intel Corporation
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

import com.intel.podm.business.dto.redfish.AdapterDto;
import com.intel.podm.redfish.json.templates.AdapterJson;
import com.intel.podm.rest.odataid.ODataId;
import com.intel.podm.rest.representation.json.serializers.DtoJsonSerializer;

import static com.intel.podm.rest.odataid.ODataContextProvider.getContextFromId;
import static com.intel.podm.rest.odataid.ODataId.oDataId;
import static com.intel.podm.rest.odataid.ODataIds.oDataIdFromContext;
import static java.net.URI.create;

public class AdapterDtoJsonSerializer extends DtoJsonSerializer<AdapterDto> {

    public AdapterDtoJsonSerializer() {
        super(AdapterDto.class);
    }

    @Override
    protected AdapterJson translate(AdapterDto dto) {
        AdapterJson storageControllerJson = new AdapterJson();

        ODataId oDataId = oDataId(context.getRequestPath());
        storageControllerJson.oDataId = oDataId;
        storageControllerJson.oDataContext = getContextFromId(oDataId);

        storageControllerJson.id = dto.getId();
        storageControllerJson.name = dto.getName();
        storageControllerJson.description = dto.getDescription();
        storageControllerJson.model = dto.getModel();
        storageControllerJson.status = dto.getStatus();
        storageControllerJson.devices = oDataId(create(context.getRequestPath() + "/Devices"));
        storageControllerJson.busInfo = dto.getBusInfo();
        storageControllerJson.manufacturer = dto.getManufacturer();
        storageControllerJson.links.containedBy = oDataIdFromContext(dto.getContainedBy());
        storageControllerJson.storageControllerInterface = dto.getStorageControllerInterface();

        return storageControllerJson;
    }
}
