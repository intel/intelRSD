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
package com.intel.podm.tester.utilities;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.podm.components.ODataId;

import java.util.Collections;
import java.util.List;

public class LagCreationJson {

    @JsonProperty("Links")
    final Link link;

    public LagCreationJson(String portMemberUrl) {
        this.link = new Link(new ODataId(portMemberUrl));
    }

    private class Link {
        @JsonProperty("PortMembers")
        List<ODataId> portMembers;

        Link(ODataId portMember) {
            this.portMembers = Collections.singletonList(portMember);
        }
    }
}
