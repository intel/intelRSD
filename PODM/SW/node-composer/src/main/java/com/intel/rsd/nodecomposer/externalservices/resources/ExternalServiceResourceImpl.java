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

package com.intel.rsd.nodecomposer.externalservices.resources;

import com.fasterxml.jackson.annotation.JsonIgnore;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonTypeInfo;
import com.fasterxml.jackson.databind.annotation.JsonTypeIdResolver;
import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.externalservices.WebClient;
import com.intel.rsd.nodecomposer.externalservices.WebClientRequestException;
import com.intel.rsd.nodecomposer.externalservices.reader.ResourceLinks;
import com.intel.rsd.nodecomposer.externalservices.reader.ResourceSupplier;
import com.intel.rsd.nodecomposer.externalservices.redfish.response.RedfishEntityResponseBodyImpl;
import com.intel.rsd.nodecomposer.externalservices.typeidresolver.ResourceResolver;
import lombok.extern.slf4j.Slf4j;

import java.net.URI;
import java.util.List;
import java.util.stream.StreamSupport;

import static com.fasterxml.jackson.annotation.JsonTypeInfo.As.PROPERTY;
import static com.fasterxml.jackson.annotation.JsonTypeInfo.Id.NAME;
import static java.util.Collections.emptyList;
import static java.util.stream.Collectors.toList;
import static java.util.stream.Collectors.toSet;

@Slf4j
@JsonTypeInfo(
    use = NAME,
    include = PROPERTY,
    property = "@odata.type",
    visible = true
)
@JsonTypeIdResolver(ResourceResolver.class)
public abstract class ExternalServiceResourceImpl extends RedfishEntityResponseBodyImpl implements ExternalServiceResource {
    @JsonProperty("Id")
    private String id;
    @JsonProperty("Name")
    private String name;
    @JsonProperty("Description")
    private String description;

    @JsonIgnore
    private WebClient webClient;

    @JsonIgnore
    private URI uri;

    @Override
    public URI getUri() {
        return uri;
    }

    @Override
    public void setUri(URI uri) {
        this.uri = uri;
    }

    public String getId() {
        return id;
    }

    public String getName() {
        return name;
    }

    public String getDescription() {
        return description;
    }

    @Override
    public void setWebClient(WebClient webClient) {
        this.webClient = webClient;
    }

    @Override
    public ResourceLinks getLinks() {
        return new ResourceLinks(this);
    }

    protected Iterable<ResourceSupplier> processMembersListResource(ODataId membersListResource) throws WebClientRequestException {
        if (membersListResource == null || membersListResource.toUri() == null) {
            return emptyList();
        }

        Iterable<ODataId> members = ((MembersListResource) toSupplier(membersListResource).get())
            .getMembers();
        return toSuppliers(members);
    }

    protected Iterable<ResourceSupplier> toSuppliers(Iterable<ODataId> collection) throws WebClientRequestException {
        if (collection == null) {
            return emptyList();
        }

        return StreamSupport.stream(collection.spliterator(), false)
            .map(this::toSupplier)
            .collect(toList());
    }

    protected ResourceSupplier toSupplier(ODataId oDataId) {
        return toSupplier(webClient, oDataId);
    }

    protected <T extends ExternalServiceResource> Iterable<ResourceSupplier> toSuppliersFromEmbeddableResourceElement(
        List<T> resourcesCollection, String embeddableUriSegmentName) {
        return resourcesCollection.stream()
            .map(resource -> createResourceSupplier(resource, resourcesCollection.indexOf(resource), embeddableUriSegmentName))
            .collect(toSet());
    }

    private <T extends ExternalServiceResource> ResourceSupplier createResourceSupplier(T item, int resourceId, String embeddableUriSegmentName) {
        return new ResourceSupplier() {
            String uriString = uri.getPath() + "#/" + embeddableUriSegmentName + "/" + resourceId;

            @Override
            public ExternalServiceResource get() throws WebClientRequestException {
                item.setWebClient(webClient);
                item.setUri(URI.create(uriString));
                return item;
            }

            @Override
            public URI getUri() {
                return URI.create(uriString);
            }
        };
    }
}
