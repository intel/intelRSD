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

package com.intel.rsd.podm.security.whitelist;

import com.intel.rsd.dto.accessverifier.VerifyAccessRequest;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.HttpMethod;
import org.springframework.stereotype.Component;
import org.springframework.util.AntPathMatcher;

import java.util.List;

@Component
public class WhiteListedAccessRequestsRegistry {

    private final List<WhiteListedAccessRequest> whitelistedRequests;
    private final AntPathMatcher antPathMatcher;

    @Autowired
    public WhiteListedAccessRequestsRegistry(WhiteListedAccessRequestsConfiguration serviceConfig) {
        this.whitelistedRequests = serviceConfig.getWhiteListedAccessRequests();
        this.antPathMatcher = new AntPathMatcher();
    }

    public boolean isWhitelisted(VerifyAccessRequest request) {
        for (WhiteListedAccessRequest whitelistedRequest : whitelistedRequests) {
            if (match(whitelistedRequest, request)) {
                return true;
            }
        }
        return false;
    }

    private boolean match(WhiteListedAccessRequest whiteListedAccessRequest, VerifyAccessRequest request) {
        return sameUri(whiteListedAccessRequest, request) && containsRequestedMethod(whiteListedAccessRequest, request.getMethod());
    }

    private boolean sameUri(WhiteListedAccessRequest whiteListedAccessRequest, VerifyAccessRequest request) {
        return this.antPathMatcher.match(whiteListedAccessRequest.getPath(), request.getUri());
    }

    private boolean containsRequestedMethod(WhiteListedAccessRequest whiteListedAccessRequest, HttpMethod requestMethod) {
        return whiteListedAccessRequest.getMethods().contains(requestMethod);
    }
}
