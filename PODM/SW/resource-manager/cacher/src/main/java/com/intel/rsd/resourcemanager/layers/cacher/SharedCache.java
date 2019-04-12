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

package com.intel.rsd.resourcemanager.layers.cacher;

import com.intel.rsd.resourcemanager.layers.Response;
import com.intel.rsd.services.common.Shared;
import lombok.extern.slf4j.Slf4j;
import lombok.val;
import org.springframework.stereotype.Component;

import java.util.Map;
import java.util.Optional;

import static java.util.Optional.empty;
import static java.util.Optional.of;

@Component
@Slf4j
//todo: expose last/next stale entries collection time as metric?
public class SharedCache implements Cache {
    private final Map<ResourceReference, SerializedResponse> cache;

    @SuppressWarnings("unchecked")
    public SharedCache(@Shared("SharedCache.cache") Map<?, ?> cache) {
        this.cache = (Map<ResourceReference, SerializedResponse>) cache;
    }

    @Override
    public Optional<SerializedResponse> findCached(ResourceReference reference, CacheHint noOlderThan) {
        val response = cache.get(reference);

        if (response == null) {
            log.trace("{} does not exist in a cache", reference);
            return empty();
        }

        if (noOlderThan.isAfter(response.getLastUpdate())) {
            log.trace("{} does exist in a cache but is older than {}", reference, noOlderThan);
            return empty();
        }

        log.debug("{} does exist in a cache ", reference);
        return of(response);
    }

    @Override
    public void updateOrCreate(ResourceReference reference, Response response) {
        cache.put(reference, new SerializedResponse(response));
    }

    @Override
    public void remove(ResourceReference resourceReference) {
        cache.remove(resourceReference);
    }
}
