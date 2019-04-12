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
import com.intel.rsd.service.detector.ServiceEndpoint;
import com.intel.rsd.service.detector.data.Service;
import com.intel.rsd.service.detector.data.ServiceRepository;
import lombok.NonNull;
import lombok.val;
import org.springframework.dao.DataIntegrityViolationException;

import java.net.URI;

import static java.net.URI.create;
import static org.apache.commons.lang3.StringUtils.removeEnd;

public abstract class AbstractServiceRegistrar implements ServiceRegistrar {
    private final ServiceDescriptor serviceDescriptor;
    private final ServiceRepository serviceRepository;

    protected AbstractServiceRegistrar(ServiceDescriptor serviceDescriptor, ServiceRepository serviceRepository) {
        this.serviceDescriptor = serviceDescriptor;
        this.serviceRepository = serviceRepository;
    }

    @Override
    public final URI register(String serviceUriString) throws ServiceRegistrationException {
        val remoteRedfishServiceUri = extractRemoteRedfishServiceUri(serviceUriString);
        validateServiceUri(remoteRedfishServiceUri);
        val serviceEndpoint = serviceDescriptor.describe(remoteRedfishServiceUri);
        val serviceEntity = prepareServiceEntity(serviceEndpoint);
        try {
            val service = serviceRepository.save(serviceEntity);
            return create("/redfish/v1/Managers/" + service.getId());
        } catch (DataIntegrityViolationException e) {
            throw new ServicePersistingException(String.format("Could not persist service %s", serviceEntity.getServiceUri()));
        }
    }

    protected abstract void validateServiceUri(URI serviceUri) throws ServiceDefinitionException;

    private URI extractRemoteRedfishServiceUri(@NonNull String serviceUriString) {
        try {
            return URI.create(removeTrailingSlash(serviceUriString));
        } catch (IllegalArgumentException e) {
            throw new ServiceDefinitionException("RemoteRedfishServiceUri contains an invalid URI");
        }
    }

    private String removeTrailingSlash(@NonNull String uri) {
        return removeEnd(uri, "/");
    }

    private Service prepareServiceEntity(ServiceEndpoint serviceEndpoint) {
        val serviceUuid = serviceEndpoint.getServiceUuid();
        if (!serviceRepository.findById(serviceUuid.toString()).isPresent()) {
            return new Service(serviceUuid, serviceEndpoint.getEndpointUri(), serviceEndpoint.getRemoteServiceType());
        }

        throw new ServiceStateConflictException("Requested service is already registered");
    }
}
