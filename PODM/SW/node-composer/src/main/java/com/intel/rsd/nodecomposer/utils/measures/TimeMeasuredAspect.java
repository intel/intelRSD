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

package com.intel.rsd.nodecomposer.utils.measures;

import com.google.common.base.Stopwatch;
import lombok.extern.slf4j.Slf4j;
import org.aspectj.lang.ProceedingJoinPoint;
import org.aspectj.lang.annotation.Around;
import org.aspectj.lang.annotation.Aspect;
import org.aspectj.lang.reflect.MethodSignature;
import org.springframework.context.annotation.Scope;
import org.springframework.core.annotation.Order;
import org.springframework.stereotype.Component;

import java.io.Serializable;

import static org.apache.commons.lang3.StringUtils.isEmpty;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_SINGLETON;

@Slf4j
@Aspect
@Component
@Scope(SCOPE_SINGLETON)
@Order(100)
public class TimeMeasuredAspect implements Serializable {
    private static final long serialVersionUID = -8802729860808177818L;

    @Around("execution(* *(..)) && @annotation(timeMeasured)")
    @SuppressWarnings({"checkstyle:IllegalThrows"})
    public Object invoke(ProceedingJoinPoint joinPoint, TimeMeasured timeMeasured) throws Throwable {
        String identity = getClassIdentityName(joinPoint);
        String methodName = ((MethodSignature) joinPoint.getSignature()).getMethod().getName();
        String tag = timeMeasured.tag();
        tag += isEmpty(tag) ? "" : " ";

        log.info("{}{}.{} - started", tag, identity, methodName);
        Stopwatch stopwatch = Stopwatch.createStarted();
        Object result = joinPoint.proceed();
        stopwatch.stop();
        log.info("{}{}.{} - ended, execution time: {}", tag, identity, methodName, stopwatch);
        return result;
    }

    @SuppressWarnings({"unchecked"})
    private String getClassIdentityName(ProceedingJoinPoint joinPoint) {
        Object target = joinPoint.getTarget();
        Class<?> clazz = target.getClass();
        if (clazz.isAnnotationPresent(TimeMeasuredClassIdentityProvider.class)) {
            try {
                IdentityProvider provider = clazz.getAnnotation(TimeMeasuredClassIdentityProvider.class).value().newInstance();
                return provider.getIdentity(target);
            } catch (InstantiationException | IllegalAccessException e) {
                log.trace("Could not obtain identity name for {}", clazz.getSimpleName());
            }
        }
        return clazz.getSimpleName();
    }
}
