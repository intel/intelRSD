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

package com.intel.rsd.nodecomposer.business.redfish.services.actions;

import com.intel.rsd.nodecomposer.externalservices.WebClient;
import com.intel.rsd.nodecomposer.externalservices.WebClientBuilder;
import com.intel.rsd.nodecomposer.externalservices.WebClientRequestException;
import com.intel.rsd.nodecomposer.externalservices.actions.NetworkDeviceFunctionRequest;
import com.intel.rsd.nodecomposer.externalservices.resources.redfish.NetworkDeviceFunctionResource;
import com.intel.rsd.nodecomposer.mappers.redfish.NetworkDeviceFunctionMapper;
import com.intel.rsd.nodecomposer.persistence.redfish.NetworkDeviceFunction;
import com.intel.rsd.nodecomposer.types.actions.NetworkDeviceFunctionUpdateDefinition;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import javax.transaction.Transactional;

import static javax.transaction.Transactional.TxType.MANDATORY;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_SINGLETON;

@Slf4j
@Component
@Scope(SCOPE_SINGLETON)
public class NetworkDeviceFunctionInvoker {
    private final WebClientBuilder webClientBuilder;
    private final NetworkDeviceFunctionMapper networkDeviceFunctionMapper;

    @Autowired
    public NetworkDeviceFunctionInvoker(WebClientBuilder webClientBuilder, NetworkDeviceFunctionMapper networkDeviceFunctionMapper) {
        this.webClientBuilder = webClientBuilder;
        this.networkDeviceFunctionMapper = networkDeviceFunctionMapper;
    }

    @Transactional(MANDATORY)
    public void updateNetworkDeviceFunction(NetworkDeviceFunction deviceFunction,
                                            NetworkDeviceFunctionUpdateDefinition updateDefinition) throws WebClientRequestException {
        try (WebClient webClient = webClientBuilder.createResourceManagerInstance().retryable().build()) {
            NetworkDeviceFunctionRequest request = new NetworkDeviceFunctionRequest(updateDefinition);
            NetworkDeviceFunctionResource networkDeviceFunctionResource = webClient.patchAndRetrieve(deviceFunction.getUri().toUri(), request);
            networkDeviceFunctionMapper.map(networkDeviceFunctionResource, deviceFunction);
        }
    }
}
