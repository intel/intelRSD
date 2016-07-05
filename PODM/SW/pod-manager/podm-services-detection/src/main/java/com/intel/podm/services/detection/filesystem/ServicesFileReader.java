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

package com.intel.podm.services.detection.filesystem;

import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.logger.LoggerFactory;

import java.io.IOException;
import java.nio.file.Path;
import java.util.List;
import java.util.stream.Stream;

import static java.nio.file.Files.lines;
import static java.util.Collections.emptyList;
import static java.util.stream.Collectors.toList;

public class ServicesFileReader {

    private static final Logger LOGGER = LoggerFactory.getLogger(ServicesFileReader.class);

    private Path filePath;

    public ServicesFileReader(Path path) {
        this.filePath = path;
    }

    public List<String> readServicesFile() {
        try (Stream<String> stringStream = lines(filePath)) {
            return stringStream.collect(toList());
        } catch (IOException e) {
            LOGGER.d("Error while reading file: {}", filePath);
        }
        return emptyList();
    }
}
