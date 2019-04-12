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

package com.intel.rsd.resourcemanager.layers.cacher;

import lombok.NonNull;
import lombok.experimental.UtilityClass;
import lombok.val;
import org.springframework.http.HttpHeaders;

import java.util.Optional;
import java.util.regex.Pattern;

import static java.lang.Long.parseLong;
import static java.util.Optional.empty;
import static java.util.Optional.of;

@UtilityClass
final class HttpHeadersExtensions {
    private static final Pattern MAX_STALE_PATTERN = Pattern.compile("max-stale=(?<maxStaleSeconds>\\d+)");

    public static Optional<Long> getMaxStale(@NonNull HttpHeaders headers) {
        val cacheControlString = headers.getCacheControl();
        if (cacheControlString != null) {
            val parts = cacheControlString.split(",");
            for (String part : parts) {
                val matcher = MAX_STALE_PATTERN.matcher(part);

                if (matcher.matches()) {
                    return of(parseLong(matcher.group("maxStaleSeconds")));
                }
            }
        }

        return empty();
    }
}
