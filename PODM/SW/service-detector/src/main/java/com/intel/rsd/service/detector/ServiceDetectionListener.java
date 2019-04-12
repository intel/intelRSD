/*
 * Copyright (c) 2018-2019 Intel Corporation
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

package com.intel.rsd.service.detector;

import com.intel.rsd.service.detector.data.RemoteServiceType;
import com.intel.rsd.service.detector.data.Service;
import com.intel.rsd.service.detector.data.ServiceRepository;
import lombok.val;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;

import java.net.URI;
import java.util.UUID;

@Component
public class ServiceDetectionListener {
    private final ServiceRepository servicesRepository;

    @Autowired
    public ServiceDetectionListener(ServiceRepository servicesRepository) {
        this.servicesRepository = servicesRepository;
    }

    public void onServiceDetected(UUID serviceUuid, URI serviceUri, RemoteServiceType remoteServiceType) {
        val service = new Service(serviceUuid, serviceUri, remoteServiceType);
        servicesRepository.save(service);
    }

    public void onServiceRemoved(URI serviceUri) {
        servicesRepository.deleteByServiceUri(serviceUri);
    }
}
