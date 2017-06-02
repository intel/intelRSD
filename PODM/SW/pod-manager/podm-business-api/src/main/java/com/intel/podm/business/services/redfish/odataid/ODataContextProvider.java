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

package com.intel.podm.business.services.redfish.odataid;

import java.net.URI;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.StringTokenizer;

import static java.lang.String.format;
import static java.util.stream.Collectors.joining;

@SuppressWarnings({"checkstyle:MethodLength"})
public final class ODataContextProvider {
    private static final int EXPECTED_PARAMS_IN_EVENT_SERVICE_ODATA_ID = 2;
    private static final int EXPECTED_PARAMS_TOP_LEVEL_RESOURCE = 1;
    private static final String ROOT_SERVICE_CONTEXT = "/redfish/v1/$metadata#ServiceRoot";
    private static final String SERVICE_PREFIX = "/redfish/v1";
    private static final String METADATA_PREFIX = "/redfish/v1/$metadata#";
    private static final String ENTITY_SUFFIX = "$entity";
    private static final String COLLECTION_MEMBER = "Members";
    private static final Map<String, String> WORDS_TO_CHANGE = new HashMap<String, String>() {
        {
            put("Targets", "RemoteTargets");
        }
    };


    private ODataContextProvider() {
    }

    public static URI getContextFromId(ODataId oDataId) {

        String oDataIdString = oDataId.toString();

        if (!oDataIdString.contains(SERVICE_PREFIX)) {
            throw new IllegalStateException(format("No mapping for ODataId outside /redfish/v1 namespace: %s", oDataId));
        }

        String resourceId = oDataIdString.replace(SERVICE_PREFIX, "");

        if (resourceId.isEmpty()) {
            return URI.create(ROOT_SERVICE_CONTEXT);
        }

        String resourceMetadata;
        if (resourceId.startsWith("/EventService/")) {
            resourceMetadata = eventServiceOdataIdToContextConverter(resourceId);
        } else {
            resourceMetadata = defaultOdataIdToContextConverter(resourceId);
        }

        String context = METADATA_PREFIX + resourceMetadata;
        return URI.create(context);
    }

    /**
        ODataId is made as pairs: /{resource_name}/{resource_id}
        We can treat such pair as member of resource collection so in metadata it will look like:
        /{resource_name}/Members/{resource_id}

        Last pair can be treated in two different ways:
        1) if last pair is only one-element (ex. /{resource_name}) we treat is AS IS as it is collection identifier
        2) if last pair is full (ex. /{resource_name}/{resource_id}) we change id to metadata -> /Members/$entity
     */
    //TODO: refactor me
    private static String defaultOdataIdToContextConverter(String resourceId) {
        // Dividing entry parameter by slash
        StringTokenizer tokenizer = new StringTokenizer(resourceId, "/");
        List<String> contextElements = new ArrayList<>();

        if (tokenizer.countTokens() == EXPECTED_PARAMS_TOP_LEVEL_RESOURCE) {
            contextElements.add(tokenizer.nextToken());
        }

        while (tokenizer.hasMoreElements()) {
            String resourceName = tokenizer.nextToken();
            resourceName = WORDS_TO_CHANGE.getOrDefault(resourceName, resourceName);

            contextElements.add(resourceName);

            if (tokenizer.hasMoreElements()) {
                contextElements.add(COLLECTION_MEMBER);

                String assetId = tokenizer.nextToken();
                if (tokenizer.hasMoreElements()) {
                    contextElements.add(assetId);
                } else {
                    contextElements.add(ENTITY_SUFFIX);
                }
            } else {
                contextElements.add(ENTITY_SUFFIX);
            }
        }

        return contextElements.stream().collect(joining("/"));
    }

    private static String eventServiceOdataIdToContextConverter(String resourceId) {
        List<String> contextElements = new ArrayList<>();
        StringTokenizer tokenizer = new StringTokenizer(resourceId, "/");

        for (int i = 0; i < EXPECTED_PARAMS_IN_EVENT_SERVICE_ODATA_ID; i++) {
            contextElements.add(tokenizer.nextToken());
        }

        if (tokenizer.hasMoreElements()) {
            contextElements.add(COLLECTION_MEMBER);
        }

        contextElements.add(ENTITY_SUFFIX);

        return contextElements.stream().collect(joining("/"));
    }

}
