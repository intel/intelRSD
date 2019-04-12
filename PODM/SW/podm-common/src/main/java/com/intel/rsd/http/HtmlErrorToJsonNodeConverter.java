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

package com.intel.rsd.http;

import com.fasterxml.jackson.databind.JsonNode;
import org.apache.commons.codec.Charsets;
import org.springframework.http.HttpInputMessage;
import org.springframework.http.HttpOutputMessage;
import org.springframework.http.MediaType;
import org.springframework.http.client.ClientHttpResponse;
import org.springframework.http.converter.HttpMessageConverter;
import org.springframework.http.converter.HttpMessageNotReadableException;
import org.springframework.http.converter.HttpMessageNotWritableException;
import org.springframework.lang.Nullable;
import org.springframework.util.StreamUtils;

import java.io.IOException;
import java.util.Arrays;
import java.util.List;

import static com.intel.rsd.json.JsonUtils.objectToJsonNode;
import static com.intel.rsd.redfish.RedfishErrors.createRedfishError;
import static org.springframework.http.HttpStatus.INTERNAL_SERVER_ERROR;

public class HtmlErrorToJsonNodeConverter implements HttpMessageConverter<JsonNode> {
    @Override
    public boolean canRead(Class<?> clazz, @Nullable MediaType mediaType) {
        return MediaType.TEXT_HTML.includes(mediaType);
    }

    @Override
    public boolean canWrite(Class<?> clazz, @Nullable MediaType mediaType) {
        return false;
    }

    @Override
    public List<MediaType> getSupportedMediaTypes() {
        return Arrays.asList(MediaType.TEXT_HTML);
    }

    @Override
    public JsonNode read(Class<? extends JsonNode> clazz, HttpInputMessage inputMessage) throws HttpMessageNotReadableException, IOException {
        if (ClientHttpResponse.class.isAssignableFrom(inputMessage.getClass())) {
            if (((ClientHttpResponse) inputMessage).getStatusCode().isError()) {
                return objectToJsonNode(
                    createRedfishError(
                        INTERNAL_SERVER_ERROR,
                        "Returned response contains html data which cannot be converted to JSON",
                        extractBody(inputMessage)
                    )
                );
            }
        }

        throw new HttpMessageNotReadableException("Received message cannot be converted into JSON: " + extractBody(inputMessage));
    }

    private String extractBody(HttpInputMessage inputMessage) {
        try {
            return StreamUtils.copyToString(inputMessage.getBody(), Charsets.UTF_8);
        } catch (IOException e) {
            return null;
        }
    }

    @Override
    public void write(JsonNode jsonNode, MediaType contentType, HttpOutputMessage outputMessage) throws HttpMessageNotWritableException {
        throw new UnsupportedOperationException();
    }
}
