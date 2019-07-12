/*
 * Copyright (c) 2018-2019 Intel Corporation
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

import com.intel.rsd.nodecomposer.business.redfish.services.actions.NetworkDeviceFunctionInvoker;
import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.externalservices.WebClientRequestException;
import com.intel.rsd.nodecomposer.persistence.dao.GenericDao;
import com.intel.rsd.nodecomposer.persistence.redfish.Endpoint;
import com.intel.rsd.nodecomposer.persistence.redfish.NetworkDeviceFunction;
import com.intel.rsd.nodecomposer.types.actions.NetworkDeviceFunctionUpdateDefinition;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import java.util.Optional;

import static com.intel.rsd.nodecomposer.types.EntityRole.INITIATOR;
import static com.intel.rsd.nodecomposer.types.Ref.of;
import static com.intel.rsd.nodecomposer.utils.Collector.toSingleOrNull;
import static java.lang.String.format;
import static java.util.Objects.nonNull;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_SINGLETON;

@Slf4j
@Component
@Scope(SCOPE_SINGLETON)
public class NetworkDeviceFunctionAuthenticationUpdater {
    private final NetworkDeviceFunctionInvoker networkDeviceFunctionInvoker;
    private final GenericDao genericDao;

    @Autowired
    public NetworkDeviceFunctionAuthenticationUpdater(NetworkDeviceFunctionInvoker networkDeviceFunctionInvoker, GenericDao genericDao) {
        this.networkDeviceFunctionInvoker = networkDeviceFunctionInvoker;
        this.genericDao = genericDao;
    }

    void updateAuthenticationIfNetworkDeviceFunctionIsPresentInEndpoint(Endpoint endpoint, String username, String password) {
        retrieveNetworkDeviceFunction(endpoint).ifPresent(networkDeviceFunction -> {
            ODataId networkDeviceFunctionOdataId = networkDeviceFunction.getUri();
            try {
                NetworkDeviceFunction deviceFunction = genericDao.find(NetworkDeviceFunction.class, networkDeviceFunctionOdataId);
                NetworkDeviceFunctionUpdateDefinition updateDefinition = prepareUpdateDefinition(endpoint.hasRole(INITIATOR), username, password);
                networkDeviceFunctionInvoker.updateNetworkDeviceFunction(deviceFunction, updateDefinition);
                log.debug("NetworkDeviceFunction (uri: {}) credentials associated with Endpoint(uri: {}) updated successfully",
                    networkDeviceFunctionOdataId, endpoint.getUri());
            } catch (WebClientRequestException e) {
                log.error(format("Reconfiguring NetworkDeviceFunction (uri: %s) failed", networkDeviceFunctionOdataId), e);
            }
        });
    }

    private NetworkDeviceFunctionUpdateDefinition prepareUpdateDefinition(boolean isInitiator, String username, String password) {
        NetworkDeviceFunctionUpdateDefinition.IscsiBootDefinition bootDefinition = new NetworkDeviceFunctionUpdateDefinition.IscsiBootDefinition();
        if (isInitiator) {
            bootDefinition.setChapUsername(of(username));
            bootDefinition.setChapSecret(of(password));
        } else {
            bootDefinition.setMutualChapUsername(of(username));
            bootDefinition.setMutualChapSecret(of(password));
        }

        NetworkDeviceFunctionUpdateDefinition updateDefinition = new NetworkDeviceFunctionUpdateDefinition();
        updateDefinition.setIscsiBoot(of(bootDefinition));
        return updateDefinition;
    }

    private Optional<NetworkDeviceFunction> retrieveNetworkDeviceFunction(Endpoint endpoint) {
        Endpoint initiator = retrieveEndpointInitiator(endpoint);

        if (initiator == null || initiator.getComputerSystem() == null) {
            return Optional.empty();
        }

        return initiator.getComputerSystem().getNetworkDeviceFunction();
    }

    private Endpoint retrieveEndpointInitiator(Endpoint endpoint) {
        if (endpoint.getComputerSystem() != null) {
            return endpoint;
        }

        if (endpoint.getZone() == null) {
            return null;
        }

        return endpoint.getZone().getEndpoints().stream()
            .filter(ep -> nonNull(ep.getComputerSystem()))
            .collect(toSingleOrNull());
    }
}
