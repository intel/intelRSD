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

package com.intel.rsd.service.detector.data;

import com.fasterxml.jackson.annotation.JsonIgnore;
import com.fasterxml.jackson.annotation.JsonProperty;
import lombok.Data;
import lombok.NoArgsConstructor;

import javax.persistence.Column;
import javax.persistence.Entity;
import javax.persistence.Enumerated;
import javax.persistence.Id;
import java.net.URI;
import java.util.UUID;

import static com.intel.rsd.service.detector.data.Service.State.Enabled;
import static javax.persistence.EnumType.STRING;

@Data
@Entity
@NoArgsConstructor
public class Service {
    @Id
    @JsonProperty("Id")
    private String id;

    @Column(unique = true)
    @JsonProperty("ServiceRootUri")
    private URI serviceUri;

    @JsonProperty("Type")
    @Enumerated(STRING)
    private RemoteServiceType remoteServiceType;

    @JsonIgnore
    private boolean trusted;

    @JsonProperty("Status")
    @Enumerated(STRING)
    private State status;

    public Service(UUID serviceRootUuid, URI serviceUri, RemoteServiceType remoteServiceType) {
        this.id = serviceRootUuid.toString();
        this.serviceUri = serviceUri;
        this.remoteServiceType = remoteServiceType;
        this.status = Enabled;
    }

    //TODO: use shared package of enums in podm-common (currently is in node-composer)
    public enum State {
        Enabled,
        UnavailableOffline
    }
}
