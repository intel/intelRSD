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

package com.intel.rsd.resourcemanager.runner.requiredlayer;

import com.fasterxml.jackson.databind.JsonNode;
import com.intel.rsd.http.HtmlErrorToJsonNodeConverter;
import lombok.val;
import org.assertj.core.api.Assertions;
import org.springframework.http.client.ClientHttpRequestFactory;
import org.testng.annotations.Test;

import static java.net.URI.create;
import static java.util.Arrays.asList;
import static java.util.Collections.emptyList;
import static org.mockito.Mockito.mock;
import static org.springframework.http.HttpHeaders.ACCEPT;
import static org.springframework.http.HttpMethod.GET;
import static org.springframework.http.HttpStatus.INTERNAL_SERVER_ERROR;
import static org.springframework.http.MediaType.APPLICATION_JSON_VALUE;
import static org.springframework.http.MediaType.TEXT_HTML;
import static org.springframework.http.RequestEntity.get;
import static org.springframework.test.web.client.MockRestServiceServer.createServer;
import static org.springframework.test.web.client.match.MockRestRequestMatchers.method;
import static org.springframework.test.web.client.match.MockRestRequestMatchers.requestTo;
import static org.springframework.test.web.client.response.MockRestResponseCreators.withServerError;

public class RestTemplateBasedRestClientTest {

    private static final String SAMPLE_HTML_BODY = "<html><body>NotEvenPretendingToBeJson</body></html>";

    @Test
    public void whenUnderlyingServiceReturnsTextHtmlBody_itShouldBeConvertedIntoApplicationJsonBody() {
        val restTemplate = new RsdRestTemplate(mock(ClientHttpRequestFactory.class), asList(new HtmlErrorToJsonNodeConverter()), emptyList());
        val uri = create("http://localhost/html");

        val responseEntity = withServerError().contentType(TEXT_HTML).body(SAMPLE_HTML_BODY);
        val requestEntity = get(uri).header(ACCEPT, APPLICATION_JSON_VALUE).build();

        createServer(restTemplate)
            .expect(requestTo(uri))
            .andExpect(method(GET))
            .andRespond(responseEntity);

        val response = restTemplate.exchange(requestEntity, JsonNode.class);

        Assertions.assertThat(response)
            .hasFieldOrPropertyWithValue("status", INTERNAL_SERVER_ERROR.value())
            .extracting("body")
            .doesNotContainNull()
            .anySatisfy(body -> Assertions.assertThat(body.toString()).contains(SAMPLE_HTML_BODY));
    }
}
