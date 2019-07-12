/*
 * Copyright (c) 2015-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.discovery.external.restgraph;

import com.intel.rsd.nodecomposer.externalservices.WebClientRequestException;
import com.intel.rsd.nodecomposer.externalservices.reader.ResourceLinks;
import com.intel.rsd.nodecomposer.externalservices.reader.ResourceSupplier;
import com.intel.rsd.nodecomposer.externalservices.resources.ExternalServiceResource;
import lombok.extern.slf4j.Slf4j;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import java.util.HashSet;
import java.util.Set;

import static com.intel.rsd.nodecomposer.externalservices.WebClientExceptionUtils.isConnectionExceptionTheRootCause;
import static java.util.Collections.emptyList;
import static java.util.Collections.emptySet;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_PROTOTYPE;

@Slf4j
@Component
@Scope(SCOPE_PROTOTYPE)
public class ResourceLinkExtractor {
    public Set<ResourceLink> extractFrom(ExternalServiceResource resource) throws WebClientRequestException {
        ResourceLinks links = resource.getLinks();
        if (links == null) {
            return emptySet();
        }
        Set<ResourceLink> result = new HashSet<>();
        for (String linkName : links.getLinkNames()) {
            Iterable<ExternalServiceResource> linkedResources = getResources(links, linkName);

            for (ExternalServiceResource linked : linkedResources) {
                result.add(new ResourceLink(resource, linked, linkName));
            }
        }
        return result;
    }

    private Set<ExternalServiceResource> getResources(ResourceLinks links, String linkName) throws WebClientRequestException {
        Set<ExternalServiceResource> result = new HashSet<>();

        for (ResourceSupplier supplier : getSuppliers(links, linkName)) {
            try {
                result.add(supplier.get());
            } catch (WebClientRequestException e) {
                rethrowIfCausedByConnectionException(e);
            }
        }

        return result;
    }

    private Iterable<ResourceSupplier> getSuppliers(ResourceLinks links, String linkName) throws WebClientRequestException {
        try {
            return links.get(linkName);
        } catch (WebClientRequestException e) {
            rethrowIfCausedByConnectionException(e);
            return emptyList();
        }
    }

    private void rethrowIfCausedByConnectionException(WebClientRequestException e) throws WebClientRequestException {
        if (isConnectionExceptionTheRootCause(e)) {
            throw e;
        }

        log.error("Problem while reading: {}, {}: {}", e.getResourceUri(), e.getMessage(), e.getCause().getMessage());
    }
}
