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

package com.intel.rsd.nodecomposer;

import com.hazelcast.core.IAtomicReference;
import com.intel.rsd.services.common.Shared;
import lombok.NonNull;
import lombok.extern.slf4j.Slf4j;
import lombok.val;
import org.apache.commons.lang3.builder.ToStringBuilder;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;

import java.time.LocalDateTime;
import java.util.Set;

import static java.lang.Thread.currentThread;

@Component
@Slf4j
public class ModelState {

    private final Set<String> runningOperations;
    private final IAtomicReference<LocalDateTime> timestamp;

    @Autowired
    public ModelState(@Shared("ModelState:running-operations") Set<?> runningOperations,
                      @Shared("ModelState:timestamp") IAtomicReference<LocalDateTime> timestamp) {

        this.runningOperations = (Set<String>) runningOperations;
        this.timestamp = timestamp;
        this.timestamp.alter(existing -> existing == null ? LocalDateTimeProvider.now() : existing);
    }

    public void register() {
        val thread = currentThread().getName();
        runningOperations.add(thread);
    }

    public void arrived() {
        String thread = currentThread().getName();
        timestamp.set(LocalDateTimeProvider.now());
        runningOperations.remove(thread);
    }

    public boolean isNewerThan(@NonNull LocalDateTime lastSyncTimestamp) {
        return !runningOperations.isEmpty()
            || this.timestamp.get().isAfter(lastSyncTimestamp)
            || this.timestamp.get().isEqual(lastSyncTimestamp);
    }

    @Override
    public String toString() {
        return new ToStringBuilder(this)
            .append("runningOperations", runningOperations)
            .append("timestamp", timestamp.get())
            .toString();
    }
}
