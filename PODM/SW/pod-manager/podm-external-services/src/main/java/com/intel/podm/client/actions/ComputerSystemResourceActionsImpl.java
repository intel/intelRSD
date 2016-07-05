/*
 * Copyright (c) 2015 Intel Corporation
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

import com.intel.podm.client.WebClientImpl;
import com.intel.podm.client.actions.requests.BootSourceOverrideRequest;
import com.intel.podm.client.actions.requests.ResetRequest;
import com.intel.podm.client.api.ExternalServiceApiActionException;
import com.intel.podm.client.api.ExternalServiceApiReaderException;
import com.intel.podm.client.api.WebClient;
import com.intel.podm.client.api.actions.ComputerSystemResourceActions;
import com.intel.podm.client.api.resources.redfish.ComputerSystemResource;
import com.intel.podm.common.types.BootSourceState;
import com.intel.podm.common.types.BootSourceType;
import com.intel.podm.common.types.actions.ResetType;

import java.net.URI;

import static java.net.URI.create;

public class ComputerSystemResourceActionsImpl implements ComputerSystemResourceActions {

    private static final String COMPUTER_SYSTEM_RESET_ACTION_PATH_PART = "/Actions/ComputerSystem.Reset";

    private WebClient webClient;

    public ComputerSystemResourceActionsImpl(URI baseUri) {
        this(WebClientImpl.createRetryable(baseUri));
    }

    public ComputerSystemResourceActionsImpl(WebClient webClient) {
        this.webClient = webClient;
    }

    @Override
    public ComputerSystemResource bootSourceOverride(URI computerSystemUri, BootSourceType type, BootSourceState state)
            throws ExternalServiceApiActionException, ExternalServiceApiReaderException {

        webClient.patch(computerSystemUri, new BootSourceOverrideRequest(type, state));
        return (ComputerSystemResource) webClient.get(computerSystemUri);
    }

    @Override
    public void reset(URI computerSystemUri, ResetType resetType) throws ExternalServiceApiActionException {
        URI resetActionUri = create(computerSystemUri.toString() + COMPUTER_SYSTEM_RESET_ACTION_PATH_PART);
        webClient.post(resetActionUri, new ResetRequest(resetType));
    }
}
