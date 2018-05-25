/*
 * Copyright (c) 2017-2018 Intel Corporation
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

package com.intel.podm.business.redfish.services.helpers;

import com.intel.podm.business.dto.EndpointDto;
import com.intel.podm.business.dto.EndpointDto.ConnectedEntityDto;
import com.intel.podm.business.dto.EndpointDto.IdentifierDto;
import com.intel.podm.business.entities.redfish.Fabric;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.common.types.DurableNameFormat;
import com.intel.podm.common.types.Protocol;

import java.util.UUID;

import static com.intel.podm.business.redfish.services.helpers.DurableNameProducer.createDurableName;
import static com.intel.podm.common.types.DurableNameFormat.findByProtocol;
import static com.intel.podm.common.types.EntityRole.INITIATOR;
import static com.intel.podm.common.types.EntityRole.TARGET;
import static java.util.UUID.randomUUID;

public final class EndpointCreationRequestBuilder {
    private EndpointCreationRequestBuilder() {
    }

    public static EndpointDto buildTargetEndpointCreationRequest(Fabric fabric, Context context) {
        Protocol fabricType = fabric.getFabricType();
        EndpointDto endpointDto = new EndpointDto();
        endpointDto.addIdentifier(buildIdentifierDto(getDurableNameFormat(fabricType)));
        endpointDto.addConnectedEntity(buildConnectedEntityDto(context));

        return endpointDto;
    }

    private static IdentifierDto buildIdentifierDto(DurableNameFormat durableNameFormat) {
        IdentifierDto identifierDto = new IdentifierDto();
        identifierDto.setDurableNameFormat(durableNameFormat);
        identifierDto.setDurableName(createDurableName(durableNameFormat, randomUUID()));

        return identifierDto;
    }

    private static ConnectedEntityDto buildConnectedEntityDto(Context context) {
        ConnectedEntityDto connectedEntityDto = new ConnectedEntityDto();
        connectedEntityDto.setEntityLink(context);
        connectedEntityDto.setEntityRole(TARGET);

        return connectedEntityDto;
    }

    public static EndpointDto buildInitiatorEndpointCreationRequest(Fabric fabric, UUID computerSystemUuid) {
        Protocol fabricType = fabric.getFabricType();
        EndpointDto endpointDto = new EndpointDto();
        endpointDto.addIdentifier(buildIdentifierDto(getDurableNameFormat(fabricType), computerSystemUuid));
        endpointDto.addConnectedEntity(buildConnectedEntityDto());

        return endpointDto;
    }

    private static DurableNameFormat getDurableNameFormat(Protocol fabricType) {
        return findByProtocol(fabricType)
            .orElseThrow(() -> new RuntimeException("Cannot determine durable name format for fabric type: " + fabricType));
    }

    private static IdentifierDto buildIdentifierDto(DurableNameFormat durableNameFormat, UUID computerSystemUuid) {
        IdentifierDto identifierDto = new IdentifierDto();
        identifierDto.setDurableNameFormat(durableNameFormat);
        identifierDto.setDurableName(createDurableName(durableNameFormat, computerSystemUuid));

        return identifierDto;
    }

    private static ConnectedEntityDto buildConnectedEntityDto() {
        ConnectedEntityDto connectedEntityDto = new ConnectedEntityDto();
        connectedEntityDto.setEntityRole(INITIATOR);

        return connectedEntityDto;
    }
}
