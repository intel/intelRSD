/*
 * Copyright (c) 2016-2017 Intel Corporation
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

package com.intel.podm.tester;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.testng.TestNG;
import org.testng.xml.Parser;
import org.testng.xml.XmlSuite;
import org.xml.sax.SAXException;

import javax.xml.parsers.ParserConfigurationException;
import java.io.IOException;
import java.util.List;

@SuppressWarnings({"checkstyle:UncommentedMain"})
public final class StressTesterMain {
    private static final Logger LOGGER = LoggerFactory.getLogger(StressTesterMain.class);

    private StressTesterMain() {
    }

    public static void main(String[] args) {
        List<XmlSuite> suites = readTestListFromFile();
        TestNG testNg = new TestNG();
        testNg.setXmlSuites(suites);
        testNg.run();
    }

    private static List<XmlSuite> readTestListFromFile() {
        String testNgXml = "../config/testNG.xml";
        try {
            return (List<XmlSuite>) new Parser(testNgXml).parse();
        } catch (ParserConfigurationException | SAXException | IOException e) {
            LOGGER.error("Error reading testNG.xml file {} {}", e.getCause(), e.getMessage());
            throw new RuntimeException(e);
        }
    }
}
