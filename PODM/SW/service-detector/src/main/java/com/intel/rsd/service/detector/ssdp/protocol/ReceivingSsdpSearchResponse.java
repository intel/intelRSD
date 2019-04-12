/*
 * Copyright (c) 2016-2019 Intel Corporation
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

package com.intel.rsd.service.detector.ssdp.protocol;

import com.intel.rsd.service.detector.ServiceDescriptor;
import com.intel.rsd.service.detector.ssdp.model.message.IncomingSsdpSearchResponse;
import org.fourthline.cling.UpnpService;
import org.fourthline.cling.model.message.IncomingDatagramMessage;
import org.fourthline.cling.model.message.UpnpResponse;
import org.fourthline.cling.model.message.discovery.IncomingSearchResponse;
import org.fourthline.cling.model.meta.RemoteDevice;
import org.fourthline.cling.model.meta.RemoteDeviceIdentity;
import org.fourthline.cling.model.types.UDN;
import org.fourthline.cling.protocol.ReceivingAsync;

import java.util.logging.Logger;

/**
 * Class responsible for registering Rack Scale based SSDP messages and overriding default behaviour for checking
 * incoming M-SEARCH responses from SSDP protocol (ex. removing checking SOAP descriptors)
 *
 * @see org.fourthline.cling.protocol.async.ReceivingSearchResponse
 */
class ReceivingSsdpSearchResponse extends ReceivingAsync<IncomingSearchResponse> {

    private static final Logger LOG = Logger.getLogger(ReceivingSsdpSearchResponse.class.getName());

    private final ServiceDescriptor serviceDescriptor;

    ReceivingSsdpSearchResponse(UpnpService upnpService,
                                IncomingDatagramMessage<UpnpResponse> inputMessage,
                                ServiceDescriptor serviceDescriptor) {

        super(upnpService, new IncomingSsdpSearchResponse(inputMessage));
        this.serviceDescriptor = serviceDescriptor;
    }

    protected void execute() {
        if (!validateInputMessage()) {
            return;
        }

        UDN udn = getInputMessage().getRootDeviceUDN();

        RemoteDeviceIdentity rdIdentity = new RemoteDeviceIdentity(getInputMessage());
        LOG.fine("Received device search response: " + rdIdentity);

        updateIpAddressIfNecessary(rdIdentity);

        if (getUpnpService().getRegistry().update(rdIdentity)) {
            LOG.fine("Remote device was already known: " + udn);
        } else {
            addDevice(rdIdentity);
        }
    }

    private void updateIpAddressIfNecessary(RemoteDeviceIdentity rdIdentity) {
        RemoteDevice remoteDevice = getUpnpService().getRegistry().getRemoteDevice(rdIdentity.getUdn(), false);

        if (remoteDevice != null) {
            if (!remoteDevice.getIdentity().getDescriptorURL().equals(rdIdentity.getDescriptorURL())) {
                getUpnpService().getRegistry().removeDevice(remoteDevice);
            }
        }
    }

    private boolean validateInputMessage() {
        if (!getInputMessage().isSearchResponseMessage()) {
            LOG.fine("Ignoring invalid search response message: " + getInputMessage());
            return false;
        }

        if (getInputMessage().getRootDeviceUDN() == null) {
            LOG.fine("Ignoring search response message without UDN: " + getInputMessage());
            return false;
        }
        return true;
    }

    private void addDevice(RemoteDeviceIdentity rdIdentity) {
        if (!validateDescriptorUrlAndMaxAgeHeaders(rdIdentity)) {
            return;
        }

        getUpnpService()
            .getConfiguration()
            .getAsyncProtocolExecutor()
            .execute(new RetrieveRedfishServiceDescription(getUpnpService(), rdIdentity, serviceDescriptor));
    }

    private boolean validateDescriptorUrlAndMaxAgeHeaders(RemoteDeviceIdentity rdIdentity) {
        if (rdIdentity.getDescriptorURL() == null) {
            LOG.finer("Ignoring message without location URL header: " + getInputMessage());
            return false;
        }

        if (rdIdentity.getMaxAgeSeconds() == null) {
            LOG.finer("Ignoring message without max-age header: " + getInputMessage());
            return false;
        }
        return true;
    }
}
