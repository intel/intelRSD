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

package com.intel.podm.discovery.external.restgraph;

import com.intel.podm.client.api.ExternalServiceApiReaderException;
import com.intel.podm.client.api.reader.ResourceLinks;
import com.intel.podm.client.api.reader.ResourceSupplier;
import com.intel.podm.client.api.resources.ExternalServiceResource;
import com.intel.podm.common.logger.Logger;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.util.ArrayList;

import static java.util.Collections.emptyList;


@Dependent
public class ResourceLinkExtractor {
    @Inject
    private Logger logger;

    public Iterable<ResourceLink> extractFrom(ExternalServiceResource resource) {

        ArrayList<ResourceLink> result = new ArrayList<>();

        ResourceLinks links = (ResourceLinks) resource.getLinks();
        for (String name : links.getNames()) {
            Iterable<ExternalServiceResource> linkedResources = getResources(links, name);

            for (ExternalServiceResource linked : linkedResources) {
                result.add(new ResourceLink(resource, linked, name));
            }
        }

        return result;
    }

    private Iterable<ExternalServiceResource> getResources(ResourceLinks links, String name) {
        ArrayList<ExternalServiceResource> result = new ArrayList<>();

        for (ResourceSupplier supplier : getSuppliers(links, name)) {
            try {
                result.add(supplier.get());
            } catch (ExternalServiceApiReaderException e) {
                logger.e("Problem while reading: {}, error: {}", e.getResourceUri(), e.getErrorResponse(), e);
            }
        }

        return result;
    }

    private Iterable<ResourceSupplier> getSuppliers(ResourceLinks links, String name) {
        try {
            return links.get(name);
        } catch (ExternalServiceApiReaderException e) {
            logger.e("Problem while reading: {}, error: {}", e.getResourceUri(), e.getErrorResponse(), e);
            return emptyList();
        }
    }
}
