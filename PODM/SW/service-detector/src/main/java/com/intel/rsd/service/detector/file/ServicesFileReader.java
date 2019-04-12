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

package com.intel.rsd.service.detector.file;

import lombok.extern.slf4j.Slf4j;

import java.io.IOException;
import java.nio.file.Path;
import java.util.List;
import java.util.stream.Stream;

import static java.nio.file.Files.lines;
import static java.util.Collections.emptyList;
import static java.util.stream.Collectors.toList;

@Slf4j
public class ServicesFileReader {

    private Path filePath;

    public ServicesFileReader(Path path) {
        this.filePath = path;
    }

    public List<String> readServicesFile() {
        try (Stream<String> stringStream = lines(filePath)) {
            return stringStream.collect(toList());
        } catch (IOException e) {
            log.debug("Error while reading file: {}", filePath);
        }
        return emptyList();
    }
}
