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

package com.intel.rsd.podm.services;

import com.fasterxml.jackson.databind.JsonNode;
import org.hamcrest.Description;
import org.hamcrest.TypeSafeMatcher;

public class ContainsLinkTo extends TypeSafeMatcher<JsonNode> {

    private final String linkId;

    ContainsLinkTo(String linkId) {
        this.linkId = linkId;
    }

    @Override
    protected boolean matchesSafely(JsonNode node) {
        for (JsonNode aNode : node) {
            String link = aNode.get("@odata.id").asText();
            if (link.equalsIgnoreCase(linkId)) {
                return true;
            }
        }
        return false;
    }

    @Override
    public void describeTo(Description description) {
        description.appendText("Searching for link " + linkId);
    }
}
