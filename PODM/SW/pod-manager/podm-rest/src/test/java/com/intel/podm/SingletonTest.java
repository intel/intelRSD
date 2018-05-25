/*
 * Copyright (c) 2017-2018 Intel Corporation
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

package com.intel.podm;

import org.testng.annotations.BeforeClass;
import org.testng.annotations.Test;

import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

import static com.intel.podm.common.utils.ClassGatherer.getAllClassesByPackageAndAnnotation;
import static com.intel.podm.common.utils.FailManager.failWithMessageIfAnyError;
import static java.lang.reflect.Modifier.isPublic;
import static java.util.Arrays.stream;
import static java.util.stream.Collectors.toList;

public class SingletonTest {
    private static final String PODM_PACKAGE = "com.intel.*";
    private static final Set<Class<?>> EJB_SINGLETONS = new HashSet<>();
    private static final Set<Class<?>> CDI_SINGLETONS = new HashSet<>();

    @BeforeClass
    public static void initializeSingletons() {
        EJB_SINGLETONS.addAll(getAllClassesByPackageAndAnnotation(PODM_PACKAGE, javax.ejb.Singleton.class));
        CDI_SINGLETONS.addAll(getAllClassesByPackageAndAnnotation(PODM_PACKAGE, javax.inject.Singleton.class));
    }

    @Test
    public void cdiSingletonsMustNotBeUsed() {
        List<String> errors = CDI_SINGLETONS.stream()
            .map(cdiSingletonClass -> cdiSingletonClass.getName() + ": javax.inject.Singleton must not be used, use javax.ejb.Singleton instead.")
            .collect(toList());

        failWithMessageIfAnyError(errors, "CDI Singletons");
    }

    @Test
    public void ejbSingletonsMustHaveExplicitAnnotationsOnTheirPublicMethods() {
        List<String> errors = new ArrayList<>();

        EJB_SINGLETONS.stream()
            .map(this::checkIfEjbSingletonHasExplicitAnnotationsOnItsPublicMethods)
            .forEach(errors::addAll);

        failWithMessageIfAnyError(errors, "EJB Singletons");
    }

    private List<String> checkIfEjbSingletonHasExplicitAnnotationsOnItsPublicMethods(Class<?> ejbSingletonClass) {
        List<String> errors = new ArrayList<>();

        List<Method> publicMethods = extractPublicMethodsFromClass(ejbSingletonClass);
        for (Method publicMethod : publicMethods) {
            if (!publicMethod.isAnnotationPresent(javax.ejb.Lock.class)) {
                errors.add(getClassWithMethodPrefix(ejbSingletonClass, publicMethod) + ": must have explicit @javax.ejb.Lock annotation");
            }
            if (!publicMethod.isAnnotationPresent(javax.ejb.AccessTimeout.class)) {
                errors.add(getClassWithMethodPrefix(ejbSingletonClass, publicMethod) + ": must have explicit @javax.ejb.AccessTimeout annotation");
            }
            if (!publicMethod.isAnnotationPresent(javax.transaction.Transactional.class)) {
                errors.add(getClassWithMethodPrefix(ejbSingletonClass, publicMethod) + ": must have explicit @javax.transaction.Transactional annotation");
            }
        }

        return errors;
    }

    private List<Method> extractPublicMethodsFromClass(Class<?> clazz) {
        return stream(clazz.getDeclaredMethods())
            .filter(method -> isPublic(method.getModifiers()))
            .collect(toList());
    }

    private String getClassWithMethodPrefix(Class<?> clazz, Method method) {
        return clazz.getName() + "." + method.getName() + "(...)";
    }
}
