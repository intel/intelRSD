/*
 * Copyright (c) 2016 Intel Corporation
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

package com.intel.podm.client.typeidresolver;

import com.intel.podm.client.api.resources.ExternalServiceResource;
import com.intel.podm.client.resources.MembersListResource;
import com.intel.podm.client.resources.redfish.ManagerResourceImpl;
import com.intel.podm.client.resources.redfish.ProcessorResourceImpl;
import com.intel.podm.client.resources.redfish.properties.OemVendorImpl;
import com.intel.podm.common.logger.Logger;

import javax.annotation.PostConstruct;
import javax.ejb.Singleton;
import javax.ejb.Startup;
import javax.enterprise.inject.Instance;
import javax.inject.Inject;

import static com.intel.podm.client.typeidresolver.OdataTypeMatcher.odataTypePatternMatcher;
import static com.intel.podm.client.typeidresolver.OdataTypeMatcher.simpleOdataTypeMatcher;
import static com.intel.podm.client.typeidresolver.ResourceResolver.register;
import static com.intel.podm.client.typeidresolver.ResourceResolver.registerResource;

@Singleton
@Startup
public class ResourceProvider {

    @Inject
    private Logger logger;

    @Inject
    private Instance<ExternalServiceResource> externalServiceResources;

    @Inject
    private Instance<OemVendorImpl> oemResources;

    @PostConstruct
    public void resourceProvider() {
        logger.d("Registering supported OData types...");
        registerRedfishResourceOdataTypes();
        registerOemOdataTypes();
        registerKnownIncorrectOdataTypes();
    }

    private void registerRedfishResourceOdataTypes() {
        for (ExternalServiceResource resource : externalServiceResources) {
            registerResource(resource.getClass());
        }
    }

    private void registerOemOdataTypes() {
        for (OemVendorImpl resource : oemResources) {
            registerResource(resource.getClass());
        }
    }

    private void registerKnownIncorrectOdataTypes() {
        register(odataTypePatternMatcher(".*Processor.+Processor", ProcessorResourceImpl.class));
        register(simpleOdataTypeMatcher("#Managers.1.0.0.Manager", ManagerResourceImpl.class));
        register(simpleOdataTypeMatcher("#Processors.1.0.0.ProcessorsCollection", MembersListResource.class));
    }
}
