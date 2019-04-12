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
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;

@Component
@Slf4j
public class ModelState {

    private final IAtomicReference<Boolean> isDirty;

    @Autowired
    public ModelState(@Shared("ModelState.isDirty") IAtomicReference<Boolean> isDirty) {
        this.isDirty = isDirty;
    }

    public void dirty(String source) {
        log.info("Marking model(dirty) because of {}", source);
        isDirty.alter(input -> true);
    }

    public boolean getAndResetIfDirty() {
        return isDirty.compareAndSet(true, false);
    }
}
