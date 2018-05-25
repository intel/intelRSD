/*
 * Copyright (c) 2018 Intel Corporation
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

import com.intel.podm.business.EntityOperationException;
import com.intel.podm.business.entities.redfish.Endpoint;
import com.intel.podm.business.entities.redfish.NetworkDeviceFunction;
import com.intel.podm.common.enterprise.utils.beans.BeanFactory;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.synchronization.TaskCoordinator;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.util.concurrent.TimeoutException;

import static com.intel.podm.common.types.EntityRole.INITIATOR;
import static com.intel.podm.common.utils.Collector.toSingleOrNull;
import static java.lang.String.format;
import static java.util.Objects.nonNull;

@Dependent
public class NetworkDeviceFunctionAuthenticationUpdater {
    @Inject
    private Logger logger;

    @Inject
    private BeanFactory beanFactory;

    @Inject
    private TaskCoordinator taskCoordinator;

    public void updateAuthenticationIfNetworkDeviceFunctionPresentInEndpoint(Endpoint endpoint, String username, String password) {
        NetworkDeviceFunction networkDeviceFunction = retrieveNetworkDeviceFunction(endpoint);
        if (networkDeviceFunction != null) {
            NetworkDeviceFunctionAuthenticationUpdateTask task =
                beanFactory.create(NetworkDeviceFunctionAuthenticationUpdateTask.class);

            task.setNetworkDeviceFunctionId(networkDeviceFunction.getId());
            task.setUsername(username);
            task.setPassword(password);
            task.setRelatedEndpointInRoleInitiator(endpoint.hasRole(INITIATOR));
            task.setAssociatedEndpointUri(endpoint.getSourceUri());

            try {
                taskCoordinator.run(networkDeviceFunction.getService().getUuid(), task);
            } catch (TimeoutException | EntityOperationException e) {
                logger.e(format("Reconfiguring NetworkDeviceFunction (uri: %s) failed", networkDeviceFunction.getSourceUri()), e);
            }
        }
    }

    private NetworkDeviceFunction retrieveNetworkDeviceFunction(Endpoint endpoint) {
        Endpoint initiator = retrieveEndpointInitiator(endpoint);

        if (initiator == null || initiator.getComputerSystem() == null) {
            return null;
        }

        return initiator.getComputerSystem().getNetworkDeviceFunctionOrNull();
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
