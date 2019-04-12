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

package com.intel.rsd.podm.sessions;

import lombok.Data;

import java.io.Serializable;
import java.time.Instant;
import java.util.UUID;

import static java.time.Instant.now;

@Data
public class RedfishSession implements Serializable {
    public static final String X_AUTH_TOKEN = "X-Auth-Token";

    private static final long serialVersionUID = -9104755116186894107L;

    private final String id;

    private final String xAuthToken;

    private String userName;

    private final Instant creationTime = now();

    private Instant lastAccessedTime = this.creationTime;

    private String role;

    public RedfishSession() {
        this.id = generateRandomIdentifier();
        this.xAuthToken = generateRandomIdentifier();
    }

    private String generateRandomIdentifier() {
        return UUID.randomUUID().toString();
    }
}
