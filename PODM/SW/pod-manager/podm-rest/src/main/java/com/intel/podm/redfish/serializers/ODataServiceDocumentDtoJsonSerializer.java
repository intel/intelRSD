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

package com.intel.podm.redfish.serializers;

import com.intel.podm.business.dto.redfish.ODataServiceDocumentDto;
import com.intel.podm.business.dto.redfish.attributes.ODataServiceDto;
import com.intel.podm.redfish.json.templates.ODataServiceDocumentJson;
import com.intel.podm.rest.representation.json.serializers.DtoJsonSerializer;

import java.net.URI;
import java.util.List;

public class ODataServiceDocumentDtoJsonSerializer extends DtoJsonSerializer<ODataServiceDocumentDto> {
    private static final String METADATA = "/redfish/v1/$metadata";

    public ODataServiceDocumentDtoJsonSerializer() {
        super(ODataServiceDocumentDto.class);
    }

    @Override
    protected ODataServiceDocumentJson translate(ODataServiceDocumentDto dto) {
        ODataServiceDocumentJson oDataJson = new ODataServiceDocumentJson();
        oDataJson.oDataContext = URI.create(METADATA);
        addODataServices(oDataJson, dto.getValues());
        return oDataJson;
    }

    private void addODataServices(ODataServiceDocumentJson json, List<ODataServiceDto> serviceList) {
        serviceList.forEach(service ->
            json.value.add(new ODataServiceDocumentJson.ODataServiceJson(service.getName(), service.getKind(), service.getUrl()))
        );
    }
}
