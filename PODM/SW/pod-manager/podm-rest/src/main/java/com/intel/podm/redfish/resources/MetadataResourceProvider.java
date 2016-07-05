/*
 * Copyright (c) 2016 Intel Corporation
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

package com.intel.podm.redfish.resources;

import org.w3c.dom.Document;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;
import org.xml.sax.SAXException;

import javax.ws.rs.GET;
import javax.ws.rs.Produces;
import javax.ws.rs.core.Response;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;
import javax.xml.transform.Result;
import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerException;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.dom.DOMSource;
import javax.xml.transform.stream.StreamResult;
import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;

import static com.intel.podm.redfish.resources.MetadataResourceProvider.XmlUtils.getDocument;
import static com.intel.podm.redfish.resources.MetadataResourceProvider.XmlUtils.transformToStream;
import static com.intel.podm.rest.error.PodmExceptions.notFound;
import static javax.ws.rs.core.MediaType.APPLICATION_XML_TYPE;
import static javax.ws.rs.core.Response.ok;
import static org.apache.commons.io.FilenameUtils.getName;

public class MetadataResourceProvider {
    private final String resourceName;

    public MetadataResourceProvider(String resourceName) {
        this.resourceName = resourceName;
    }

    @GET
    @Produces
    public Response getMetadata() {
        if (!getName(resourceName).equals(resourceName)) {
            throw notFound();
        }

        Document xmlDocument = getXmlDocument();
        try (InputStream transformedInputStream = transformToStream(xmlDocument)) {
            return ok(transformedInputStream)
                .type(APPLICATION_XML_TYPE)
                .build();
        } catch (IOException e) {
            throw notFound();
        }
    }

    private Document getXmlDocument() {
        InputStream inputStream = this.getClass().getClassLoader().getResourceAsStream(resourceName);
        if (inputStream == null) {
            throw notFound();
        }

        Document xmlDocument = getDocument(inputStream);
        try {
            inputStream.close();
        } catch (IOException e) {
            throw notFound();
        }
        updateUris(xmlDocument);
        return xmlDocument;
    }

    private void updateUris(Document xmlDocument) {
        NodeList list = xmlDocument.getElementsByTagName("edmx:Reference");
        for (int i = 0; i < list.getLength(); i++) {
            Node item = list.item(i);
            Node uri = item.getAttributes().getNamedItem("Uri");
            uri.setTextContent("/redfish/v1/metadata/" + uri.getTextContent());
        }
    }

    static class XmlUtils {
        static InputStream transformToStream(Document xmlDocument) {
            Transformer transformer;
            DOMSource source = new DOMSource(xmlDocument);
            ByteArrayOutputStream outputStream = new ByteArrayOutputStream();
            Result result = new StreamResult(outputStream);
            try {
                transformer = TransformerFactory.newInstance().newTransformer();
                transformer.transform(source, result);
            } catch (TransformerException e) {
                throw new RuntimeException(e);
            }
            return new ByteArrayInputStream(outputStream.toByteArray());
        }

        static Document getDocument(InputStream inputStream) {
            try {
                return DocumentBuilderFactory.newInstance().newDocumentBuilder().parse(inputStream);
            } catch (SAXException | IOException | ParserConfigurationException e) {
                throw new RuntimeException(e);
            }
        }
    }
}
