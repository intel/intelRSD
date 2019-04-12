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
import org.fourthline.cling.UpnpService;
import org.fourthline.cling.model.ValidationException;
import org.fourthline.cling.model.message.discovery.IncomingNotificationRequest;
import org.fourthline.cling.model.meta.RemoteDevice;
import org.fourthline.cling.model.meta.RemoteDeviceIdentity;

import java.util.logging.Logger;

class AliveMessageProcessor implements MessageProcessor {

    private static final Logger LOG = Logger.getLogger(AliveMessageProcessor.class.getName());

    private final ServiceDescriptor serviceDescriptor;

    AliveMessageProcessor(ServiceDescriptor serviceDescriptor) {
        this.serviceDescriptor = serviceDescriptor;
    }

    @Override
    public void run(UpnpService upnpService, RemoteDeviceIdentity remoteDeviceIdentity, IncomingNotificationRequest request) throws ValidationException {
        LOG.fine("Received device ALIVE advertisement, descriptor location is: " + remoteDeviceIdentity.getDescriptorURL());
        if (!validateIncomingAliveMessage(remoteDeviceIdentity, request)) {
            return;
        }

        updateIpAddressIfNecessary(upnpService, remoteDeviceIdentity, request);

        if (upnpService.getRegistry().update(remoteDeviceIdentity)) {
            LOG.finer("Remote device was already known: " + remoteDeviceIdentity.getUdn());
        } else {
            upnpService.getConfiguration().getAsyncProtocolExecutor().execute(
                new RetrieveRedfishServiceDescription(upnpService, remoteDeviceIdentity, serviceDescriptor)
            );
        }
    }

    private void updateIpAddressIfNecessary(UpnpService upnpService, RemoteDeviceIdentity remoteDeviceIdentity, IncomingNotificationRequest request) {
        RemoteDevice remoteDevice = upnpService.getRegistry().getRemoteDevice(remoteDeviceIdentity.getUdn(), false);

        if (remoteDevice != null) {
            if (!remoteDevice.getIdentity().getDescriptorURL().equals(request.getLocationURL())) {
                upnpService.getRegistry().removeDevice(remoteDevice);
            }
        }
    }

    private boolean validateIncomingAliveMessage(RemoteDeviceIdentity remoteDeviceIdentity, IncomingNotificationRequest request) {
        if (remoteDeviceIdentity.getDescriptorURL() == null) {
            LOG.finer("Ignoring message without location URL header: " + request);
            return false;
        }

        if (remoteDeviceIdentity.getMaxAgeSeconds() == null) {
            LOG.finer("Ignoring message without max-age header: " + request);
            return false;
        }
        return true;
    }
}
