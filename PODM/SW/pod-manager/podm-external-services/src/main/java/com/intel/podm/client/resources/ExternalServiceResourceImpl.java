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

package com.intel.podm.client.resources;

import com.fasterxml.jackson.annotation.JsonIgnoreProperties;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonTypeInfo;
import com.fasterxml.jackson.databind.annotation.JsonTypeIdResolver;
import com.intel.podm.client.api.ExternalServiceApiReaderException;
import com.intel.podm.client.api.WebClient;
import com.intel.podm.client.api.reader.ResourceLinks;
import com.intel.podm.client.api.reader.ResourceSupplier;
import com.intel.podm.client.api.resources.ExternalServiceResource;
import com.intel.podm.client.reader.ResourceLinksImpl;
import com.intel.podm.client.reader.ResourceSupplierImpl;
import com.intel.podm.client.typeidresolver.ResourceResolver;

import java.net.URI;
import java.util.LinkedList;

import static com.fasterxml.jackson.annotation.JsonTypeInfo.As.PROPERTY;
import static com.fasterxml.jackson.annotation.JsonTypeInfo.Id.NAME;
import static com.google.common.collect.Lists.newLinkedList;
import static java.util.Arrays.asList;
import static java.util.Collections.emptyList;
import static java.util.stream.Collectors.joining;
import static java.util.stream.Collectors.toList;
import static java.util.stream.StreamSupport.stream;

@JsonTypeInfo(
        use = NAME,
        include = PROPERTY,
        property = "@odata.type",
        visible = true
)
@JsonTypeIdResolver(ResourceResolver.class)
@JsonIgnoreProperties(ignoreUnknown = true)
public abstract class ExternalServiceResourceImpl implements ExternalServiceResource {
    protected URI uri;
    protected WebClient webClient;
    @JsonProperty("Id")
    private String id;
    @JsonProperty("Name")
    private String name;
    @JsonProperty("Description")
    private String description;

    //FIXME: delete when links to Computer system are available in child resources
    @JsonProperty("@odata.id")
    private String oDataId;

    @Override
    public void setUri(URI uri) {
        this.uri = uri;
    }

    @Override
    public URI getUri() {
        return uri;
    }

    @Override
    public String getId() {
        return id;
    }

    @Override
    public String getName() {
        return name;
    }

    @Override
    public String getDescription() {
        return description;
    }

    @Override
    public void setWebClient(WebClient webClient) {
        this.webClient = webClient;
    }

    @Override
    public ResourceLinks getLinks() {
        return new ResourceLinksImpl(this);
    }

    protected Iterable<ResourceSupplier> processMembersListResource(ODataId membersListResource) throws ExternalServiceApiReaderException {
        if (membersListResource == null || membersListResource.toUri() == null) {
            return emptyList();
        }

        Iterable<ODataId> members = ((MembersListResource) toSupplier(membersListResource).get())
                .getMembers();
        return toSuppliers(members);
    }

    protected Iterable<ResourceSupplier> toSuppliers(Iterable<ODataId> collection) throws ExternalServiceApiReaderException {
        if (collection == null) {
            return emptyList();
        }

        return stream(collection.spliterator(), false)
                .map(this::toSupplier)
                .collect(toList());
    }

    protected ResourceSupplier toSupplier(ODataId oDataId) {
        if (oDataId == null || oDataId.toUri() == null) {
            return new ResourceSupplier() {
                @Override
                public ExternalServiceResource get() throws ExternalServiceApiReaderException {
                    throw new ExternalServiceApiReaderException("null ODataId", ExternalServiceResourceImpl.this.getUri());
                }

                @Override
                public URI getUri() {
                    throw new IllegalStateException("URI not available");
                }
            };
        }

        return new ResourceSupplierImpl(webClient, oDataId.toUri());
    }

    //FIXME: delete when links to Computer system are available in child resources
    protected ODataId getComputerSystemODataId() {
        if (!oDataId.contains("Systems")) {
            return null;
        }

        String[] uriChunks = oDataId.split("/");
        LinkedList<String> linkedList = newLinkedList(asList(uriChunks));
        linkedList.removeLast();
        linkedList.removeLast();

        String computerSystemUri = linkedList.stream().collect(joining("/"));
        return new ODataId(computerSystemUri);
    }
}
