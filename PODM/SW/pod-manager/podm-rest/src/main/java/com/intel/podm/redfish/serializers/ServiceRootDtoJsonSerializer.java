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

import com.intel.podm.business.dto.redfish.ServiceRootDto;
import com.intel.podm.redfish.json.templates.ServiceRootJson;
import com.intel.podm.business.services.redfish.odataid.ODataId;
import com.intel.podm.rest.representation.json.serializers.BaseDtoJsonSerializer;

import static com.intel.podm.business.services.redfish.odataid.ODataContextProvider.getContextFromId;
import static com.intel.podm.business.services.redfish.odataid.ODataIdHelper.oDataIdFromUri;
import static java.net.URI.create;

@SuppressWarnings({"checkstyle:ExecutableStatementCount"})
public final class ServiceRootDtoJsonSerializer extends BaseDtoJsonSerializer<ServiceRootDto> {
    public ServiceRootDtoJsonSerializer() {
        super(ServiceRootDto.class);
    }

    @Override
    protected ServiceRootJson translate(ServiceRootDto dto) {
        ServiceRootJson result = new ServiceRootJson();

        ODataId oDataId = oDataIdFromUri(context.getRequestPath());

        result.oDataId = oDataId;
        result.oDataContext = getContextFromId(oDataId);

        result.id = dto.getId();
        result.name = dto.getName();
        result.description = dto.getDescription();
        result.uuid = dto.getUuid();
        result.nodes = oDataIdFromUri(create(oDataId + "/Nodes"));
        result.systems = oDataIdFromUri(create(oDataId + "/Systems"));
        result.chassis = oDataIdFromUri(create(oDataId + "/Chassis"));
        result.managers = oDataIdFromUri(create(oDataId + "/Managers"));
        result.services = oDataIdFromUri(create(oDataId + "/Services"));
        result.eventService = oDataIdFromUri(create(oDataId + "/EventService"));
        result.redfishVersion = dto.getRedfishVersion();
        result.ethernetSwitches = oDataIdFromUri(create(oDataId + "/EthernetSwitches"));
        result.fabrics = oDataIdFromUri(create(oDataId + "/Fabrics"));

        return result;
    }
}
