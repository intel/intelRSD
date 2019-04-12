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

package com.intel.rsd.nodecomposer.externalservices.typeidresolver;

import com.intel.rsd.nodecomposer.externalservices.OdataTypes;
import com.intel.rsd.nodecomposer.externalservices.resources.ExternalServiceResource;
import lombok.extern.slf4j.Slf4j;
import org.springframework.boot.ApplicationArguments;
import org.springframework.boot.ApplicationRunner;
import org.springframework.context.annotation.Scope;
import org.springframework.core.Ordered;
import org.springframework.data.util.AnnotatedTypeScanner;
import org.springframework.stereotype.Component;

import static java.lang.reflect.Modifier.isAbstract;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_SINGLETON;

@Slf4j
@Component
@Scope(SCOPE_SINGLETON)
public class ResourceProvider implements ApplicationRunner, Ordered {
    private static final int ORDER = 1;

    @Override
    public void run(ApplicationArguments args) {
        log.debug("Registering supported OData types...");
        AnnotatedTypeScanner scanner = new AnnotatedTypeScanner(OdataTypes.class);
        String scannedPackageName = ExternalServiceResource.class.getPackage().getName();
        scanner.findTypes(scannedPackageName).stream()
            .filter(clazz -> !isAbstract(clazz.getModifiers()))
            .filter(ExternalServiceResource.class::isAssignableFrom)
            .forEach(ResourceResolver::registerResource);
    }

    @Override
    public int getOrder() {
        return ORDER;
    }
}
