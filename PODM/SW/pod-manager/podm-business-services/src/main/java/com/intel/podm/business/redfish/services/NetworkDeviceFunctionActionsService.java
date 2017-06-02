/*
 * Copyright (c) 2017 Intel Corporation
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

import com.intel.podm.actions.ActionException;
import com.intel.podm.actions.NetworkDeviceFunctionInvoker;
import com.intel.podm.business.ContextResolvingException;
import com.intel.podm.business.EntityOperationException;
import com.intel.podm.business.entities.redfish.NetworkDeviceFunction;
import com.intel.podm.business.redfish.EntityTreeTraverser;
import com.intel.podm.business.redfish.services.helpers.NetworkDeviceFunctionActionHelper;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.common.types.actions.NetworkDeviceFunctionUpdateDefinition;
import com.intel.podm.common.types.redfish.RedfishNetworkDeviceFunction;

import javax.inject.Inject;
import javax.transaction.Transactional;

import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

public class NetworkDeviceFunctionActionsService {

    @Inject
    private EntityTreeTraverser traverser;

    @Inject
    private NetworkDeviceFunctionInvoker networkDeviceFunctionInvoker;

    @Inject
    private NetworkDeviceFunctionActionHelper networkDeviceFunctionActionHelper;

    @Transactional(REQUIRES_NEW)
    public void updateNetworkDeviceFunction(Context context, RedfishNetworkDeviceFunction redfishNetworkDeviceFunction)
        throws ContextResolvingException, EntityOperationException {

        NetworkDeviceFunctionUpdateDefinition networkDeviceFunctionUpdateDefinition =
            networkDeviceFunctionActionHelper.networkDeviceFunctionDefinition(redfishNetworkDeviceFunction);

        NetworkDeviceFunction networkDeviceFunction = (NetworkDeviceFunction) traverser.traverse(context);

        try {
            networkDeviceFunctionInvoker.updateNetworkDeviceFunction(networkDeviceFunction, networkDeviceFunctionUpdateDefinition);
        } catch (ActionException e) {
            throw new EntityOperationException("Network device function creation action could not be completed! " + e.getMessage(), e);
        }
    }
}
