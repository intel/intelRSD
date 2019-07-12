/*
 * Copyright (c) 2019 Intel Corporation
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

import com.intel.rsd.nodecomposer.utils.ClassGatherer;
import lombok.val;
import org.springframework.transaction.annotation.Propagation;
import org.testng.annotations.Test;

import javax.transaction.Transactional;
import javax.transaction.Transactional.TxType;
import java.lang.reflect.Method;
import java.util.Collection;
import java.util.Set;
import java.util.stream.Stream;

import static java.util.Arrays.asList;
import static java.util.stream.Collectors.toList;
import static org.apache.commons.lang3.reflect.MethodUtils.getMethodsListWithAnnotation;
import static org.assertj.core.api.Assertions.assertThat;

public class CheckTransactionsDeclarations {

    private static final Collection<TxType> JAVAX_TX_TYPES_TO_BE_CHECKED = asList(TxType.REQUIRES_NEW, TxType.REQUIRED);

    private static final Collection<Propagation> SPRING_TX_TYPES_TO_BE_CHECKED = asList(Propagation.REQUIRES_NEW, Propagation.REQUIRED);

    private static final Set<Class<?>> CLASSES_TO_CHECK = ClassGatherer.getAllClassesByPackage("com.intel.rsd.nodecomposer.*");

    @Test
    public void checkJavaxTransactions() {

        val transactionalMethodsToBeChecked = CLASSES_TO_CHECK.stream().map(clazz -> getMethodsListWithAnnotation(clazz, javax.transaction.Transactional.class))
            .flatMap(Collection::stream)
            .filter(method -> JAVAX_TX_TYPES_TO_BE_CHECKED.contains(method.getAnnotation(javax.transaction.Transactional.class).value()))
            .collect(toList());

        for (Method transactionalMethod : transactionalMethodsToBeChecked) {
            val transactionalAnnotation = transactionalMethod.getAnnotation(Transactional.class);

            val exceptionTypes = Stream.of(transactionalMethod.getExceptionTypes())
                .filter(exceptionClass -> !RuntimeException.class.isAssignableFrom(exceptionClass))
                .collect(toList());

            val declaredRollbackOnExceptions = asList(transactionalAnnotation.rollbackOn());

            assertThat(declaredRollbackOnExceptions.containsAll(exceptionTypes))
                    .as("Exception(s): %s should be declared as @Transactional(rollbackFor=...) for %s", exceptionTypes, transactionalMethod)
                    .isTrue();

        }
    }

    @Test
    public void checkSpringTransactions() {
        val transactionalMethodsToBeChecked = CLASSES_TO_CHECK.stream()
            .map(clazz -> getMethodsListWithAnnotation(clazz, org.springframework.transaction.annotation.Transactional.class))
            .flatMap(Collection::stream)
            .filter(method -> SPRING_TX_TYPES_TO_BE_CHECKED.contains(
                method.getAnnotation(org.springframework.transaction.annotation.Transactional.class).propagation())
            )
            .collect(toList());

        for (Method transactionalMethod : transactionalMethodsToBeChecked) {
            val transactionalAnnotation = transactionalMethod.getAnnotation(org.springframework.transaction.annotation.Transactional.class);

            val exceptionTypes = Stream.of(transactionalMethod.getExceptionTypes())
                .filter(exceptionClass -> !RuntimeException.class.isAssignableFrom(exceptionClass))
                .collect(toList());

            val declaredRollbackOnExceptions = asList(transactionalAnnotation.rollbackFor());

            assertThat(declaredRollbackOnExceptions.containsAll(exceptionTypes))
                .as("Exception(s): %s should be declared as @Transactional(rollbackFor=...) for %s", exceptionTypes, transactionalMethod)
                .isTrue();

        }
    }
}
