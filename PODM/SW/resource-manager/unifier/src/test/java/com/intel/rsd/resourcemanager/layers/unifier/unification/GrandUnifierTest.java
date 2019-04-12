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

package com.intel.rsd.resourcemanager.layers.unifier.unification;

import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.node.TextNode;
import com.google.common.util.concurrent.UncheckedExecutionException;
import com.intel.rsd.resourcemanager.layers.Response;
import org.springframework.http.HttpHeaders;
import org.springframework.http.ResponseEntity;
import org.testng.annotations.Test;

import static com.google.common.collect.ImmutableSet.of;
import static java.util.Collections.emptyList;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;
import static org.springframework.http.HttpStatus.OK;
import static org.testng.Assert.assertEquals;

public class GrandUnifierTest {

    private static final Unifier<JsonNode> PASS_THROUGH_JSON_NODE_UNIFIER = new Unifier<JsonNode>() {
        @Override
        public JsonNode unify(JsonNode toBeUnified, UnifiedOdataIdResolver resolver) {
            return toBeUnified;
        }

        @Override
        public Class<JsonNode> getUnifiableClass() {
            return JsonNode.class;
        }
    };

    private static final Unifier<HttpHeaders> PASS_THROUGH_HTTP_HEADERS_UNIFIER = new Unifier<HttpHeaders>() {
        @Override
        public HttpHeaders unify(HttpHeaders toBeUnified, UnifiedOdataIdResolver resolver) {
            return toBeUnified;
        }

        @Override
        public Class<HttpHeaders> getUnifiableClass() {
            return HttpHeaders.class;
        }
    };

    private static final Deunifier<HttpHeaders> PASS_THROUGH_HTTP_HEADERS_DEUNIFIER = new Deunifier<HttpHeaders>() {

        @Override
        public HttpHeaders deunify(HttpHeaders toBeDeunified, UnifiedOdataIdResolver resolver) {
            return toBeDeunified;
        }

        @Override
        public Class<HttpHeaders> getDeunifiableClass() {
            return HttpHeaders.class;
        }
    };

    @Test(expectedExceptions = NullPointerException.class)
    public void whenInputResponseDoesntContainHttpHeaders_exceptionShouldBeThrown() throws Exception {
        GrandUnifier sut = new GrandUnifier(emptyList(), emptyList());
        sut.unify(mock(Response.class), mock(UnifiedOdataIdResolver.class));
    }

    @Test(expectedExceptions = UncheckedExecutionException.class)
    public void whenInputContainsHttpHeadersButRelatedUnifierIsNotRegistered_exceptionShouldBeThrown() throws Exception {
        GrandUnifier sut = new GrandUnifier(emptyList(), emptyList());
        Response response = mock(Response.class);
        when(response.getHttpHeaders()).thenReturn(new HttpHeaders());
        sut.unify(response, mock(UnifiedOdataIdResolver.class));
    }

    @Test(expectedExceptions = NullPointerException.class)
    public void whenInputContainsHttpHeadersAndRelatedUnifierIsRegisteredButInputDoesntContainHttpStatus_exceptionShouldBeThrown() throws Exception {
        GrandUnifier sut = new GrandUnifier(of(PASS_THROUGH_HTTP_HEADERS_UNIFIER), emptyList());
        Response response = mock(Response.class);
        when(response.getHttpHeaders()).thenReturn(new HttpHeaders());
        sut.unify(response, mock(UnifiedOdataIdResolver.class));
    }

    @Test
    public void whenInputContainsHttpStatusAndHeadersAndRelatedUnifiersAreRegistered() throws Exception {
        GrandUnifier sut = new GrandUnifier(of(PASS_THROUGH_HTTP_HEADERS_UNIFIER), emptyList());
        Response response = mock(Response.class);
        when(response.getHttpHeaders()).thenReturn(new HttpHeaders());
        when(response.getHttpStatus()).thenReturn(OK);
        Response result = sut.unify(response, mock(UnifiedOdataIdResolver.class));
        assertEquals(result.getHttpHeaders(), response.getHttpHeaders());
        assertEquals(result.getHttpStatus(), response.getHttpStatus());
    }

    @Test
    public void whenInputContainsBodyHttpStatusAndHeadersAndRelatedUnifiersAreRegistered() throws Exception {
        GrandUnifier sut = new GrandUnifier(of(PASS_THROUGH_HTTP_HEADERS_UNIFIER, PASS_THROUGH_JSON_NODE_UNIFIER), emptyList());
        Response response = new Response(null, new HttpHeaders(), OK);
        Response result = sut.unify(response, mock(UnifiedOdataIdResolver.class));
        assertEquals(result.getBody(), response.getBody());
        assertEquals(result.getHttpHeaders(), response.getHttpHeaders());
        assertEquals(result.getHttpStatus(), response.getHttpStatus());
    }

    @Test
    public void whenRequestedJsonNodeIsInheritedFromHierarchyOfRegisteredUnifier() {
        GrandUnifier sut = new GrandUnifier(of(PASS_THROUGH_JSON_NODE_UNIFIER), emptyList());
        TextNode toBeUnified = new TextNode("someThingToBeUnified");
        TextNode result = sut.unify(toBeUnified, mock(UnifiedOdataIdResolver.class));
        assertEquals(result, toBeUnified);
    }

    @Test(expectedExceptions = UncheckedExecutionException.class)
    public void deunifyHttpHeadersWhenRelatedDeunifierIsNotRegistered_exceptionShouldBeThrown() throws Exception {
        GrandUnifier sut = new GrandUnifier(emptyList(), emptyList());
        sut.deunify(new HttpHeaders(), mock(UnifiedOdataIdResolver.class));
    }

    @Test
    public void deunifyHttpHeadersWhenRelatedDeunifierIsRegistered() throws Exception {
        GrandUnifier sut = new GrandUnifier(emptyList(), of(PASS_THROUGH_HTTP_HEADERS_DEUNIFIER));
        HttpHeaders headers = ResponseEntity.ok().build().getHeaders();
        UnifiedOdataIdResolver resolver = mock(UnifiedOdataIdResolver.class);
        HttpHeaders result = sut.deunify(headers, resolver);
        assertEquals(result, headers);
    }
}
