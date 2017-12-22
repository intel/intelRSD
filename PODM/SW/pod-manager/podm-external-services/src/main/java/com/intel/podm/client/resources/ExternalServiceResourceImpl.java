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

package com.intel.podm.client.resources;

import com.fasterxml.jackson.annotation.JsonAnySetter;
import com.fasterxml.jackson.annotation.JsonIgnore;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonTypeInfo;
import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.annotation.JsonTypeIdResolver;
import com.fasterxml.jackson.databind.node.ObjectNode;
import com.intel.podm.client.WebClient;
import com.intel.podm.client.WebClientRequestException;
import com.intel.podm.client.reader.ResourceLinks;
import com.intel.podm.client.reader.ResourceSupplier;
import com.intel.podm.client.redfish.response.RedfishEntityResponseBodyImpl;
import com.intel.podm.client.resources.redfish.UnknownOemObject;
import com.intel.podm.client.typeidresolver.ResourceResolver;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.types.redfish.OemType;
import org.atteo.classindex.IndexSubclasses;

import java.net.URI;
import java.util.ArrayList;
import java.util.LinkedList;
import java.util.List;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import java.util.stream.StreamSupport;

import static com.fasterxml.jackson.annotation.JsonTypeInfo.As.PROPERTY;
import static com.fasterxml.jackson.annotation.JsonTypeInfo.Id.NAME;
import static com.intel.podm.client.resources.UnknownOemsHelper.createUnknownOemObject;
import static com.intel.podm.client.resources.UnknownOemsHelper.extractUnknownOemsFromActionsOemObject;
import static com.intel.podm.client.resources.UnknownOemsHelper.extractUnknownOemsFromLinksOemObject;
import static com.intel.podm.client.resources.UnknownOemsHelper.extractUnknownOemsFromOemObject;
import static com.intel.podm.common.logger.LoggerFactory.getLogger;
import static com.intel.podm.common.utils.Contracts.requiresNonNull;
import static java.lang.String.format;
import static java.util.Arrays.asList;
import static java.util.Arrays.stream;
import static java.util.Collections.emptyList;
import static java.util.stream.Collectors.joining;
import static java.util.stream.Collectors.toList;
import static java.util.stream.Collectors.toSet;

@JsonTypeInfo(
    use = NAME,
    include = PROPERTY,
    property = "@odata.type",
    visible = true
)
@JsonTypeIdResolver(ResourceResolver.class)
@SuppressWarnings({"checkstyle:MethodCount", "checkstyle:ClassFanOutComplexity"})
public abstract class ExternalServiceResourceImpl extends RedfishEntityResponseBodyImpl implements ExternalServiceResource {
    private static final Logger LOGGER = getLogger(ExternalServiceResourceImpl.class);

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

    @JsonIgnore
    private List<UnknownOemObject> unknownOems = new ArrayList<>();

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

    public void addUnknownOem(UnknownOemObject unknownOemObject) {
        unknownOems.add(unknownOemObject);
    }

    @Override
    public List<UnknownOemObject> getUnknownOems() {
        return unknownOems;
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

    protected <T extends ExternalServiceResource> Iterable<ResourceSupplier> toSuppliersFromResources(Iterable<T> resourcesCollection) {
        return StreamSupport.stream(requiresNonNull(resourcesCollection, "resourcesCollection").spliterator(), false)
            .map(item -> new ResourceSupplier() {
                @Override
                public ExternalServiceResource get() throws WebClientRequestException {
                    item.setWebClient(webClient);
                    return item;
                }

                @Override
                public URI getUri() {
                    return item.getUri();
                }
            }).collect(toList());
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

    //FIXME: delete when links to Computer system are available in child resources
    protected ODataId getComputerSystemODataId() {
        if (!oDataId.contains("Systems")) {
            return null;
        }

        String[] uriChunks = oDataId.split("/");
        LinkedList<String> linkedList = new LinkedList<>(asList(uriChunks));
        linkedList.removeLast();
        linkedList.removeLast();

        String computerSystemUri = linkedList.stream().collect(joining("/"));
        return new ODataId(computerSystemUri);
    }

    protected Matcher createMatcher(String uriFragement, String... uriFragmentGroups) {
        Pattern pattern = Pattern.compile(
            stream(uriFragmentGroups)
                .map(groupName -> format("(?<%s>\\w+)", groupName))
                .collect(joining("/", "/?", ""))
        );

        return pattern.matcher(uriFragement);
    }

    @JsonAnySetter
    private void addUnknownOemFromExistingOemObject(String oemName, JsonNode oemValue) throws JsonProcessingException {
        if (oemValue == null) {
            return;
        }

        switch (oemName) {
            case "Oem":
                extractUnknownOemsFromOemObject(oemName, oemValue).forEach(ExternalServiceResourceImpl.this::addUnknownOem);
                break;
            case "Links":
                extractUnknownOemsFromLinksOemObject("Oem", oemValue.findValue("Oem")).forEach(ExternalServiceResourceImpl.this::addUnknownOem);
                break;
            case "Actions":
                extractUnknownOemsFromActionsOemObject("Oem", oemValue.findValue("Oem")).forEach(ExternalServiceResourceImpl.this::addUnknownOem);
                break;
            default:
                break;
        }
    }

    @IndexSubclasses
    public abstract class RedfishOem {
        @JsonAnySetter
        @SuppressWarnings({"checkstyle:ReturnCount"})
        private void addUnknownOemFromOemObject(String oemName, ObjectNode oemValue) throws JsonProcessingException {
            if (handleKnownVendor(oemName, oemValue)) {
                return;
            }

            OemType oemType = getClass().getAnnotation(OemType.class);
            if (oemType == null) {
                LOGGER.e("Mandatory OemType annotation is not defined for: " + getClass().getName());
                return;
            }

            createUnknownOemObject(oemType.value().getPath(), oemName, oemValue).ifPresent(ExternalServiceResourceImpl.this::addUnknownOem);
        }

        public boolean handleKnownVendor(String oemName, ObjectNode oemValue) throws JsonProcessingException {
            return false;
        }
    }

    public abstract class RedfishLinks {
        @JsonAnySetter
        private void addUnknownOemFromLinksOemObject(String oemName, JsonNode oemValue) throws JsonProcessingException {
            extractUnknownOemsFromLinksOemObject(oemName, oemValue)
                .forEach(ExternalServiceResourceImpl.this::addUnknownOem);
        }
    }

    public abstract class RedfishActions {
        @JsonAnySetter
        private void addUnknownOemFromActionsOemObject(String oemName, JsonNode oemValue) throws JsonProcessingException {
            extractUnknownOemsFromActionsOemObject(oemName, oemValue)
                .forEach(ExternalServiceResourceImpl.this::addUnknownOem);
        }
    }
}
