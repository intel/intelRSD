/*
 * Copyright (c) 2018-2019 Intel Corporation
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

package com.intel.rsd.podm;

import lombok.extern.slf4j.Slf4j;
import lombok.val;
import org.apache.commons.io.IOUtils;
import org.springframework.core.io.Resource;
import org.springframework.core.io.support.PathMatchingResourcePatternResolver;
import org.springframework.stereotype.Component;
import org.w3c.dom.Document;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;

import javax.annotation.PostConstruct;
import java.io.IOException;
import java.io.InputStream;
import java.nio.file.Paths;
import java.util.HashMap;
import java.util.Map;

import static com.intel.rsd.podm.XmlUtils.getDocument;
import static com.intel.rsd.podm.XmlUtils.transformToStream;
import static java.nio.charset.Charset.defaultCharset;

@Slf4j
@Component
public class MetadataResourceProvider {
    private final Map<String, String> cache = new HashMap<>();
    private final ClassLoader loader = MetadataResourceProvider.class.getClassLoader();

    @PostConstruct
    public void init() throws IOException {
        final val resolver = new PathMatchingResourcePatternResolver(loader);
        final val resources = resolver.getResources("metadata/*");

        for (Resource resource : resources) {
            final String filename = "metadata/" + resource.getFilename();
            cache.computeIfAbsent(filename, this::readResource);
        }
    }

    public String getMetadataBody(String requestUri) {
        final String resourceName = getResourceNameFromUri(requestUri);

        return cache.get(resourceName);
    }

    private String getResourceNameFromUri(String requestUri) {
        if (requestUri.startsWith("/redfish/v1/$metadata")) {
            return "metadata/$metadata.xml";
        } else {
            String fileName = Paths.get(requestUri).getFileName().toString();

            return "metadata/" + fileName;
        }
    }

    private String readResource(String resourceName) {
        try {
            final InputStream inputStream = getMetadataInputStream(resourceName);
            final String content = IOUtils.toString(inputStream, defaultCharset());

            log.debug("Cache generated for resource: {}", resourceName);

            return content;
        } catch (IOException e) {
            log.debug("Couldn't find resource: {}", resourceName);

            return null;
        }
    }

    private InputStream getMetadataInputStream(String resourceName) throws IOException {
        try (InputStream inputStream = loader.getResourceAsStream(resourceName)) {
            return transformInputStream(inputStream);
        }
    }

    private InputStream transformInputStream(InputStream inputStream) throws IOException {
        if (inputStream == null) {
            throw new IOException();
        }

        Document xmlDocument = getDocument(inputStream);
        updateUris(xmlDocument);

        try (InputStream transformedInputStream = transformToStream(xmlDocument)) {
            return transformedInputStream;
        }
    }

    private void updateUris(Document xmlDocument) {
        NodeList list = xmlDocument.getElementsByTagName("edmx:Reference");

        for (int i = 0; i < list.getLength(); i++) {
            Node item = list.item(i);
            Node uri = item.getAttributes().getNamedItem("Uri");
            uri.setTextContent("/redfish/v1/metadata/" + uri.getTextContent());
        }
    }
}
