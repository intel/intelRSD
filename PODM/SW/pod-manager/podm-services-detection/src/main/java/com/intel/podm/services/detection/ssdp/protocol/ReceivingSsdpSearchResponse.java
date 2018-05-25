/*
 * Copyright (c) 2016-2018 Intel Corporation
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

package com.intel.podm.services.detection.ssdp.protocol;

import com.intel.podm.discovery.external.ServiceDescriptor;
import com.intel.podm.services.detection.ssdp.model.message.IncomingSsdpSearchResponse;
import org.fourthline.cling.UpnpService;
import org.fourthline.cling.model.ValidationError;
import org.fourthline.cling.model.ValidationException;
import org.fourthline.cling.model.message.IncomingDatagramMessage;
import org.fourthline.cling.model.message.UpnpResponse;
import org.fourthline.cling.model.message.discovery.IncomingSearchResponse;
import org.fourthline.cling.model.meta.RemoteDevice;
import org.fourthline.cling.model.meta.RemoteDeviceIdentity;
import org.fourthline.cling.model.types.UDN;
import org.fourthline.cling.protocol.ReceivingAsync;
import org.fourthline.cling.transport.RouterException;

import java.util.logging.Logger;

import static java.util.Optional.ofNullable;
import static java.util.stream.Collectors.joining;

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

    protected void execute() throws RouterException {
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
        RemoteDevice remoteDevice;
        try {
            remoteDevice = new RemoteDevice(rdIdentity);
        } catch (ValidationException e) {
            LOG.warning("Validation errors of device during discovery: " + rdIdentity);
            ofNullable(e.getErrors()).ifPresent(
                    validationErrors -> LOG.warning(validationErrors.stream().map(ValidationError::toString).collect(joining(", "))));
            return;
        }

        if (validateRemoteDevice(rdIdentity)) {
            getUpnpService().getConfiguration().getAsyncProtocolExecutor().execute(
                    new RetrieveRedfishServiceDescription(getUpnpService(), remoteDevice, serviceDescriptor)
            );
        }
    }

    private boolean validateRemoteDevice(RemoteDeviceIdentity rdIdentity) {
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
