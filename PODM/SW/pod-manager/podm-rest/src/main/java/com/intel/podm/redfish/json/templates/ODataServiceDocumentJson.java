/*
 * Copyright (c) 2016-2017 Intel Corporation
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

package com.intel.podm.redfish.json.templates;

import com.fasterxml.jackson.annotation.JsonInclude;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.intel.podm.common.types.ServiceKind;

import java.net.URI;
import java.util.ArrayList;
import java.util.List;

import static com.fasterxml.jackson.annotation.JsonInclude.Include.NON_NULL;

@JsonPropertyOrder({
    "@odata.context", "value"
})
@SuppressWarnings({"checkstyle:VisibilityModifier"})
@JsonInclude(NON_NULL)
public class ODataServiceDocumentJson extends BaseJson {
    @JsonProperty("@odata.context")
    public URI oDataContext;
    @JsonProperty("value")
    public List<ODataServiceJson> value = new ArrayList<>();

    public ODataServiceDocumentJson() {
        super(null);
    }

    @JsonPropertyOrder({
        "name", "kind", "url"
    })
    public static final class ODataServiceJson {
        public String name;
        public ServiceKind kind;
        public URI url;

        public ODataServiceJson(String name, ServiceKind kind, URI url) {
            this.name = name;
            this.kind = kind;
            this.url = url;
        }
    }
}
