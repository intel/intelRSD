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

package com.intel.rsd.resourcemanager.layers.tagger;

import com.fasterxml.jackson.core.JsonPointer;
import lombok.experimental.UtilityClass;

import static com.fasterxml.jackson.core.JsonPointer.compile;
import static org.apache.commons.lang3.StringUtils.substringAfter;
import static org.apache.commons.lang3.StringUtils.substringBefore;

@UtilityClass
class JsonPointerUtils {
    static JsonPointer parentOf(JsonPointer pointer) {
        return compile(substringBefore(pointer.toString(), pointer.last().toString()));
    }

    static String leafNameOf(JsonPointer pointer) {
        return substringAfter(pointer.last().toString(), "/");
    }
}
