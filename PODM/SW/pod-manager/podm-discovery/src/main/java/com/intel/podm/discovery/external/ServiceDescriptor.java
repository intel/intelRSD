/*
 * Copyright (c) 2015-2017 Intel Corporation
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

package com.intel.podm.discovery.external;

import com.intel.podm.client.api.ExternalServiceApiReaderException;
import com.intel.podm.client.api.reader.ExternalServiceReader;
import com.intel.podm.client.api.reader.ExternalServiceReaderFactory;
import com.intel.podm.client.api.reader.ResourceSupplier;
import com.intel.podm.client.api.resources.redfish.RackscaleServiceRootResource;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.types.ServiceType;
import com.intel.podm.config.base.Config;
import com.intel.podm.config.base.Holder;
import com.intel.podm.config.base.dto.ServiceDetectionConfig;

import javax.enterprise.context.ApplicationScoped;
import javax.inject.Inject;
import java.net.URI;
import java.util.Iterator;

import static com.intel.podm.common.types.ServiceType.LUI;
import static com.intel.podm.common.types.ServiceType.PSME;

/**
 * Service Detection implementation based on obtaining UUID of service
 * being detected at given URI using REST client.
 */
@ApplicationScoped
public class ServiceDescriptor {

    private static final String LUI_COMPUTER_SYSTEM_URI_PART = "/redfish/v1/Systems/1";

    private static final ServiceType UNDEFINED_SERVICE_TYPE = null;

    @Inject
    private ExternalServiceReaderFactory externalServiceReaderFactory;

    @Config
    @Inject
    private Holder<ServiceDetectionConfig> serviceDetectionConfig;

    @Inject
    private Logger logger;

    public ServiceEndpoint describe(URI serviceUri) throws UnrecognizedServiceTypeException {
        return describe(serviceUri, UNDEFINED_SERVICE_TYPE);
    }

    public ServiceEndpoint describe(URI serviceUri, ServiceType predefinedServiceType) throws UnrecognizedServiceTypeException {
        try (ExternalServiceReader reader = externalServiceReaderFactory.createExternalServiceReader(serviceUri)) {
            RackscaleServiceRootResource redfishService = reader.getServiceRoot();
            ServiceType serviceType = determineServiceType(predefinedServiceType, redfishService);
            if (LUI.equals(serviceType)) {
                if (!checkIfLuiIsReady(redfishService)) {
                    throw new UnrecognizedServiceTypeException("LUI service is not ready yet.");
                }
            }
            return new ServiceEndpoint(serviceType, redfishService.getUuid(), serviceUri);
        } catch (ExternalServiceApiReaderException e) {
            throw new UnrecognizedServiceTypeException("Unable to retrieve Rack Scale service root.", e);
        }
    }

    private ServiceType determineServiceType(ServiceType predefinedServiceType, RackscaleServiceRootResource redfishService) {
        ServiceType serviceType = predefinedServiceType;
        if (serviceType == UNDEFINED_SERVICE_TYPE) {
            serviceType = serviceDetectionConfig.get().getServiceTypeMapping().getServiceTypeForName(redfishService.getName());
            if (serviceType == null) {
                serviceType = PSME;
            }
        }
        return serviceType;
    }

    private boolean checkIfLuiIsReady(RackscaleServiceRootResource redfishService) throws ExternalServiceApiReaderException {
        Iterator chassisCollection = redfishService.getComputerSystems().iterator();
        if (chassisCollection.hasNext()) {
            ResourceSupplier resourceSupplier = (ResourceSupplier) chassisCollection.next();
            if (resourceSupplier.getUri().toString().equals(LUI_COMPUTER_SYSTEM_URI_PART)) {
                return true;
            }
        }
        return false;
    }
}
