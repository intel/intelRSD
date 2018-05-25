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
import com.intel.podm.business.entities.dao.GenericDao;
import com.intel.podm.business.entities.redfish.NetworkDeviceFunction;
import com.intel.podm.business.redfish.services.actions.NetworkDeviceFunctionInvoker;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.synchronization.ThrowingRunnable;
import com.intel.podm.common.types.Id;
import com.intel.podm.common.types.actions.NetworkDeviceFunctionUpdateDefinition;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.net.URI;

import static com.intel.podm.common.types.Ref.of;
import static java.lang.String.format;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;
import static org.apache.commons.lang3.StringUtils.substringBefore;

@Dependent
public class NetworkDeviceFunctionAuthenticationUpdateTask implements ThrowingRunnable<EntityOperationException> {
    @Inject
    private Logger logger;

    @Inject
    private GenericDao genericDao;

    @Inject
    private NetworkDeviceFunctionInvoker networkDeviceFunctionInvoker;

    private Id networkDeviceFunctionId;
    private String username;
    private String password;
    private boolean isRelatedEndpointInRoleInitiator;
    private URI associatedEndpointUri;

    @Override
    @Transactional(REQUIRES_NEW)
    public void run() throws EntityOperationException {
        NetworkDeviceFunction networkDeviceFunction = genericDao.find(NetworkDeviceFunction.class, networkDeviceFunctionId);
        networkDeviceFunctionInvoker.updateNetworkDeviceFunction(networkDeviceFunction,
            prepareUpdateDefinition(isRelatedEndpointInRoleInitiator, username, password));
        logger.d("NetworkDeviceFunction (uri: {}) credentials associated with Endpoint(uri: {}) updated successfully",
            networkDeviceFunction.getSourceUri(), associatedEndpointUri);
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

    public void setNetworkDeviceFunctionId(Id networkDeviceFunctionId) {
        this.networkDeviceFunctionId = networkDeviceFunctionId;
    }

    public void setUsername(String username) {
        this.username = username;
    }

    public void setPassword(String password) {
        this.password = password;
    }

    public void setRelatedEndpointInRoleInitiator(boolean relatedEndpointInRoleInitiator) {
        isRelatedEndpointInRoleInitiator = relatedEndpointInRoleInitiator;
    }

    public void setAssociatedEndpointUri(URI associatedEndpointUri) {
        this.associatedEndpointUri = associatedEndpointUri;
    }

    @Override
    public String toString() {
        return format("%s(#%s)", substringBefore(getClass().getSimpleName(), "$"), networkDeviceFunctionId);
    }
}
