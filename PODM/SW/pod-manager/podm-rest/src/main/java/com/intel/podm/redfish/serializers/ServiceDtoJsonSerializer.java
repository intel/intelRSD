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

import com.intel.podm.business.dto.redfish.ServiceDto;
import com.intel.podm.redfish.json.templates.StorageServiceJson;
import com.intel.podm.business.services.redfish.odataid.ODataContextProvider;
import com.intel.podm.business.services.redfish.odataid.ODataId;
import com.intel.podm.rest.representation.json.serializers.BaseDtoJsonSerializer;

import static com.intel.podm.business.services.redfish.odataid.ODataIdFromContextHelper.asOdataIdSet;
import static com.intel.podm.business.services.redfish.odataid.ODataIdHelper.oDataIdFromUri;
import static java.net.URI.create;

public class ServiceDtoJsonSerializer extends BaseDtoJsonSerializer<ServiceDto> {
    protected ServiceDtoJsonSerializer() {
        super(ServiceDto.class);
    }

    @Override
    protected StorageServiceJson translate(ServiceDto dto) {
        StorageServiceJson serviceJson = new StorageServiceJson();
        ODataId oDataId = oDataIdFromUri(context.getRequestPath());

        serviceJson.oDataId = oDataId;
        serviceJson.oDataContext = ODataContextProvider.getContextFromId(oDataId);

        serviceJson.id = dto.getId();
        serviceJson.name = dto.getName();
        serviceJson.description = dto.getDescription();
        serviceJson.status = dto.getStatus();
        serviceJson.remoteTargets = oDataIdFromUri(create(oDataId + "/Targets"));
        serviceJson.logicalDrives = oDataIdFromUri(create(oDataId + "/LogicalDrives"));
        serviceJson.drives = oDataIdFromUri(create(oDataId + "/Drives"));

        serviceJson.links.managedBy.addAll(asOdataIdSet(dto.getLinks().getManagedBy()));

        return serviceJson;
    }
}
