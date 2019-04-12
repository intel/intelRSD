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

package com.intel.rsd.resourcemanager.layers.tagger;

import com.fasterxml.jackson.core.JsonPointer;
import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.node.TextNode;
import org.springframework.validation.Errors;

import static com.fasterxml.jackson.core.JsonPointer.compile;
import static com.intel.rsd.resourcemanager.layers.tagger.JsonNodeUtils.putNodeAtPointer;
import static java.lang.String.format;

public class ReadOnlyTagsActions {

    private static final JsonNode DEFAULT_ODATA_TYPE_VALUE = new TextNode("#Intel.Oem.Resource");
    private static final String OEM_INTEL_RACKSCALE = "/Oem/Intel_RackScale";
    private static final JsonPointer OEM_INTEL_RACKSCALE_ODATA_POINTER = compile("/Oem/Intel_RackScale/@odata.type");

    public static void addODataTypeIfNeeded(TagDefinition tagDefinition, JsonNode jsonNode) {
        if (tagDefinition.getProperty().toString().startsWith(OEM_INTEL_RACKSCALE)
            && jsonNode.at(OEM_INTEL_RACKSCALE_ODATA_POINTER).isMissingNode()) {
            putNodeAtPointer(jsonNode, OEM_INTEL_RACKSCALE_ODATA_POINTER, DEFAULT_ODATA_TYPE_VALUE);
        }
    }

    public static void rejectIfContainsReadOnlyTags(JsonNode jsonNode, Errors errors) {
        if (!jsonNode.at(OEM_INTEL_RACKSCALE_ODATA_POINTER).isMissingNode()) {
            errors.rejectValue(OEM_INTEL_RACKSCALE, "IllegalArgument",
                format("Cannot change %s field, because it is a read-only field", OEM_INTEL_RACKSCALE_ODATA_POINTER));
        }
    }
}
