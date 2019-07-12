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

import com.intel.rsd.nodecomposer.business.EntityOperationException;
import com.intel.rsd.nodecomposer.business.redfish.services.RemoteEndpointAccessService;
import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.persistence.dao.DriveDao;
import com.intel.rsd.nodecomposer.persistence.dao.EndpointDao;
import com.intel.rsd.nodecomposer.persistence.redfish.ComposedNode;
import com.intel.rsd.nodecomposer.persistence.redfish.base.DiscoverableEntity;
import com.intel.rsd.nodecomposer.types.Protocol;
import lombok.SneakyThrows;
import lombok.extern.slf4j.Slf4j;
import lombok.val;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;

import java.util.List;

import static com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId.oDataIdFromUri;
import static com.intel.rsd.nodecomposer.externalservices.WebClientExceptionUtils.badRequestInTheRootCause;
import static com.intel.rsd.nodecomposer.types.Protocol.NVME;
import static com.intel.rsd.nodecomposer.types.Protocol.PCIE;
import static com.intel.rsd.nodecomposer.utils.Contracts.requiresNonNull;
import static java.lang.String.format;
import static java.util.Arrays.asList;
import static java.util.Optional.ofNullable;

@Component
@Slf4j
public class InitiatorEndpointFinder {

    public static final List<Protocol> PNC_FABRIC_TYPES = asList(NVME, PCIE);

    private final DriveDao driveDao;

    private final RemoteEndpointAccessService remoteEndpointAccessService;

    private final EndpointDao endpointDao;

    @Autowired
    public InitiatorEndpointFinder(RemoteEndpointAccessService remoteEndpointAccessService, DriveDao driveDao, EndpointDao endpointDao) {
        this.driveDao = driveDao;
        this.endpointDao = endpointDao;
        this.remoteEndpointAccessService = remoteEndpointAccessService;
    }

    public ODataId findOrCreateInitiatorEndpoint(ComposedNode composedNode, ODataId fabricODataId, Protocol fabricType) {
        val system = composedNode.getComputerSystem();
        requiresNonNull(system, format("There is no computer system connected to the composed node(%s)", composedNode.getUri()));

        val initiatorEndpoint = ofNullable(
            PNC_FABRIC_TYPES.contains(fabricType)
                ? driveDao.getConnectedInitiatorEndpoint(system)
                : endpointDao.findInitiatorEndpointBySystemAndFabric(system.getUri(), fabricODataId)
        );

        return initiatorEndpoint.map(DiscoverableEntity::getUri)
            .orElseGet(() -> createOrObtainEndpoint(composedNode, fabricODataId, fabricType));
    }

    @SneakyThrows
    private ODataId createOrObtainEndpoint(ComposedNode composedNode, ODataId fabricODataId, Protocol fabricType) {
        val system = composedNode.getComputerSystem();
        requiresNonNull(system, "computerSystem");
        try {
            return remoteEndpointAccessService.createInitiatorEndpoint(fabricODataId, fabricType, system.getUuid(), system.getPcieConnectionIds());
        } catch (EntityOperationException e) {
            if (!badRequestInTheRootCause(e)) {
                log.warn("Cannot find initiator endpoint, while creating initiator endpoint return bad request");
                throw new EntityOperationException(e.getMessage(), e);
            }
            val uri = remoteEndpointAccessService.getInitiatorEndpointUriBySystemUuid(fabricODataId, composedNode.getAssociatedComputerSystemUuid());
            return oDataIdFromUri(uri);
        }
    }
}
