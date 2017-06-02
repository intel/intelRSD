/*
 * Copyright (c) 2015-2017 Intel Corporation
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

import com.intel.podm.client.api.ExternalServiceApiActionException;
import com.intel.podm.client.api.ExternalServiceApiReaderException;
import com.intel.podm.client.api.WebClient;
import com.intel.podm.client.api.actions.EthernetSwitchPortResourceActions;
import com.intel.podm.client.api.actions.EthernetSwitchPortResourceCreationRequest;
import com.intel.podm.client.api.actions.EthernetSwitchPortResourceModificationRequest;
import com.intel.podm.client.api.resources.redfish.EthernetSwitchPortResource;

import java.net.URI;

public class EthernetSwitchPortResourceActionsImpl implements EthernetSwitchPortResourceActions {

    private WebClient webClient;

    EthernetSwitchPortResourceActionsImpl(WebClient webClient) {
        this.webClient = webClient;
    }

    @Override
    public URI createSwitchPort(URI switchPortCollectionUri, EthernetSwitchPortResourceCreationRequest creationRequest)
            throws ExternalServiceApiActionException {
        return webClient.post(switchPortCollectionUri, creationRequest);
    }

    @Override
    public EthernetSwitchPortResource getSwitchPort(URI switchPortUri) throws ExternalServiceApiReaderException {
        if (switchPortUri == null) {
            throw new ExternalServiceApiReaderException("Could not read Switch Port", null);
        }

        return (EthernetSwitchPortResource) webClient.get(switchPortUri);
    }

    @Override
    public void updateSwitchPort(URI switchPortUri, EthernetSwitchPortResourceModificationRequest modificationRequest)
            throws ExternalServiceApiActionException {
        webClient.patch(switchPortUri, modificationRequest);
    }

    @Override
    public void deleteSwitchPort(URI switchPortUri) throws ExternalServiceApiActionException {
        webClient.delete(switchPortUri);
    }

    @Override
    public void close() {
        webClient.close();
    }
}
