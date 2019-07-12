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
import lombok.val;
import org.testng.annotations.Test;

import static com.intel.rsd.redfish.RedfishErrorCode.InvalidEndpoint;
import static com.intel.rsd.redfish.RedfishErrors.createRedfishError;
import static com.intel.rsd.resourcemanager.layers.merger.response.ErrorResponseFactory.createErrorResponse;
import static org.assertj.core.api.AssertionsForClassTypes.assertThat;
import static org.springframework.http.HttpStatus.CONFLICT;
import static org.springframework.http.HttpStatus.NOT_FOUND;
import static org.testng.Assert.assertEquals;
import static org.testng.Assert.assertTrue;

public class ErrorResponseFactoryTest {

    @Test
    public void testCreateNotFoundResponse() throws Exception {
        val redfishError = createRedfishError(NOT_FOUND);

        val sut = createErrorResponse(redfishError);
        assertEquals(sut.getHttpStatus(), NOT_FOUND);
        assertThat(sut.getBody()).isInstanceOf(ObjectNode.class);
        assertThat(sut.getBody())
            .matches(actual -> actual.has("error"))
            .matches(actual -> !actual.at("/error/code").isMissingNode());
        assertThat(sut.getBody().at("/error/code")).isEqualTo(new TextNode(InvalidEndpoint.asString()));
    }

    @Test
    public void testCreateErrorResponse() {
        val redfishError = createRedfishError(CONFLICT);
        val errorResponse = createErrorResponse(redfishError);

        assertEquals(errorResponse.getHttpStatus(), redfishError.getHttpStatus());
        assertTrue(errorResponse.getBody() != null);
        assertThat(errorResponse.getBody()).isInstanceOf(ObjectNode.class);
        assertThat(errorResponse.getBody())
            .matches(actual ->
                    actual.has("error"),
                "Error response should contain 'error' field"
            )
            .matches(actual ->
                redfishError.getError().getRedfishErrorCode().asString().equals(actual.at("/error/code").asText()),
                "Error responses has appropriate error code"
            );
        assertThat(errorResponse.getBody().at("/error/code")).isEqualTo(new TextNode(redfishError.getError().getRedfishErrorCode().asString()));
        assertThat(errorResponse.getBody().at("/error/message")).isEqualTo(new TextNode(redfishError.getError().getMessage()));
    }
}
