/*
 * Copyright (c) 2016-2017 Intel Corporation
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

package com.intel.podm.client.actions;

import com.intel.podm.client.actions.requests.ChassisUpdateRequest;
import com.intel.podm.client.actions.requests.LegacyRackChassisUpdateRequest;
import com.intel.podm.client.api.ExternalServiceApiActionException;
import com.intel.podm.client.api.ExternalServiceApiReaderException;
import com.intel.podm.client.api.WebClient;
import com.intel.podm.client.api.actions.ChassisResourceActions;
import com.intel.podm.client.api.resources.redfish.ChassisResource;
import com.intel.podm.common.types.actions.BaseChassisUpdateDefinition;
import com.intel.podm.common.types.actions.ChassisUpdateDefinition;
import com.intel.podm.common.types.actions.LegacyRackChassisUpdateDefinition;

import java.net.URI;

public class ChassisResourceActionsImpl implements ChassisResourceActions {

    private WebClient webClient;

    public ChassisResourceActionsImpl(WebClient webClient) {
        this.webClient = webClient;
    }

    @Override
    public ChassisResource update(URI chassisUri, BaseChassisUpdateDefinition chassisUpdateDefinition)
        throws ExternalServiceApiActionException, ExternalServiceApiReaderException {

        Object chassisUpdateRequest;
        if (chassisUpdateDefinition instanceof LegacyRackChassisUpdateDefinition) {
            chassisUpdateRequest = new LegacyRackChassisUpdateRequest((LegacyRackChassisUpdateDefinition) chassisUpdateDefinition);
        } else {
            chassisUpdateRequest = new ChassisUpdateRequest((ChassisUpdateDefinition) chassisUpdateDefinition);
        }

        return webClient.patchAndRetrieve(chassisUri, chassisUpdateRequest);
    }

    @Override
    public ChassisResource get(URI chassisUri) throws ExternalServiceApiReaderException {
        return (ChassisResource) webClient.get(chassisUri);
    }

    @Override
    public void close() {
        webClient.close();
    }
}
