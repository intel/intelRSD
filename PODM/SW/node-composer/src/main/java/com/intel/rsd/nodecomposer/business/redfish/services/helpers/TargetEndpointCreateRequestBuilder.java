/*
 * Copyright (c) 2017-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.business.redfish.services.helpers;

import com.intel.rsd.nodecomposer.business.dto.EndpointDto;
import com.intel.rsd.nodecomposer.business.dto.EndpointDto.ConnectedEntityDto;
import com.intel.rsd.nodecomposer.business.dto.EndpointDto.IdentifierDto;
import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.types.DurableNameFormat;
import com.intel.rsd.nodecomposer.types.Protocol;
import lombok.val;

import static com.intel.rsd.nodecomposer.business.redfish.services.helpers.DurableNameProducer.createDurableName;
import static com.intel.rsd.nodecomposer.types.DurableNameFormat.findByProtocol;
import static com.intel.rsd.nodecomposer.types.EntityRole.TARGET;
import static java.util.UUID.randomUUID;

public final class TargetEndpointCreateRequestBuilder {
    private TargetEndpointCreateRequestBuilder() {
    }

    public static EndpointDto buildTargetEndpointCreationRequest(Protocol fabricType, ODataId oDataId) {
        val endpointDto = new EndpointDto();
        endpointDto.addIdentifier(buildIdentifierDto(getDurableNameFormat(fabricType)));
        endpointDto.addConnectedEntity(buildConnectedEntityDto(oDataId));

        return endpointDto;
    }

    private static IdentifierDto buildIdentifierDto(DurableNameFormat durableNameFormat) {
        val identifierDto = new IdentifierDto();
        identifierDto.setDurableNameFormat(durableNameFormat);
        identifierDto.setDurableName(createDurableName(durableNameFormat, randomUUID()));

        return identifierDto;
    }

    private static ConnectedEntityDto buildConnectedEntityDto(ODataId oDataId) {
        val connectedEntityDto = new ConnectedEntityDto();
        connectedEntityDto.setEntityLink(oDataId);
        connectedEntityDto.setEntityRole(TARGET);

        return connectedEntityDto;
    }

    private static DurableNameFormat getDurableNameFormat(Protocol fabricType) {
        return findByProtocol(fabricType)
            .orElseThrow(() -> new RuntimeException("Cannot determine durable name format for fabric type: " + fabricType));
    }
}
