/*
 * Copyright (c) 2017-2019 Intel Corporation
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

package com.intel.rsd.resourcemanager.layers.unifier;

import com.google.common.collect.ImmutableMultiset;
import com.intel.rsd.resourcemanager.layers.unifier.patterns.UnificationPattern;
import com.intel.rsd.resourcemanager.layers.unifier.patterns.UnificationPatternFactory;
import lombok.val;
import org.antlr.v4.runtime.ANTLRErrorListener;
import org.antlr.v4.runtime.BaseErrorListener;
import org.antlr.v4.runtime.RecognitionException;
import org.antlr.v4.runtime.Recognizer;
import org.springframework.core.io.ClassPathResource;
import org.testng.annotations.Test;

import java.io.IOException;
import java.util.Collection;
import java.util.stream.Collectors;

import static com.google.common.base.Joiner.on;
import static com.google.common.collect.Multiset.Entry;
import static java.text.MessageFormat.format;
import static java.util.stream.Collectors.toList;
import static org.testng.Assert.assertTrue;
import static org.testng.Assert.fail;

@SuppressWarnings("checkstyle:ParameterNumber")
public class UnificationPatternsDefinitionTest {
    private static final ANTLRErrorListener ANTLR_ERROR_LISTENER = new BaseErrorListener() {
        @Override
        public void syntaxError(Recognizer<?, ?> recognizer, Object offendingSymbol, int line, int charPositionInLine, String msg, RecognitionException e) {
            fail("line " + line + ":" + charPositionInLine + " " + msg);
        }
    };

    private Collection<UnificationPattern> unificationPatterns;

    @Test
    public void testPatternSyntax() throws IOException {
        val resource = new ClassPathResource("patterns");
        val unificationPatternFactory = new UnificationPatternFactory();
        unificationPatterns = unificationPatternFactory.readFromStream(resource.getInputStream(), ANTLR_ERROR_LISTENER);
    }

    @Test(dependsOnMethods = {"testPatternSyntax"})
    public void testDuplicateEntries() {
        val unificationPatternsStrings = unificationPatterns.stream()
            .map(UnificationPattern::toString)
            .collect(toList());

        val duplicates = ImmutableMultiset.copyOf(unificationPatternsStrings).entrySet().stream()
            .filter(entry -> entry.getCount() > 1)
            .map(Entry::getElement)
            .collect(Collectors.toList());

        val duplicatesMultilineString = on("\n").join(duplicates);

        assertTrue(duplicates.isEmpty(), format("Patterns file contains duplicate entries: \n{0}\n", duplicatesMultilineString));
    }
}
