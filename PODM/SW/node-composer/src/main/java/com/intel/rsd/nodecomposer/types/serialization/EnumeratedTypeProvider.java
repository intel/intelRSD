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

package com.intel.rsd.nodecomposer.types.serialization;

import com.intel.rsd.nodecomposer.types.AutoDeserializing;
import com.intel.rsd.nodecomposer.types.EnumeratedType;
import lombok.experimental.UtilityClass;
import org.springframework.data.util.AnnotatedTypeScanner;

import java.util.Set;

import static java.lang.reflect.Modifier.isAbstract;
import static java.util.stream.Collectors.toSet;

@UtilityClass
public class EnumeratedTypeProvider {
    private static Set<Class<? extends EnumeratedType>> subTypes = scanForSubTypes();

    private static Set<Class<? extends EnumeratedType>> scanForSubTypes() {
        AnnotatedTypeScanner scanner = new AnnotatedTypeScanner(AutoDeserializing.class);
        String scannedPackageName = EnumeratedType.class.getPackage().getName();

        return scanner.findTypes(scannedPackageName).stream()
            .filter(clazz -> !isAbstract(clazz.getModifiers()))
            .filter(EnumeratedType.class::isAssignableFrom)
            .map(clazz -> (Class<? extends EnumeratedType>) clazz)
            .collect(toSet());
    }

    public static Set<Class<? extends EnumeratedType>> getSubTypes() {
        return subTypes;
    }
}
