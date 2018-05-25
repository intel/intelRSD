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

package com.intel.podm.business.redfish.services.actions;

import com.intel.podm.business.EntityOperationException;
import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.business.entities.redfish.NetworkDeviceFunction;
import com.intel.podm.client.WebClient;
import com.intel.podm.client.WebClientBuilder;
import com.intel.podm.client.WebClientRequestException;
import com.intel.podm.client.actions.NetworkDeviceFunctionRequest;
import com.intel.podm.client.resources.redfish.NetworkDeviceFunctionResource;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.types.actions.NetworkDeviceFunctionUpdateDefinition;
import com.intel.podm.mappers.redfish.NetworkDeviceFunctionMapper;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.net.URI;

import static com.intel.podm.common.utils.Contracts.requires;
import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
public class NetworkDeviceFunctionInvoker {
    @Inject
    private WebClientBuilder webClientBuilder;

    @Inject
    private Logger logger;

    @Inject
    private NetworkDeviceFunctionMapper networkDeviceFunctionMapper;

    @Transactional(MANDATORY)
    public void updateNetworkDeviceFunction(NetworkDeviceFunction networkDeviceFunction,
                                            NetworkDeviceFunctionUpdateDefinition networkDeviceFunctionDefinition) throws EntityOperationException {
        ExternalService service = networkDeviceFunction.getService();
        requires(service != null, "There is no Service associated with selected Network Device Function");

        URI baseUri = service.getBaseUri();
        URI sourceUri = networkDeviceFunction.getSourceUri();

        try (WebClient webClient = webClientBuilder.newInstance(baseUri).retryable().build()) {
            NetworkDeviceFunctionRequest request = new NetworkDeviceFunctionRequest(networkDeviceFunctionDefinition);
            NetworkDeviceFunctionResource networkDeviceFunctionResource = webClient.patchAndRetrieve(sourceUri, request);
            networkDeviceFunctionMapper.map(networkDeviceFunctionResource, networkDeviceFunction);
        } catch (WebClientRequestException e) {
            String errorMessage = "Update on selected NetworkDeviceFunction failed";
            logger.w(errorMessage + " on [ service: {}, path: {} ]", service.getBaseUri(), sourceUri);
            throw new EntityOperationException(errorMessage, e);
        }
    }
}
