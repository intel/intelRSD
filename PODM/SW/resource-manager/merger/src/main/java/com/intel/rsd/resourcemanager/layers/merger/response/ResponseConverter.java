/*
 * Copyright (c) 2017-2019 Intel Corporation
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

package com.intel.rsd.resourcemanager.layers.merger.response;

import com.fasterxml.jackson.databind.JsonNode;
import com.intel.rsd.collections.Lists;
import com.intel.rsd.resourcemanager.layers.Response;
import com.intel.rsd.resourcemanager.layers.merger.ResourceMerger;
import lombok.NonNull;
import lombok.extern.slf4j.Slf4j;
import lombok.val;
import org.jetbrains.annotations.NotNull;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.HttpHeaders;
import org.springframework.http.HttpStatus;
import org.springframework.stereotype.Component;

import java.util.Collection;
import java.util.Comparator;
import java.util.Map;
import java.util.Objects;
import java.util.Optional;

import static com.intel.rsd.redfish.RedfishErrors.createRedfishError;
import static com.intel.rsd.redfish.RedfishErrors.createRedfishErrorWithCustomMessage;
import static com.intel.rsd.resourcemanager.layers.merger.response.ErrorResponseFactory.createErrorResponse;
import static java.util.Arrays.asList;
import static java.util.Optional.empty;
import static java.util.Optional.of;
import static java.util.stream.Collectors.toList;
import static java.util.stream.Collectors.toMap;
import static org.springframework.http.HttpStatus.INTERNAL_SERVER_ERROR;
import static org.springframework.http.HttpStatus.NOT_FOUND;

@Component
@Slf4j
public class ResponseConverter {

    private final ResourceMerger resourceMerger;

    @Autowired
    public ResponseConverter(ResourceMerger resourceMerger) {
        this.resourceMerger = resourceMerger;
    }

    public Response merge(@NonNull Response... responses) {
        if (responses.length == 0) {
            return createErrorResponse(createRedfishError(NOT_FOUND));
        }

        if (responses.length == 1) {
            return responses[0];
        }

        return validateResponses(asList(responses))
            .orElseGet(() -> new Response(mergeBody(asList(responses)), mergeHeaders(asList(responses)), mergeStatus(asList(responses))));
    }

    private Optional<Response> validateResponses(Collection<Response> allResponses) {
        if (allResponses.stream().anyMatch(r -> !r.getHttpStatus().is2xxSuccessful())) {
            val errorMessage = "At least one underlying service has responded with non 2xx status: ";
            if (allResponses.stream().map(Response::getHttpStatus).distinct().count() == 1) {
                return getResponse(allResponses.iterator().next().getHttpStatus(), errorMessage, allResponses);
            }
            return getResponse(INTERNAL_SERVER_ERROR, errorMessage, allResponses);
        }

        if (allResponses.stream().map(Response::getHttpStatus).distinct().count() != 1) {
            val errorMessage = "Affected underlying services have responded with different 2xx status codes";
            return getResponse(INTERNAL_SERVER_ERROR, errorMessage, allResponses);
        }
        return empty();
    }

    @NotNull
    private Optional<Response> getResponse(HttpStatus errorStatus, String errorMessage, Collection<Response> allResponses) {
        val redfishError = createRedfishErrorWithCustomMessage(
            errorStatus,
            errorMessage,
            allResponses.stream().map(this::getSummaryOfResponse).toArray(String[]::new)
        );
        return of(createErrorResponse(redfishError));
    }

    private String getSummaryOfResponse(Response response) {
        return "HttpStatus: " + response.getHttpStatus() + ", body: " + (response.getBody() != null ? response.getBody().asText() : null);
    }

    private JsonNode mergeBody(Collection<Response> allResponses) {
        return resourceMerger.merge(allResponses.stream().map(Response::getBody).filter(Objects::nonNull).collect(toList()));
    }

    private HttpStatus mergeStatus(Collection<Response> allResponses) {
        return allResponses.stream().findFirst().map(Response::getHttpStatus)
            .orElseThrow(() -> new IllegalArgumentException("At least one response is required to determine response status."));
    }

    private HttpHeaders mergeHeaders(Collection<Response> allResponses) {
        HttpHeaders headers = allResponses.stream().map(Response::getHttpHeaders)
            .flatMap(httpHeaders -> httpHeaders.entrySet().stream())
            .collect(toMap(Map.Entry::getKey, Map.Entry::getValue, Lists::concat, HttpHeaders::new));

        return mergeAgeHeader(headers);
    }

    private HttpHeaders mergeAgeHeader(HttpHeaders headers) {
        if (headers.containsKey("Age")) {
            Optional<String> maxAge = headers.get("Age").stream().max(Comparator.comparingLong(Long::parseLong));
            maxAge.ifPresent(s -> headers.set("Age", s));
        }
        return headers;
    }
}
