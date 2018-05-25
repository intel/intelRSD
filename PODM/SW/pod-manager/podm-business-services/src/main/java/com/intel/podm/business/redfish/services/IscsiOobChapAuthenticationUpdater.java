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

package com.intel.podm.business.redfish.services;

import com.intel.podm.business.BusinessApiException;
import com.intel.podm.business.entities.dao.EndpointDao;
import com.intel.podm.business.entities.redfish.Endpoint;
import com.intel.podm.business.entities.redfish.NetworkDeviceFunction;
import com.intel.podm.business.redfish.services.actions.EndpointUpdateInvoker;
import com.intel.podm.business.redfish.services.assembly.ChapConfigurator;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.types.actions.EndpointUpdateDefinition;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.List;
import java.util.Objects;
import java.util.UUID;
import java.util.function.Predicate;

import static com.intel.podm.common.types.Protocol.ISCSI;
import static java.lang.String.format;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

@Dependent
public class IscsiOobChapAuthenticationUpdater {
    @Inject
    private Logger logger;

    @Inject
    private ChapConfigurator chapConfigurator;

    @Inject
    private EndpointDao endpointDao;

    @Inject
    private EndpointUpdateInvoker endpointUpdateInvoker;

    @Inject
    private NetworkDeviceFunctionAuthenticationUpdater networkDeviceFunctionAuthenticationUpdater;

    @Transactional(REQUIRES_NEW)
    public void checkIscsiOobAuthentication(UUID serviceUuid) {
        endpointDao.findEndpointsWithNullUsername(serviceUuid, ISCSI).forEach(this::fixAuthentication);

        List<Endpoint> endpointsWithAssociatedComputerSystem = endpointDao.findEndpointsWithAssociatedComputerSystem(serviceUuid, ISCSI);
        endpointsWithAssociatedComputerSystem.stream()
            .filter(incorrectCorrespondingUsernames())
            .forEach(this::fixAuthentication);
    }

    private Predicate<? super Endpoint> incorrectCorrespondingUsernames() {
        return endpoint -> {
            NetworkDeviceFunction networkDeviceFunction = endpoint.getComputerSystem().getNetworkDeviceFunctionOrNull();

            if (networkDeviceFunction == null) {
                return false;
            }

            String endpointUsername = endpoint.getAuthentication() != null ? endpoint.getAuthentication().getUsername() : null;
            return !Objects.equals(networkDeviceFunction.getIscsiBoot().getChapUsername(), endpointUsername);
        };
    }

    private void fixAuthentication(Endpoint endpoint) {
        logger.d("Found endpoint with incorrect authentication (uri: {})", endpoint.getSourceUri());

        String username = chapConfigurator.generateUsername();
        String password = chapConfigurator.generateSecret();

        try {
            endpointUpdateInvoker.updateEndpoint(endpoint, new EndpointUpdateDefinition(username, password));
            logger.d("Username in Endpoint (uri: {}) recreated", endpoint.getSourceUri());
        } catch (BusinessApiException e) {
            logger.e(format("Reconfiguring Endpoint (uri: %s) failed", endpoint.getSourceUri()), e);
        }

        networkDeviceFunctionAuthenticationUpdater.updateAuthenticationIfNetworkDeviceFunctionPresentInEndpoint(endpoint, username, password);
    }
}
