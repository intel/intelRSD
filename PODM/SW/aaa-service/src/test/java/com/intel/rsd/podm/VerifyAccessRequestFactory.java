/*
 * Copyright (c) 2019 Intel Corporation
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

package com.intel.rsd.podm;

import com.intel.rsd.dto.accessverifier.VerifyAccessRequest;
import org.springframework.http.HttpHeaders;
import org.springframework.http.HttpMethod;
import org.springframework.stereotype.Component;

import java.util.Base64;

import static com.intel.rsd.podm.sessions.RedfishSession.X_AUTH_TOKEN;
import static java.nio.charset.StandardCharsets.UTF_8;
import static org.springframework.http.HttpHeaders.AUTHORIZATION;

@Component
class VerifyAccessRequestFactory {

    VerifyAccessRequest create(String uri, HttpMethod method) {
        return new VerifyAccessRequest(uri, method, new HttpHeaders());
    }

    VerifyAccessRequest create(String uri, HttpMethod method, String login, String password) {
        VerifyAccessRequest verifyAccessRequest = create(uri, method);
        verifyAccessRequest.getHeaders().set(AUTHORIZATION, encodeBasicAuthToken(login, password));
        return verifyAccessRequest;
    }

    VerifyAccessRequest create(String uri, HttpMethod method, String xAuthToken) {
        VerifyAccessRequest verifyAccessRequest = create(uri, method);
        verifyAccessRequest.getHeaders().set(X_AUTH_TOKEN, xAuthToken);
        return verifyAccessRequest;
    }

    private String encodeBasicAuthToken(String login, String password) {
        String creds = String.format("%s:%s", login, password);
        return "Basic " + new String(Base64.getEncoder().encode(creds.getBytes(UTF_8)));
    }
}
