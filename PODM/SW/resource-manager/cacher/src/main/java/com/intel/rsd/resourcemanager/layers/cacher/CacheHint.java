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

package com.intel.rsd.resourcemanager.layers.cacher;

import lombok.NonNull;

import java.time.LocalDateTime;

import static java.time.LocalDateTime.now;
import static java.time.temporal.ChronoUnit.SECONDS;

public final class CacheHint {

    public static final CacheHint NEVER_CACHED = new CacheHint(LocalDateTime.MAX);
    public static final CacheHint ALWAYS_CACHED = new CacheHint(LocalDateTime.MIN);

    private final LocalDateTime oldestAcceptable;

    private CacheHint(LocalDateTime oldestAcceptable) {
        this.oldestAcceptable = oldestAcceptable;
    }

    public static CacheHint asCacheHint(@NonNull Long maxStale) {
        return maxStale == 0 ? NEVER_CACHED : new CacheHint(now().minus(maxStale, SECONDS));
    }

    public boolean isAfter(LocalDateTime lastUpdate) {
        return oldestAcceptable.isAfter(lastUpdate);
    }
}
