/*
 * Copyright (c) 2015 Intel Corporation
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

package com.intel.podm.business.entities;

import com.intel.podm.business.entities.base.AbstractDomainObject;
import com.intel.podm.business.entities.base.DomainObject;
import com.intel.podm.business.entities.base.DomainObjectLink;
import com.intel.podm.business.entities.ogm.OgmManager;
import com.intel.podm.common.logger.Logger;

import javax.annotation.PostConstruct;
import javax.ejb.DependsOn;
import javax.ejb.Singleton;
import javax.ejb.Startup;
import javax.ejb.TransactionAttribute;
import javax.ejb.TransactionAttributeType;
import javax.enterprise.inject.Instance;
import javax.inject.Inject;

import static com.intel.podm.common.enterprise.utils.proxy.Unproxier.unproxy;
import static java.util.Arrays.stream;

@Singleton
@Startup
@DependsOn("ResourceProvider")
@TransactionAttribute(TransactionAttributeType.NOT_SUPPORTED)
public class DomainObjectModelStartup {
    @Inject
    private OgmManager ogmManager;

    @Inject
    private Instance<DomainObject> domainObjects;

    @Inject
    private Logger logger;

    @PostConstruct
    public void initializeDomainObjectModel() {
        logger.d("Initializing Domain Objects...");
        initializeDomainObjects();
        logger.d("Initializing Domain Object Links...");
        initializeDomainObjectLinks();
    }

    private void initializeDomainObjects() {
        domainObjects.forEach(domainObject -> {
            if (!domainObject.getClass().isAnnotationPresent(AbstractDomainObject.class)) {
                Class<DomainObject> domainObjectClass = unproxy(domainObject.getClass());
                logger.d("Creating schema for " + domainObjectClass);
                ogmManager.addToMapping(domainObjectClass);
            }
        });
    }

    private void initializeDomainObjectLinks() {
        stream(DomainObjectLink.values())
                .map(DomainObjectLink::getLabel)
                .distinct().sorted()
                .peek(linkName -> logger.d("Creating edge " + linkName))
                .forEach(ogmManager::addLinkDefinition);
    }
}
