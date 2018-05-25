/*
 * Copyright (c) 2016-2018 Intel Corporation
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

package com.intel.podm.common.enterprise.utils.logger;

import com.google.common.base.Stopwatch;
import com.intel.podm.common.logger.Logger;

import javax.inject.Inject;
import javax.interceptor.AroundInvoke;
import javax.interceptor.Interceptor;
import javax.interceptor.InvocationContext;
import java.lang.reflect.Method;

import static com.intel.podm.common.enterprise.utils.proxy.Unproxier.unproxy;
import static org.apache.commons.lang3.StringUtils.isEmpty;

@Interceptor
@TimeMeasured
public class TimeMeasuredInterceptor {
    @Inject
    private Logger logger;

    @AroundInvoke
    public Object interceptingMethod(InvocationContext ctx) throws Exception {
        Method method = ctx.getMethod();
        String tag = getMethodAnnotationTag(method);
        String identity = getClassIdentityName(ctx.getTarget());
        String methodName = method.getName();

        logger.d("{}{}.{} - started", tag, identity, methodName);
        Stopwatch stopwatch = Stopwatch.createStarted();
        Object result = ctx.proceed();
        stopwatch.stop();
        logger.d("{}{}.{} - ended, execution time: {}", tag, identity, methodName, stopwatch);

        return result;
    }

    private String getMethodAnnotationTag(Method method) {
        String tag = method.getAnnotation(TimeMeasured.class).tag();
        return !isEmpty(tag) ? tag.concat(" ") : "";
    }

    @SuppressWarnings({"unchecked"})
    private String getClassIdentityName(Object ctxTarget) {
        Class<?> unproxiedClass = unproxy(ctxTarget.getClass());
        if (unproxiedClass.isAnnotationPresent(TimeMeasuredClassIdentityProvider.class)) {
            try {
                IdentityProvider provider = unproxiedClass.getAnnotation(TimeMeasuredClassIdentityProvider.class).value().newInstance();
                return provider.getIdentity(ctxTarget);
            } catch (InstantiationException | IllegalAccessException e) {
                logger.t("Could not obtain identity name for {}", unproxiedClass.getSimpleName());
            }
        }
        return unproxiedClass.getSimpleName();
    }
}
