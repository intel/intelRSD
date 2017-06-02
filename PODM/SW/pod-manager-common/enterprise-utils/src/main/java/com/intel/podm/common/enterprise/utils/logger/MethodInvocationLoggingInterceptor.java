/*
 * Copyright (c) 2016-2017 Intel Corporation
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
import com.intel.podm.common.enterprise.utils.proxy.Unproxier;
import com.intel.podm.common.logger.Logger;
import org.apache.commons.lang.StringUtils;

import javax.inject.Inject;
import javax.interceptor.AroundInvoke;
import javax.interceptor.Interceptor;
import javax.interceptor.InvocationContext;

@Interceptor
@TimeMeasured
public class MethodInvocationLoggingInterceptor {
    @Inject
    private Logger logger;

    @AroundInvoke
    public Object interceptingMethod(InvocationContext ctx) throws Exception {
        String className = Unproxier.unproxy(ctx.getTarget().getClass()).getSimpleName();
        String methodName = ctx.getMethod().getName();
        String tag = ctx.getMethod().getAnnotation(TimeMeasured.class).tag();
        tag += StringUtils.isEmpty(tag) ? "" : " ";

        logger.d("{}{}.{} - started", tag, className, methodName);
        Stopwatch stopwatch = Stopwatch.createStarted();
        Object result = ctx.proceed();
        stopwatch.stop();
        logger.d("{}{}.{} - ended, execution time: {}", tag, className, methodName, stopwatch);
        return result;
    }
}
