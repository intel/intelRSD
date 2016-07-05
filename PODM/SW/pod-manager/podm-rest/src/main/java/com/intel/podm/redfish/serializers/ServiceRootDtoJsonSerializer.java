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

import com.intel.podm.business.dto.redfish.ServiceRootDto;
import com.intel.podm.redfish.json.templates.ServiceRootJson;
import com.intel.podm.rest.odataid.ODataId;
import com.intel.podm.rest.representation.json.serializers.DtoJsonSerializer;

import static com.intel.podm.rest.odataid.ODataContextProvider.getContextFromId;
import static java.net.URI.create;

public final class ServiceRootDtoJsonSerializer extends DtoJsonSerializer<ServiceRootDto> {
    public ServiceRootDtoJsonSerializer() {
        super(ServiceRootDto.class);
    }

    @Override
    protected ServiceRootJson translate(ServiceRootDto dto) {
        ServiceRootJson result = new ServiceRootJson();

        ODataId oDataId = ODataId.oDataId(context.getRequestPath());

        result.oDataId = oDataId;
        result.oDataContext = getContextFromId(oDataId);

        result.id = dto.getId();
        result.uuid = dto.getUuid();
        result.name = dto.getName();
        result.nodes = ODataId.oDataId(create(oDataId + "/Nodes"));
        result.systems = ODataId.oDataId(create(oDataId + "/Systems"));
        result.chassis = ODataId.oDataId(create(oDataId + "/Chassis"));
        result.managers = ODataId.oDataId(create(oDataId + "/Managers"));
        result.services = ODataId.oDataId(create(oDataId + "/Services"));
        result.eventService = ODataId.oDataId(create(oDataId + "/EventService"));
        result.redfishVersion = dto.getRedfishVersion();
        result.ethernetSwitches = ODataId.oDataId(create(oDataId + "/EthernetSwitches"));

        return result;
    }
}
