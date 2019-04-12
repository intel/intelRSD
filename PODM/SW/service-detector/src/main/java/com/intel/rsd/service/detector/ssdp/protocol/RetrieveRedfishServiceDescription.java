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
import com.intel.rsd.service.detector.ServiceEndpoint;
import com.intel.rsd.service.detector.ssdp.RsdDevice;
import lombok.extern.slf4j.Slf4j;
import lombok.val;
import org.fourthline.cling.UpnpService;
import org.fourthline.cling.model.ValidationError;
import org.fourthline.cling.model.ValidationException;
import org.fourthline.cling.model.meta.RemoteDeviceIdentity;
import org.springframework.web.client.ResourceAccessException;

import java.net.URISyntaxException;

import static java.util.Optional.ofNullable;
import static java.util.stream.Collectors.joining;

@Slf4j
@SuppressWarnings("checkstyle:IllegalCatch")
class RetrieveRedfishServiceDescription implements Runnable {

    private final UpnpService upnpService;

    private final ServiceDescriptor serviceDescriptor;

    private final RemoteDeviceIdentity remoteDeviceIdentity;

    RetrieveRedfishServiceDescription(UpnpService upnpService, RemoteDeviceIdentity remoteDeviceIdentity, ServiceDescriptor serviceDescriptor) {
        this.upnpService = upnpService;
        this.serviceDescriptor = serviceDescriptor;
        this.remoteDeviceIdentity = remoteDeviceIdentity;
    }

    @Override
    public void run() {
        try {
            ServiceEndpoint serviceEndpoint = serviceDescriptor.describe(remoteDeviceIdentity.getDescriptorURL().toURI());
            val rsdDevice = new RsdDevice(remoteDeviceIdentity, serviceEndpoint.getRemoteServiceType());
            upnpService.getRegistry().addDevice(rsdDevice);

        } catch (URISyntaxException e) {
            log.warn(e.getMessage());

        } catch (ValidationException e) {
            log.warn("Validation errors of device during discovery: " + remoteDeviceIdentity);
            ofNullable(e.getErrors())
                .ifPresent(validationErrors -> log.warn(validationErrors.stream().map(ValidationError::toString).collect(joining(", "))));

        } catch (ResourceAccessException e) {
            log.warn("Cannot describe given service:" + e.getMessage());

        } catch (Exception e) {
            log.error("Unexpected exception: " + e.getMessage(), e);
        }
    }

}
