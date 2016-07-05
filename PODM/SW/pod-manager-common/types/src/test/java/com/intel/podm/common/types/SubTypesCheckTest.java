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

package com.intel.podm.common.types;

import org.testng.annotations.Test;

import java.io.File;
import java.io.IOException;
import java.net.URL;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Enumeration;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

import static java.util.Optional.ofNullable;
import static org.testng.Assert.assertEquals;

public class SubTypesCheckTest {
    private static final String TYPES_PACKAGE_NAME = EnumeratedType.class.getPackage().getName();

    @Test
    public void checkEnumeratedTypeSubTypes() throws ClassNotFoundException, IOException {
        Set<Class<? extends EnumeratedType>> subTypes = getAllSubTypesOf(EnumeratedType.class, TYPES_PACKAGE_NAME);
        assertEquals(subTypes, EnumeratedType.SUB_TYPES);
    }

    private <T> Set<Class<? extends T>> getAllSubTypesOf(final Class<T> classType, String packageFilter) throws ClassNotFoundException, IOException {
        ClassLoader classLoader = Thread.currentThread().getContextClassLoader();
        if (classLoader == null) {
            throw new SecurityException("No class loader found");
        }

        Set<Class> classes = new HashSet<>();
        String path = packageFilter.replace('.', '/');
        Enumeration<URL> resources = classLoader.getResources(path);
        Set<File> dirs = new HashSet<>();
        while (resources.hasMoreElements()) {
            URL resource = resources.nextElement();
            dirs.add(new File(resource.getFile()));
        }

        for (File directory : dirs) {
            classes.addAll(findClasses(directory, packageFilter));
        }

        return getSubclasses(classType, classes);
    }

    private Set<Class> findClasses(File directory, String packageName) throws ClassNotFoundException {
        String dot = ".";
        String classExt = ".class";

        Set<Class> classes = new HashSet<>();
        List<File> files = listFilesInDir(directory);
        for (File file : files) {
            if (file.isDirectory()) {
                if (!file.getName().contains(dot)) {
                    classes.addAll(findClasses(file, packageName + dot + file.getName()));
                }
            } else if (file.getName().endsWith(classExt)) {
                classes.add(Class.forName(packageName + dot + file.getName().substring(0, file.getName().length() - classExt.length())));
            }
        }
        return classes;
    }

    private List<File> listFilesInDir(File directory) {
        List<File> files = new ArrayList<>();

        if (!directory.exists()) {
            return files;
        }

        return ofNullable(directory.listFiles()).map(Arrays::asList).orElse(files);
    }

    private <T> Set<Class<? extends T>> getSubclasses(final Class<T> classType, Set<Class> classes) {
        Set<Class<? extends T>> subclasses = new HashSet<>();

        for (Class clazz : classes) {
            if (classType.isAssignableFrom(clazz) && !classType.equals(clazz)) {
                subclasses.add(clazz);
            }
        }

        return subclasses;
    }
}
