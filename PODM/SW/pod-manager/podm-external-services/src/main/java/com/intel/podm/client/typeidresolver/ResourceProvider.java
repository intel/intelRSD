/*
 * Copyright (c) 2016-2017 Intel Corporation
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

import com.intel.podm.client.resources.ExternalServiceResource;
import com.intel.podm.client.resources.MembersListResource;
import com.intel.podm.client.resources.redfish.ManagerResource;
import com.intel.podm.client.resources.redfish.OemVendor;
import com.intel.podm.client.resources.redfish.ProcessorResource;
import com.intel.podm.common.logger.Logger;

import javax.annotation.PostConstruct;
import javax.ejb.Singleton;
import javax.ejb.Startup;
import javax.inject.Inject;
import java.util.function.Predicate;

import static com.intel.podm.client.typeidresolver.OdataTypeMatcher.odataTypePatternMatcher;
import static com.intel.podm.client.typeidresolver.OdataTypeMatcher.simpleOdataTypeMatcher;
import static com.intel.podm.client.typeidresolver.ResourceResolver.register;
import static java.lang.reflect.Modifier.isAbstract;
import static java.util.stream.StreamSupport.stream;
import static org.atteo.classindex.ClassIndex.getSubclasses;

@Singleton
@Startup
public class ResourceProvider {
    @Inject
    private Logger logger;

    @PostConstruct
    private void resourceProvider() {
        logger.d("Registering supported OData types...");
        registerKnownOdataTypes(ExternalServiceResource.class);
        registerKnownOdataTypes(OemVendor.class);
        registerKnownIncorrectOdataTypes();
    }

    private void registerKnownOdataTypes(Class<?> clazz) {
        stream(getSubclasses(clazz).spliterator(), false)
            .filter(byConcreteClass())
            .forEach(ResourceResolver::registerResource);
    }

    private Predicate<Class<?>> byConcreteClass() {
        return clazz -> !isAbstract(clazz.getModifiers());
    }

    private void registerKnownIncorrectOdataTypes() {
        register(odataTypePatternMatcher(".*Processor.+Processor", ProcessorResource.class));
        register(simpleOdataTypeMatcher("#Managers.1.0.0.Manager", ManagerResource.class));
        register(simpleOdataTypeMatcher("#Processors.1.0.0.ProcessorsCollection", MembersListResource.class));
    }
}
