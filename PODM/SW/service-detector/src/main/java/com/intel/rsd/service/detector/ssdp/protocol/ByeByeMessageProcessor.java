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

import org.fourthline.cling.UpnpService;
import org.fourthline.cling.model.ValidationException;
import org.fourthline.cling.model.message.discovery.IncomingNotificationRequest;
import org.fourthline.cling.model.meta.RemoteDevice;
import org.fourthline.cling.model.meta.RemoteDeviceIdentity;

import java.util.logging.Logger;

class ByeByeMessageProcessor implements MessageProcessor {

    private static final Logger LOG = Logger.getLogger(ByeByeMessageProcessor.class.getName());

    @Override
    public void run(UpnpService upnpService, RemoteDeviceIdentity rdIdentity, IncomingNotificationRequest request) throws ValidationException {
        LOG.fine("Received device BYEBYE advertisement");
        RemoteDevice remoteDevice = new RemoteDevice(rdIdentity);
        boolean removed = upnpService.getRegistry().removeDevice(remoteDevice);
        if (removed) {
            LOG.fine("Removed remote device from registry: " + remoteDevice);
        }
    }
}
