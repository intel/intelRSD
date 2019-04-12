/*
 * Copyright (c) 2019 Intel Corporation
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

package com.intel.rsd.service.detector.endpoint.registration;

import com.intel.rsd.service.detector.ServiceDescriptor;
import com.intel.rsd.service.detector.data.ServiceRepository;
import lombok.NonNull;

import java.net.URI;

public class HttpsOnlyServiceRegistrar extends AbstractServiceRegistrar {
    public HttpsOnlyServiceRegistrar(ServiceDescriptor serviceDescriptor, ServiceRepository serviceRepository) {
        super(serviceDescriptor, serviceRepository);
    }

    @Override
    protected void validateServiceUri(@NonNull URI serviceUri) throws ServiceDefinitionException {
        if (serviceUri.getScheme() == null || !serviceUri.getScheme().toUpperCase().equals("HTTPS")) {
            throw new ServiceDefinitionException("Only services accessed via HTTPS are supported!");
        }
    }
}
