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

import com.fasterxml.jackson.databind.node.ObjectNode;
import com.fasterxml.jackson.databind.node.TextNode;
import com.intel.rsd.resourcemanager.layers.Response;
import org.springframework.http.HttpStatus;
import org.testng.annotations.Test;

import static org.assertj.core.api.AssertionsForClassTypes.assertThat;
import static org.springframework.http.HttpStatus.NOT_FOUND;
import static org.testng.Assert.assertEquals;
import static org.testng.Assert.assertTrue;

public class ErrorResponseFactoryTest {

    @Test
    public void testCreateNotFoundResponse() throws Exception {
        Response notFoundResponse = ErrorResponseFactory.createNotFoundResponse();
        assertEquals(notFoundResponse.getHttpStatus(), NOT_FOUND);
        assertThat(notFoundResponse.getBody())
            .isInstanceOf(ObjectNode.class);
        assertThat(notFoundResponse.getBody())
            .matches(actual -> actual.has("error"))
            .matches(actual -> !actual.at("/error/code").isMissingNode());
        assertThat(notFoundResponse.getBody().at("/error/code")).isEqualTo(new TextNode("Base.1.0.InvalidEndpoint"));
    }

    @Test
    public void testCreateErrorResponse() {
        String code = "code";
        String message = "message";
        HttpStatus status = HttpStatus.CONFLICT;

        Response errorResponse = ErrorResponseFactory.createErrorResponse(code, message, status);
        assertEquals(errorResponse.getHttpStatus(), status);
        assertTrue(errorResponse.getBody() != null);
        assertThat(errorResponse.getBody()).isInstanceOf(ObjectNode.class);
        assertThat(errorResponse.getBody())
            .matches(actual -> actual.has("error"))
            .matches(actual -> code.equals(actual.at("/error/code").asText()));
        assertThat(errorResponse.getBody().at("/error/code")).isEqualTo(new TextNode(code));
        assertThat(errorResponse.getBody().at("/error/message")).isEqualTo(new TextNode(message));
    }
}
