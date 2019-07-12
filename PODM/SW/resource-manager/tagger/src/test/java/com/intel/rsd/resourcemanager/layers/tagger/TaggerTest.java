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

import com.fasterxml.jackson.core.JsonPointer;
import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.node.JsonNodeType;
import com.intel.rsd.resourcemanager.layers.Layer;
import com.intel.rsd.resourcemanager.layers.Response;
import com.intel.rsd.resourcemanager.layers.ServiceId;
import org.mockito.Mock;
import org.springframework.http.HttpHeaders;
import org.springframework.http.HttpMethod;
import org.springframework.http.ResponseEntity;
import org.testng.annotations.BeforeMethod;
import org.testng.annotations.Test;

import static com.fasterxml.jackson.databind.node.JsonNodeType.STRING;
import static com.intel.rsd.json.JsonUtils.stringToJsonNode;
import static com.intel.rsd.redfish.RedfishErrorCode.GeneralError;
import static java.util.Collections.emptyList;
import static java.util.Collections.singleton;
import static java.util.Collections.singletonList;
import static org.mockito.Matchers.any;
import static org.mockito.Matchers.anyMap;
import static org.mockito.Matchers.anyObject;
import static org.mockito.Matchers.argThat;
import static org.mockito.Matchers.eq;
import static org.mockito.Mockito.never;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.verifyZeroInteractions;
import static org.mockito.Mockito.when;
import static org.mockito.MockitoAnnotations.initMocks;
import static org.springframework.http.HttpMethod.GET;
import static org.springframework.http.HttpMethod.PATCH;
import static org.springframework.http.HttpStatus.BAD_REQUEST;
import static org.testng.Assert.assertEquals;
import static org.testng.Assert.assertTrue;

@SuppressWarnings({"unchecked"})
public class TaggerTest {
    private static final ServiceId SERVICE_ID = new ServiceId("someServiceId");

    @Mock
    private TagsRepository tagsRepository;

    @Mock
    private TagDefinitionsProvider tagDefinitionsProvider;

    @Mock
    private Layer nextLayer;

    private Tagger tagger;

    @BeforeMethod
    private void beforeMethod() {
        initMocks(this);
        this.tagger = new Tagger(tagsRepository, tagDefinitionsProvider);
        this.tagger.setNext(nextLayer);
    }

    @Test
    public void whenPatchRequestIsSentToResourceNotHandledByTagger_shouldNotInteractWithTagsRepository() {
        String testPath = "/path/not/handled/by/tagger";
        when(tagDefinitionsProvider.getTagDefinitionsForPath(testPath)).thenReturn(emptyList());

        tagger.invoke(SERVICE_ID, testPath, PATCH, new HttpHeaders(), stringToJsonNode("{\"HandledProperty\":\"SomeValue\"}"));
        verifyZeroInteractions(tagsRepository);
    }

    @Test
    public void whenPatchingOnlyPropertiesHandledByTagger_shouldEnsureThatResourceExistsOnTheNextLayer() {
        String testPath = "/path/handled/by/tagger";
        TagDefinition tagDefinition = createTagDefinition(testPath, JsonPointer.valueOf("/HandledProperty"), STRING);
        when(tagDefinitionsProvider.getTagDefinitionsForPath(testPath)).thenReturn(singletonList(tagDefinition));

        // mock HTTP 200 OK response from next layer - means that resource exists
        ResponseEntity<JsonNode> responseEntity = ResponseEntity.ok().build();
        when(nextLayer.getServices()).thenReturn(singleton(SERVICE_ID));
        when(nextLayer.invoke(eq(SERVICE_ID), any(String.class), any(HttpMethod.class), any(HttpHeaders.class), any(JsonNode.class), anyMap()))
            .thenReturn(new Response(responseEntity));

        JsonNode requestBody = stringToJsonNode("{\"HandledProperty\": \"SomeValue\"}");
        Response response = tagger.invoke(SERVICE_ID, testPath, PATCH, new HttpHeaders(), requestBody, anyMap());

        verify(nextLayer).invoke(eq(SERVICE_ID), eq(testPath), eq(GET), any(), any(), anyMap());
        assertTrue(response.getHttpStatus().is2xxSuccessful());
    }

    @Test
    public void whenPatchingBothPropertiesHandledAndNotHandledByTagger_shouldNotPassPropertiesHandledByTaggerToNextLayer() {
        String testPath = "/path/handled/by/tagger";
        TagDefinition tagDefinition = createTagDefinition(testPath, JsonPointer.valueOf("/HandledProperty"), STRING);
        when(tagDefinitionsProvider.getTagDefinitionsForPath(testPath)).thenReturn(singletonList(tagDefinition));

        // mock HTTP 204 No Content response from next layer - means successful PATCH on next layer
        ResponseEntity<JsonNode> responseEntity = ResponseEntity.noContent().build();
        when(nextLayer.getServices()).thenReturn(singleton(SERVICE_ID));
        when(nextLayer.invoke(eq(SERVICE_ID), any(String.class), any(HttpMethod.class), any(HttpHeaders.class), any(JsonNode.class), anyMap()))
            .thenReturn(new Response(responseEntity));

        JsonNode originalRequestBody = stringToJsonNode("{\"HandledProperty\": \"SomeValue\", \"NotHandledProperty\":\"SomeOtherValue\"}");
        JsonNode bodyAfterExtraction = stringToJsonNode("{\"NotHandledProperty\":\"SomeOtherValue\"}");

        tagger.invoke(SERVICE_ID, testPath, PATCH, new HttpHeaders(), originalRequestBody, anyMap());
        verify(nextLayer).invoke(eq(SERVICE_ID), eq(testPath), eq(PATCH), anyObject(), argThat(new JsonNodeMatcher(bodyAfterExtraction)), anyMap());
    }

    @Test
    public void whenPatchingBothPropertiesHandledAndNotHandledByTagger_shouldNotPersistPropertiesIfNextLayerResponseIsNotOk() {
        String testPath = "/path/handled/by/tagger";
        TagDefinition tagDefinition = createTagDefinition(testPath, JsonPointer.valueOf("/HandledProperty"), STRING);
        when(tagDefinitionsProvider.getTagDefinitionsForPath(testPath)).thenReturn(singletonList(tagDefinition));

        // mock HTTP 400 BadRequest response from next layer - means a PATCH on next layer failed
        ResponseEntity<JsonNode> responseEntity = ResponseEntity.badRequest().build();
        when(nextLayer.getServices()).thenReturn(singleton(SERVICE_ID));
        when(nextLayer.invoke(eq(SERVICE_ID), any(String.class), any(HttpMethod.class), any(HttpHeaders.class), any(JsonNode.class), anyMap()))
            .thenReturn(new Response(responseEntity));

        JsonNode requestBody = stringToJsonNode("{\"HandledProperty\": \"SomeValue\", \"NotHandledProperty\":\"SomeOtherValue\"}");
        Response response = tagger.invoke(SERVICE_ID, testPath, PATCH, new HttpHeaders(), requestBody, anyMap());

        assertEquals(response.getHttpStatus(), responseEntity.getStatusCode());
        verify(tagsRepository, never()).setTagValues(any(), any(), any());
    }

    @Test
    public void whenTagExtractionFromRequestBodyFails_shouldReturnBadRequestDetailedRedfishResponse() {
        String testPath = "/path/handled/by/tagger";
        TagDefinition tagDefinition = createTagDefinition(testPath, JsonPointer.valueOf("/HandledProperty"), STRING);
        when(tagDefinitionsProvider.getTagDefinitionsForPath(testPath)).thenReturn(singletonList(tagDefinition));

        JsonNode requestBody = stringToJsonNode("{\"HandledProperty\": {\"An OBJECT Encountered\": \"When STRING expected\"}}");
        Response response = tagger.invoke(SERVICE_ID, testPath, PATCH, new HttpHeaders(), requestBody);

        assertEquals(response.getHttpStatus(), BAD_REQUEST);
        String responseBodyAsString = response.getBody().toString();
        assertTrue(responseBodyAsString.contains(GeneralError.asString()));
        assertTrue(responseBodyAsString.contains("@Message.ExtendedInfo"));
        assertTrue(responseBodyAsString.contains("MessageId"));
        verify(tagsRepository, never()).setTagValues(any(), any(), any());
    }

    private TagDefinition createTagDefinition(String resource, JsonPointer property, JsonNodeType jsonNodeType) {
        TagDefinition tagDefinition = new TagDefinition();
        tagDefinition.setResource(resource);
        tagDefinition.setProperty(property);
        tagDefinition.setType(jsonNodeType);
        return tagDefinition;
    }
}
