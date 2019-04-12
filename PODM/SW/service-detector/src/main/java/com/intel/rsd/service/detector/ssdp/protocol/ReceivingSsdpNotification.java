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

import com.intel.rsd.service.detector.ssdp.model.message.IncomingSsdpNotificationRequest;
import org.fourthline.cling.UpnpService;
import org.fourthline.cling.model.ValidationError;
import org.fourthline.cling.model.ValidationException;
import org.fourthline.cling.model.message.discovery.IncomingNotificationRequest;
import org.fourthline.cling.model.meta.RemoteDeviceIdentity;
import org.fourthline.cling.protocol.ReceivingAsync;

import java.util.logging.Logger;

import static java.util.Optional.ofNullable;
import static java.util.stream.Collectors.joining;

/**
 * Class responsible for registering Rack Scale based SSDP messages and overriding default behaviour for checking
 * incoming messages from SSDP protocol (ex. removing checking SOAP descriptors)
 *
 * @see org.fourthline.cling.protocol.async.ReceivingNotification
 */
class ReceivingSsdpNotification extends ReceivingAsync<IncomingNotificationRequest> {

    private static final Logger LOG = Logger.getLogger(ReceivingSsdpNotification.class.getName());

    private final MessageProcessor messageProcessor;

    ReceivingSsdpNotification(UpnpService upnpService,
                              IncomingSsdpNotificationRequest inputNotificationMessage,
                              MessageProcessor messageProcessor) {

        super(upnpService, inputNotificationMessage);
        this.messageProcessor = messageProcessor;
    }

    protected void execute() {
        if (getInputMessage().getUDN() == null) {
            LOG.fine("Ignoring notification message without UDN: " + getInputMessage());
            return;
        }

        RemoteDeviceIdentity remoteDeviceIdentity = new RemoteDeviceIdentity(getInputMessage());
        LOG.fine("Received device notification: " + remoteDeviceIdentity);
        try {
            messageProcessor.run(getUpnpService(), remoteDeviceIdentity, getInputMessage());
        } catch (ValidationException e) {
            LOG.warning("Validation errors of device during discovery: " + remoteDeviceIdentity);
            ofNullable(e.getErrors()).ifPresent(
                    validationErrors -> LOG.warning(validationErrors.stream().map(ValidationError::toString).collect(joining(", ")))
            );
        }
    }
}
