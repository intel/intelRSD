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

import com.hazelcast.config.Config;
import com.hazelcast.core.EntryView;
import com.hazelcast.map.eviction.MapEvictionPolicy;
import lombok.extern.slf4j.Slf4j;
import org.springframework.stereotype.Component;

import static com.hazelcast.config.MaxSizeConfig.MaxSizePolicy.USED_HEAP_SIZE;

@Component
@Slf4j
//todo: run on leader only?
public class SharedCacheEvictionConfiguration {
    public SharedCacheEvictionConfiguration(Config config, CacherConfiguration cacherConfiguration) {
        log.info(
            "Configuring Shared Cache with entry time to live {} and max heap size {} MB",
            cacherConfiguration.getEntriesTimeToLive(),
            cacherConfiguration.getMaxHeapSizeInMb()
        );

        config.getMapConfig("SharedCache.cache")
            .setMapEvictionPolicy(new MapEvictionPolicy() {
                @Override
                public int compare(EntryView entryView1, EntryView entryView2) {
                    return Long.compare(entryView1.getLastUpdateTime(), entryView2.getLastUpdateTime());
                }
            })
            .setTimeToLiveSeconds(cacherConfiguration.getEntriesTimeToLiveSeconds())
            .getMaxSizeConfig()
            .setMaxSizePolicy(USED_HEAP_SIZE).setSize(cacherConfiguration.getMaxHeapSizeInMb());
    }

}
