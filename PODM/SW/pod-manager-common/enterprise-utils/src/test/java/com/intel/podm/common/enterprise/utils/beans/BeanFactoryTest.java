/*
 * Copyright (c) 2015 Intel Corporation
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
package com.intel.podm.common.enterprise.utils.beans;

import org.testng.annotations.Test;

import javax.enterprise.context.spi.CreationalContext;
import javax.enterprise.inject.Any;
import javax.enterprise.inject.spi.Bean;
import javax.enterprise.inject.spi.BeanManager;
import javax.enterprise.util.AnnotationLiteral;
import java.util.Set;

import static com.google.common.collect.Sets.newHashSet;
import static org.mockito.Matchers.any;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;
import static org.testng.Assert.assertEquals;

public class BeanFactoryTest {
    private static final AnnotationLiteral<Any> ANY_ANNOTATION_LITERAL = new AnnotationLiteral<Any>() {
    };

    @Test
    public void whenCreatingNewInstance_ShouldReturnObjectedInstantiatedByBeanManager() {
        BeanFactory sut = new BeanFactory();
        sut.beanManager = mock(BeanManager.class);

        Bean<?> bean = mock(Bean.class);
        when(sut.beanManager.getBeans(TestBean.class, ANY_ANNOTATION_LITERAL))
                .thenReturn(newHashSet(bean));

        TestBean expected = mock(TestBean.class);
        when(bean.create(any()))
                .thenReturn(expected);

        TestBean actual = sut.create(TestBean.class);

        assertEquals(actual, expected);
    }

    @Test(expectedExceptions = IllegalArgumentException.class,
            expectedExceptionsMessageRegExp = ".*TestBean.*is not a managed bean.*")
    public void whenCreatingNewInstanceForClassWithoutMatchingBeanForRequiredType_ShouldThrowException() {
        BeanFactory sut = new BeanFactory();
        sut.beanManager = mock(BeanManager.class);
        when(sut.beanManager.getBeans(any(), any()))
                .thenReturn(newHashSet());

        sut.create(TestBean.class);
    }

    @Test(expectedExceptions = IllegalArgumentException.class,
            expectedExceptionsMessageRegExp = ".*TestBean.*is satisfied by more than one bean.*")
    public void whenCreatingNewInstanceForClassWithMoreThanOneMatchingBeanForRequiredType_ShouldThrowException() {
        BeanFactory sut = new BeanFactory();
        sut.beanManager = mock(BeanManager.class);
        Set beanSet = mock(Set.class);
        when(sut.beanManager.getBeans(any(), any()))
                .thenReturn(beanSet);

        int numberOfMoreThanOneBean = 5;
        when(beanSet.size()).thenReturn(numberOfMoreThanOneBean);

        sut.create(TestBean.class);
    }

    @Test
    public void whenCreatingNewInstance_ShouldCreateBeanWithCorrectContext() {
        BeanFactory sut = new BeanFactory();
        sut.beanManager = mock(BeanManager.class);

        Bean<?> bean = mock(Bean.class);
        when(sut.beanManager.getBeans(TestBean.class, ANY_ANNOTATION_LITERAL))
                .thenReturn(newHashSet(bean));

        CreationalContext correctContext = mock(CreationalContext.class);

        when(sut.beanManager.createCreationalContext(bean)).thenReturn(correctContext);

        sut.create(TestBean.class);

        verify(bean).create(correctContext);
    }
}
