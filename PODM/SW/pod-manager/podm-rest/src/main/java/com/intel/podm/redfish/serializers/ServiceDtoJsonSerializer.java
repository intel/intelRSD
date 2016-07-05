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

import com.intel.podm.business.dto.redfish.ServiceDto;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.redfish.json.templates.StorageServiceJson;
import com.intel.podm.rest.odataid.ODataContextProvider;
import com.intel.podm.rest.odataid.ODataId;
import com.intel.podm.rest.odataid.ODataIds;
import com.intel.podm.rest.representation.json.serializers.DtoJsonSerializer;

import java.util.Collection;
import java.util.List;

import static com.intel.podm.rest.odataid.ODataId.oDataId;
import static java.net.URI.create;
import static java.util.Collections.emptyList;
import static java.util.Objects.isNull;

public class ServiceDtoJsonSerializer extends DtoJsonSerializer<ServiceDto> {
    protected ServiceDtoJsonSerializer() {
        super(ServiceDto.class);
    }

    @Override
    protected StorageServiceJson translate(ServiceDto dto) {
        StorageServiceJson serviceJson = new StorageServiceJson();
        ODataId oDataId = oDataId(context.getRequestPath());

        serviceJson.oDataId = oDataId;
        serviceJson.oDataContext = ODataContextProvider.getContextFromId(oDataId);

        serviceJson.id = dto.getId();
        serviceJson.name = dto.getName();
        serviceJson.status = dto.getStatus();
        serviceJson.remoteTargets = oDataId(create(oDataId + "/Targets"));
        serviceJson.logicalDrives = oDataId(create(oDataId + "/LogicalDrives"));
        serviceJson.drives = oDataId(create(oDataId + "/Drives"));

        serviceJson.links.managedBy.addAll(mapToODataIdsCollection(dto.getManagedBy()));

        return serviceJson;
    }

    private Collection<ODataId> mapToODataIdsCollection(List<Context> contexts) {
        return isNull(contexts)
                ?
                emptyList()
                :
                ODataIds.oDataIdsCollection(contexts.stream().toArray(Context[]::new));
    }
}
