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

package com.intel.podm.services.detection.ssdp;

import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.types.ServiceType;
import com.intel.podm.discovery.external.ServiceDetectionListener;
import com.intel.podm.common.types.discovery.ServiceEndpoint;
import org.fourthline.cling.registry.event.Phase;
import org.fourthline.cling.registry.event.RemoteDeviceDiscovery;

import javax.enterprise.context.Dependent;
import javax.enterprise.event.Observes;
import javax.inject.Inject;
import java.net.URI;
import java.net.URISyntaxException;
import java.util.UUID;

import static com.intel.podm.common.types.ServiceType.valueOf;
import static java.util.UUID.fromString;

@Dependent
public class SsdpRegistryObserver {

    @Inject
    private Logger logger;

    @Inject
    private ServiceDetectionListener detectionListener;

    public void deviceDetected(@Observes @Phase.Complete RemoteDeviceDiscovery deviceAddedEvent) {
        logger.d("SSDP resource detected: {}", deviceAddedEvent.getDevice());
        try {
            ServiceEndpoint serviceEndpoint = createServiceEndpoint(deviceAddedEvent);
            detectionListener.onServiceDetected(serviceEndpoint);
        } catch (IllegalArgumentException | URISyntaxException e) {
            logger.d("Cannot create ServiceEndpoint instance", e);
        }
    }

    public void deviceRemoved(@Observes @Phase.Byebye RemoteDeviceDiscovery deviceRemovedEvent) {
        logger.d("SSDP resource removed: {}", deviceRemovedEvent.getDevice());

        try {
            ServiceEndpoint serviceEndpoint = createServiceEndpoint(deviceRemovedEvent);
            detectionListener.onServiceRemoved(serviceEndpoint);
        } catch (IllegalArgumentException | URISyntaxException e) {
            logger.d("Cannot create ServiceEndpoint instance", e);
        }
    }

    private ServiceEndpoint createServiceEndpoint(RemoteDeviceDiscovery deviceAddedEvent) throws URISyntaxException {
        return new ServiceEndpoint(
                        getServiceType(deviceAddedEvent),
                        getUuid(deviceAddedEvent),
                        getUri(deviceAddedEvent)
                );
    }

    private ServiceType getServiceType(RemoteDeviceDiscovery deviceAddedEvent) {
        return valueOf(deviceAddedEvent.getDevice().getDetails().getFriendlyName());
    }

    private static URI getUri(RemoteDeviceDiscovery deviceDiscovery) throws URISyntaxException {
        return deviceDiscovery.getDevice().getIdentity().getDescriptorURL().toURI();
    }

    private static UUID getUuid(RemoteDeviceDiscovery deviceDiscovery) {
        return fromString(deviceDiscovery.getDevice().getIdentity().getUdn().getIdentifierString());
    }
}
