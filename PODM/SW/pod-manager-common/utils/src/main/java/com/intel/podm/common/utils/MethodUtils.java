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

package com.intel.podm.common.utils;

import java.lang.annotation.Annotation;
import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.List;

import static org.apache.commons.lang.Validate.isTrue;

public final class MethodUtils {

    private MethodUtils() {
    }

    public static List<Method> getMethodsListWithAnnotation(final Class<?> cls, final Class<? extends Annotation> annotationCls) {
        isTrue(cls != null, "The class must not be null");
        isTrue(annotationCls != null, "The annotation class must not be null");
        final Method[] allMethods = cls.getMethods();
        final List<Method> annotatedMethods = new ArrayList<Method>();
        for (final Method method : allMethods) {
            if (method.getAnnotation(annotationCls) != null) {
                annotatedMethods.add(method);
            }
        }
        return annotatedMethods;
    }
}
