/*
 * Copyright (c) 2015-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.utils.retry;

import com.intel.rsd.nodecomposer.types.exceptions.NoRetriesLeftException;
import lombok.extern.slf4j.Slf4j;
import org.aspectj.lang.ProceedingJoinPoint;
import org.aspectj.lang.annotation.Around;
import org.aspectj.lang.annotation.Aspect;
import org.aspectj.lang.reflect.MethodSignature;
import org.springframework.context.annotation.Scope;
import org.springframework.core.annotation.Order;
import org.springframework.orm.ObjectOptimisticLockingFailureException;
import org.springframework.stereotype.Component;

import javax.persistence.OptimisticLockException;
import javax.persistence.PersistenceException;
import javax.transaction.RollbackException;
import java.io.Serializable;

import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_SINGLETON;

@Slf4j
@Aspect
@Component
@Scope(SCOPE_SINGLETON)
@Order(300)
public class RetryOnRollbackAspect implements Serializable {
    private static final long serialVersionUID = 2495996382984369338L;

    @Around("execution(* *(..)) && @annotation(retryOnRollback)")
    @SuppressWarnings({"checkstyle:IllegalThrows"})
    public Object invoke(ProceedingJoinPoint joinPoint, RetryOnRollback retryOnRollback) throws Throwable {
        Integer retriesLeft = retryOnRollback == null ? 1 : retryOnRollback.value();
        Exception lastException = null;
        while (retriesLeft > 0) {
            try {
                return joinPoint.proceed();
            } catch (RollbackException | ObjectOptimisticLockingFailureException | OptimisticLockException e) {
                log.warn("Retrying method: {}", ((MethodSignature) joinPoint.getSignature()).getMethod());
                lastException = e;
                retriesLeft--;
            } catch (PersistenceException e) {
                // https://jira01.devtools.intel.com/browse/RS6-1256
                // possible problem of ConstraintViolation fix in 5.2.13 -> https://hibernate.atlassian.net/browse/HHH-12074
                if (e.getCause() instanceof org.hibernate.exception.ConstraintViolationException) {
                    log.warn("Retrying in case of ConstraintViolation exception method: {}, {}", ((MethodSignature) joinPoint.getSignature()).getMethod(), e);
                    lastException = e;
                    retriesLeft--;
                } else {
                    throw e;
                }
            }
        }
        throw new NoRetriesLeftException(lastException);
    }
}
