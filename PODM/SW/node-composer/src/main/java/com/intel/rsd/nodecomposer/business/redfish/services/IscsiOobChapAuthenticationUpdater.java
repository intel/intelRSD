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

package com.intel.rsd.nodecomposer.business.redfish.services;

import com.intel.rsd.nodecomposer.business.BusinessApiException;
import com.intel.rsd.nodecomposer.business.redfish.services.actions.EndpointUpdateInvoker;
import com.intel.rsd.nodecomposer.composition.assembly.ChapConfigurator;
import com.intel.rsd.nodecomposer.persistence.dao.EndpointDao;
import com.intel.rsd.nodecomposer.persistence.redfish.Endpoint;
import com.intel.rsd.nodecomposer.types.actions.EndpointUpdateDefinition;
import lombok.extern.slf4j.Slf4j;
import lombok.val;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import javax.transaction.Transactional;
import java.util.List;
import java.util.Objects;

import static com.intel.rsd.nodecomposer.types.Protocol.ISCSI;
import static java.lang.String.format;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_SINGLETON;

@Slf4j
@Component
@Scope(SCOPE_SINGLETON)
public class IscsiOobChapAuthenticationUpdater {
    private final ChapConfigurator chapConfigurator;
    private final EndpointDao endpointDao;
    private final EndpointUpdateInvoker endpointUpdateInvoker;
    private final NetworkDeviceFunctionAuthenticationUpdater networkDeviceFunctionAuthenticationUpdater;

    @Autowired
    public IscsiOobChapAuthenticationUpdater(ChapConfigurator chapConfigurator, EndpointDao endpointDao, EndpointUpdateInvoker endpointUpdateInvoker,
                                             NetworkDeviceFunctionAuthenticationUpdater networkDeviceFunctionAuthenticationUpdater) {
        this.chapConfigurator = chapConfigurator;
        this.endpointDao = endpointDao;
        this.endpointUpdateInvoker = endpointUpdateInvoker;
        this.networkDeviceFunctionAuthenticationUpdater = networkDeviceFunctionAuthenticationUpdater;
    }

    @Transactional(REQUIRES_NEW)
    public void checkIscsiOobAuthentication() {
        endpointDao.findEndpointsWithNullUsername(ISCSI).forEach(this::fixAuthentication);

        List<Endpoint> endpointsWithAssociatedComputerSystem = endpointDao.findEndpointsWithAssociatedComputerSystem(ISCSI);
        endpointsWithAssociatedComputerSystem.stream()
            .filter(this::incorrectCorrespondingUsernames)
            .forEach(this::fixAuthentication);
    }

    private boolean incorrectCorrespondingUsernames(Endpoint endpoint) {
        val networkDeviceFunction = endpoint.getComputerSystem().getNetworkDeviceFunction();

        if (networkDeviceFunction.isPresent()) {
            final String chapUsername = networkDeviceFunction.get().getIscsiBoot().getChapUsername();
            final String endpointUsername = endpoint.getAuthentication() != null ? endpoint.getAuthentication().getUsername() : null;

            return !Objects.equals(chapUsername, endpointUsername);
        }

        return false;
    }

    private void fixAuthentication(Endpoint endpoint) {
        log.debug("Found endpoint with incorrect authentication (uri: {})", endpoint.getUri());

        String username = chapConfigurator.generateUsername();
        String password = chapConfigurator.generateSecret();

        try {
            endpointUpdateInvoker.updateEndpoint(endpoint, new EndpointUpdateDefinition(username, password));
            log.debug("Username in Endpoint (uri: {}) recreated", endpoint.getUri());
        } catch (BusinessApiException e) {
            log.debug(format("Reconfiguring Endpoint (uri: %s) failed", endpoint.getUri()), e);
        }

        networkDeviceFunctionAuthenticationUpdater.updateAuthenticationIfNetworkDeviceFunctionIsPresentInEndpoint(endpoint, username, password);
    }
}
