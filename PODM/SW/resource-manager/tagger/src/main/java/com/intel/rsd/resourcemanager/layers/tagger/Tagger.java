/*
 * Copyright (c) 2018-2019 Intel Corporation
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

package com.intel.rsd.resourcemanager.layers.tagger;

import com.fasterxml.jackson.databind.JsonNode;
import com.intel.rsd.resourcemanager.layers.Layer;
import com.intel.rsd.resourcemanager.layers.Response;
import com.intel.rsd.resourcemanager.layers.ServiceId;
import lombok.val;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.HttpHeaders;
import org.springframework.http.HttpMethod;
import org.springframework.http.HttpStatus;
import org.springframework.stereotype.Component;
import org.springframework.validation.Errors;
import org.springframework.validation.ObjectError;

import java.util.List;
import java.util.Map;
import java.util.Objects;
import java.util.Set;

import static com.intel.rsd.json.JsonUtils.objectToJsonNode;
import static com.intel.rsd.redfish.RedfishErrors.createRedfishError;
import static com.intel.rsd.resourcemanager.layers.tagger.TagsOverlay.putTagsInJsonNode;
import static org.springframework.http.HttpMethod.GET;
import static org.springframework.http.HttpMethod.PATCH;
import static org.springframework.http.HttpStatus.BAD_REQUEST;
import static org.springframework.http.HttpStatus.NO_CONTENT;
import static org.springframework.http.HttpStatus.OK;

@Component
@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
public class Tagger extends Layer {
    private final TagsRepository tagsRepository;
    private final TagDefinitionsProvider tagDefinitionsProvider;

    @Autowired
    public Tagger(TagsRepository tagsRepository, TagDefinitionsProvider tagDefinitionsProvider) {
        this.tagsRepository = tagsRepository;
        this.tagDefinitionsProvider = tagDefinitionsProvider;
    }

    @Override
    public Set<ServiceId> getServices() {
        return getNextLayer().getServices();
    }

    @Override
    @SuppressWarnings({"checkstyle:CyclomaticComplexity", "checkstyle:NPathComplexity", "checkstyle:MethodLength"})
    protected Response invokeImpl(ServiceId serviceId, String path, HttpMethod method, HttpHeaders headers, JsonNode body,
                                  Map<String, String> requestParams) {
        List<TagDefinition> tagDefinitionsForPath = tagDefinitionsProvider.getTagDefinitionsForPath(path);

        if (!tagDefinitionsForPath.isEmpty()) {
            Errors errors = new PatchRequestBodyBindingResult(body, "body");
            TagBundle extractedTags = new TagsExtractor(tagDefinitionsForPath).extractFrom(body, errors);
            if (errors.hasErrors()) {
                return new Response(getRedfishErrorResponseBody(BAD_REQUEST, errors), new HttpHeaders(), BAD_REQUEST);
            }

            if (Objects.equals(method, PATCH) && !extractedTags.isEmpty()) {
                Response nextLayerResponse;
                // if nothing else was about to be PATCH'ed, just check if the resource exists
                if (!body.fieldNames().hasNext()) {
                    nextLayerResponse = passToNextLayer(serviceId, path, GET, headers, null, requestParams);
                } else {
                    nextLayerResponse = passToNextLayer(serviceId, path, method, headers, body, requestParams);
                }

                // store tags only on success from lower layers
                if (nextLayerResponse.getHttpStatus().is2xxSuccessful()) {
                    tagsRepository.setTagValues(serviceId, path, extractedTags);
                    return new Response(null, new HttpHeaders(), NO_CONTENT);
                } else {
                    return nextLayerResponse;
                }
            }
        }

        Response response = passToNextLayer(serviceId, path, method, headers, body, requestParams);
        if (shouldAddTagsToResponse(response, tagDefinitionsForPath)) {
            // properties handled by tagger are not expected to exist in response and will be overwritten if present
            putTagsInJsonNode(response.getBody(), tagDefinitionsForPath, tagsRepository.getTagValues(serviceId, path));
        }
        return response;
    }

    private JsonNode getRedfishErrorResponseBody(HttpStatus responseStatus, Errors errors) {
        val extendedInfo = errors.getAllErrors().stream()
            .map(ObjectError::getDefaultMessage)
            .filter(Objects::nonNull)
            .toArray(String[]::new);

        return objectToJsonNode(createRedfishError(responseStatus, extendedInfo));
    }

    private boolean shouldAddTagsToResponse(Response response, List<TagDefinition> tagDefinitionsForPath) {
        return !tagDefinitionsForPath.isEmpty() && Objects.equals(OK, response.getHttpStatus());
    }
}
