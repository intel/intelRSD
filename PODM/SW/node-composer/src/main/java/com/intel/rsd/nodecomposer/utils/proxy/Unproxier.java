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

package com.intel.rsd.nodecomposer.utils.proxy;

import lombok.SneakyThrows;
import org.springframework.aop.framework.Advised;

import java.util.List;

import static java.util.stream.Collectors.toList;
import static org.springframework.aop.support.AopUtils.isCglibProxy;

/**
 * Contains method allowing to determine what is underlying class of class proxied by container.
 */
public final class Unproxier {
    private Unproxier() {
    }

    public static <T> List<T> unproxy(List<T> objects) {
        return objects.stream()
            .map(Unproxier::unproxy)
            .collect(toList());
    }

    @SneakyThrows
    @SuppressWarnings({"unchecked"})
    private static <T> T unproxy(T object) {
        if (isCglibProxy(object)) {
            return (T) unproxy(((Advised) object).getTargetSource().getTarget());
        }
        return object;
    }
}
