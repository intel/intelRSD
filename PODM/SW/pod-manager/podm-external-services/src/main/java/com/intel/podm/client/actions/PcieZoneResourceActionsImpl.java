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

import com.intel.podm.client.actions.requests.PcieZoneAttachDetachEndpointRequest;
import com.intel.podm.client.api.ExternalServiceApiActionException;
import com.intel.podm.client.api.WebClient;
import com.intel.podm.client.api.actions.PcieZoneResourceActions;

import java.net.URI;
import java.util.Set;

public class PcieZoneResourceActionsImpl implements PcieZoneResourceActions {
    private final WebClient webClient;

    public PcieZoneResourceActionsImpl(WebClient webClient) {
        this.webClient = webClient;
    }

    @Override
    public void patchEndpointsInZoneAction(URI zoneActionUri, Set<URI> endpointUris) throws ExternalServiceApiActionException {
        webClient.patch(zoneActionUri, new PcieZoneAttachDetachEndpointRequest(endpointUris));
    }

    @Override
    public void close() {
        webClient.close();
    }
}
