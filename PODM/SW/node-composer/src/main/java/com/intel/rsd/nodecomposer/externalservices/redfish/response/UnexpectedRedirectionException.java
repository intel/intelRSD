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

package com.intel.rsd.nodecomposer.externalservices.redfish.response;

import java.net.URI;

public class UnexpectedRedirectionException extends Exception {
    private static final long serialVersionUID = -4884503888507888079L;
    private final URI expectedUri;
    private final URI redirectUri;

    UnexpectedRedirectionException(URI expectedUri, URI redirectUri) {
        this.expectedUri = expectedUri;
        this.redirectUri = redirectUri;
    }

    public URI getExpectedUri() {
        return expectedUri;
    }

    public URI getRedirectUri() {
        return redirectUri;
    }
}
