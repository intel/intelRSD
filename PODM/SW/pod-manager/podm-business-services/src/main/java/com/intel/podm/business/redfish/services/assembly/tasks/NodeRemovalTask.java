/*
 * Copyright (c) 2017 Intel Corporation
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

package com.intel.podm.business.redfish.services.assembly.tasks;

import com.intel.podm.common.enterprise.utils.logger.TimeMeasured;
import com.intel.podm.common.enterprise.utils.retry.NumberOfRetriesOnRollback;
import com.intel.podm.common.enterprise.utils.retry.RetryOnRollbackInterceptor;

import javax.interceptor.Interceptors;
import javax.transaction.Transactional;

import static javax.transaction.Transactional.TxType.MANDATORY;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

public abstract class NodeRemovalTask extends NodeTask {

    @Transactional(MANDATORY)
    @TimeMeasured(tag = "[NodeDisassemblyTask]")
    public abstract void disassemble();

    @Transactional(MANDATORY)
    @TimeMeasured(tag = "[NodeDeallocateTask]")
    public abstract void deallocate();

    @Override
    @NumberOfRetriesOnRollback(3)
    @Interceptors(RetryOnRollbackInterceptor.class)
    @Transactional(REQUIRES_NEW)
    @TimeMeasured(tag = "[NodeRemovalTask]")
    public void run() {
        disassemble();
        deallocate();
    }
}
