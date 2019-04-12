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

package com.intel.rsd.nodecomposer.utils.lock;

import lombok.extern.slf4j.Slf4j;
import org.aspectj.lang.ProceedingJoinPoint;
import org.aspectj.lang.annotation.Around;
import org.aspectj.lang.annotation.Aspect;
import org.aspectj.lang.reflect.MethodSignature;
import org.springframework.context.annotation.Scope;
import org.springframework.core.annotation.Order;
import org.springframework.stereotype.Component;

import java.io.Serializable;
import java.lang.reflect.Method;

import static com.intel.rsd.nodecomposer.utils.lock.LockCoordinator.lockOrThrow;
import static com.intel.rsd.nodecomposer.utils.lock.LockCoordinator.unlockIfHeldByCurrentThread;
import static java.time.Duration.ofMillis;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_SINGLETON;

@Aspect
@Slf4j
@Component
@Scope(SCOPE_SINGLETON)
@Order(200)
public class LockAspect implements Serializable {
    private static final long serialVersionUID = -3006436408768196562L;

    @Around("execution(* *(..)) && @annotation(lock)")
    @SuppressWarnings({"checkstyle:IllegalThrows"})
    public Object invoke(ProceedingJoinPoint joinPoint, Lock lock) throws Throwable {
        Class<?> targetClass = joinPoint.getTarget().getClass();
        Method method = joinPoint.getTarget().getClass().getDeclaredMethod(joinPoint.getSignature().getName(),
            ((MethodSignature) joinPoint.getSignature()).getMethod().getParameterTypes());
        String name = targetClass.getName() + "." + method.getName() + "(...)";

        try {
            log.trace("Before locking: " + name);
            lockOrThrow(targetClass, ofMillis(lock.accessTimeoutInMs()));
            log.trace("After locking: " + name);
            return joinPoint.proceed();
        } finally {
            log.trace("Before unlocking: " + name);
            unlockIfHeldByCurrentThread(targetClass);
            log.trace("After unlocking: " + name);
        }
    }
}
