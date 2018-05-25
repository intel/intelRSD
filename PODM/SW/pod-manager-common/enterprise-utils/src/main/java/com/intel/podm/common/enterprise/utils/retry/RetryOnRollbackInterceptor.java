/*
 * Copyright (c) 2015-2018 Intel Corporation
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

package com.intel.podm.common.enterprise.utils.retry;

import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.types.exceptions.NoRetriesLeftException;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.interceptor.AroundInvoke;
import javax.interceptor.InvocationContext;
import javax.persistence.OptimisticLockException;
import javax.transaction.RollbackException;

@Dependent
public class RetryOnRollbackInterceptor {
    @Inject
    private Logger logger;

    @AroundInvoke
    public Object invoke(InvocationContext ic) throws Exception {
        Integer retriesLeft = getNumberOfRetries(ic);
        Exception lastException = null;
        while (retriesLeft > 0) {
            try {
                return ic.proceed();
            } catch (RollbackException | OptimisticLockException e) {
                logger.w("Retrying method: {}", ic.getMethod());
                lastException = e;
                retriesLeft--;
            }
        }
        throw new NoRetriesLeftException(lastException);
    }

    private Integer getNumberOfRetries(InvocationContext ic) {
        RetryOnRollback annotation = ic.getMethod().getAnnotation(RetryOnRollback.class);
        return annotation == null ? 1 : annotation.value();
    }
}
