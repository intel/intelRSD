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

package com.intel.rsd.service.detector.ssdp;

import com.intel.rsd.optional.OptionalUtils;
import com.intel.rsd.service.detector.ServiceDetectionListener;
import com.intel.rsd.service.detector.data.RemoteServiceType;
import lombok.SneakyThrows;
import lombok.extern.slf4j.Slf4j;
import org.fourthline.cling.model.meta.RemoteDevice;
import org.fourthline.cling.registry.DefaultRegistryListener;
import org.fourthline.cling.registry.Registry;

import java.net.URI;
import java.net.URISyntaxException;
import java.util.Optional;
import java.util.UUID;

import static java.util.UUID.fromString;

@Slf4j
public class SsdpRegistryListener extends DefaultRegistryListener {

    private ServiceDetectionListener detectionListener;

    public SsdpRegistryListener(ServiceDetectionListener detectionListener) {
        this.detectionListener = detectionListener;
    }

    @Override
    public void remoteDeviceAdded(Registry registry, RemoteDevice device) {
        log.debug("SSDP resource detected: {}", device);
        try {
            detectionListener.onServiceDetected(getUuid(device), getUri(device), getRemoteServiceType(device));
        } catch (IllegalArgumentException | URISyntaxException e) {
            log.debug("Cannot create ServiceEndpoint instance", e);
        }
    }

    private RemoteServiceType getRemoteServiceType(RemoteDevice device) {
        return Optional.of(device)
            .map(OptionalUtils.select(RsdDevice.class))
            .map(RsdDevice::getRemoteDeviceType)
            .orElseThrow(() -> new RuntimeException("Unexpected device!"));
    }

    @Override
    @SneakyThrows
    public void remoteDeviceRemoved(Registry registry, RemoteDevice device) {
        log.debug("SSDP resource removed: {}", device);
        detectionListener.onServiceRemoved(getUri(device));
    }

    private static URI getUri(RemoteDevice remoteDevice) throws URISyntaxException {
        return remoteDevice.getIdentity().getDescriptorURL().toURI();
    }

    private static UUID getUuid(RemoteDevice remoteDevice) {
        return fromString(remoteDevice.getIdentity().getUdn().getIdentifierString());
    }
}
