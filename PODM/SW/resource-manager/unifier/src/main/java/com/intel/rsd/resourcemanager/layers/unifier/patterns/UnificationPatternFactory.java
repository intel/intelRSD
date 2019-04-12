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

package com.intel.rsd.resourcemanager.layers.unifier.patterns;

import com.intel.rsd.resourcemanager.layers.unifier.patterns.matching.ConstSegmentMatchingPattern;
import com.intel.rsd.resourcemanager.layers.unifier.patterns.matching.ExistingResourceMatchingPattern;
import com.intel.rsd.resourcemanager.layers.unifier.patterns.matching.MatchingPattern;
import com.intel.rsd.resourcemanager.layers.unifier.patterns.matching.SegmentMatchingPattern;
import com.intel.rsd.resourcemanager.layers.unifier.patterns.matching.VariableSegmentMatchingPattern;
import com.intel.rsd.resourcemanager.layers.unifier.patterns.parsing.UnificationPatternBaseVisitor;
import com.intel.rsd.resourcemanager.layers.unifier.patterns.parsing.UnificationPatternLexer;
import com.intel.rsd.resourcemanager.layers.unifier.patterns.parsing.UnificationPatternParser;
import com.intel.rsd.resourcemanager.layers.unifier.patterns.parsing.UnificationPatternParser.PathSegmentsContext;
import com.intel.rsd.resourcemanager.layers.unifier.patterns.rewriting.AbsolutePathSegment;
import com.intel.rsd.resourcemanager.layers.unifier.patterns.rewriting.ArgumentSubstitutionPattern;
import com.intel.rsd.resourcemanager.layers.unifier.patterns.rewriting.CalculableSegment;
import com.intel.rsd.resourcemanager.layers.unifier.patterns.rewriting.ConstSegmentRewritingPattern;
import com.intel.rsd.resourcemanager.layers.unifier.patterns.rewriting.ConstantPathSegment;
import com.intel.rsd.resourcemanager.layers.unifier.patterns.rewriting.ExistingResourceSubstitution;
import com.intel.rsd.resourcemanager.layers.unifier.patterns.rewriting.PathSegment;
import com.intel.rsd.resourcemanager.layers.unifier.patterns.rewriting.RelativePathSegment;
import com.intel.rsd.resourcemanager.layers.unifier.patterns.rewriting.RewritingPattern;
import com.intel.rsd.resourcemanager.layers.unifier.patterns.rewriting.SegmentRewritingPattern;
import lombok.val;
import org.antlr.v4.runtime.ANTLRErrorListener;
import org.antlr.v4.runtime.CharStreams;
import org.antlr.v4.runtime.CommonTokenStream;
import org.antlr.v4.runtime.tree.TerminalNode;

import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.Collection;

import static java.util.stream.Collectors.joining;
import static java.util.stream.Collectors.toList;

@SuppressWarnings({"checkstyle:MethodCount", "checkstyle:ClassDataAbstractionCoupling"})
public final class UnificationPatternFactory {
    private final Visitor visitor = new Visitor();

    public UnificationPattern create(String pattern) {
        val lexer = new UnificationPatternLexer(CharStreams.fromString(pattern));
        val tokenStream = new CommonTokenStream(lexer);
        val parser = new UnificationPatternParser(tokenStream);

        return (UnificationPattern) visitor.visit(parser.unificationPattern());
    }

    public MatchingPattern createMatchingPattern(String pattern) {
        val lexer = new UnificationPatternLexer(CharStreams.fromString(pattern));
        val tokenStream = new CommonTokenStream(lexer);
        val parser = new UnificationPatternParser(tokenStream);

        return visitor.visitMatchingPattern(parser.matchingPattern());
    }

    public Collection<UnificationPattern> readFromStream(InputStream stream, ANTLRErrorListener... errorListeners) throws IOException {
        val lexer = new UnificationPatternLexer(CharStreams.fromStream(stream));
        val tokenStream = new CommonTokenStream(lexer);
        val parser = new UnificationPatternParser(tokenStream);

        for (ANTLRErrorListener errorListener : errorListeners) {
            lexer.addErrorListener(errorListener);
            parser.addErrorListener(errorListener);
        }

        return visitor.visitUnificationPatternFile(parser.unificationPatternFile());
    }

    @SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
    static final class Visitor extends UnificationPatternBaseVisitor<Object> {
        @Override
        public Collection<UnificationPattern> visitUnificationPatternFile(UnificationPatternParser.UnificationPatternFileContext ctx) {
            return ctx
                .unificationPattern()
                .stream()
                .map(this::visitUnificationPattern)
                .collect(toList());
        }

        @Override
        public UnificationPattern visitUnificationPattern(UnificationPatternParser.UnificationPatternContext ctx) {
            val matchingPattern = visitMatchingPattern(ctx.matchingPattern());
            val rewritingPattern = visitRewritingPattern(ctx.rewritingPattern());

            return new UnificationPattern(matchingPattern, rewritingPattern);
        }

        @Override
        public MatchingPattern visitMatchingPattern(UnificationPatternParser.MatchingPatternContext ctx) {
            val segments = ctx
                .segmentMatchingPattern()
                .stream()
                .map(this::visit)
                .map(SegmentMatchingPattern.class::cast)
                .collect(toList());

            val existingResource = visitExistingResourcePattern(ctx.existingResourcePattern());
            return new MatchingPattern(existingResource, segments);
        }

        @Override
        public RewritingPattern visitRewritingPattern(UnificationPatternParser.RewritingPatternContext ctx) {
            val segments = ctx
                .segmentRewritingPattern()
                .stream()
                .map(this::visit)
                .map(SegmentRewritingPattern.class::cast)
                .collect(toList());

            val existingResourceSubstitution = visitExistingResourceSubstitution(ctx.existingResourceSubstitution());
            return new RewritingPattern(existingResourceSubstitution, segments);

        }

        @Override
        public ExistingResourceMatchingPattern visitExistingResourcePattern(UnificationPatternParser.ExistingResourcePatternContext ctx) {
            if (ctx == null) {
                return null;
            }

            val segments = ctx
                .segmentMatchingPattern()
                .stream()
                .map(this::visit)
                .map(SegmentMatchingPattern.class::cast)
                .collect(toList());

            return new ExistingResourceMatchingPattern(segments);
        }

        @Override
        public Object visitConstSegmentMatchingPattern(UnificationPatternParser.ConstSegmentMatchingPatternContext ctx) {
            return new ConstSegmentMatchingPattern(ctx.getText());
        }

        @Override
        public Object visitVariableSegmentMatchingPattern(UnificationPatternParser.VariableSegmentMatchingPatternContext ctx) {
            return new VariableSegmentMatchingPattern();
        }

        @Override
        public ExistingResourceSubstitution visitExistingResourceSubstitution(UnificationPatternParser.ExistingResourceSubstitutionContext ctx) {
            return ctx == null
                ? null
                : new ExistingResourceSubstitution();
        }

        @Override
        public Object visitArgumentSubstitutionPattern(UnificationPatternParser.ArgumentSubstitutionPatternContext ctx) {
            PathSegmentsContext currentPathSegments = ctx.pathSegments();
            val result = new ArrayList<PathSegment>();
            while (currentPathSegments != null) {
                result.add((PathSegment) this.visit(currentPathSegments.abstractPathSegment()));
                currentPathSegments = currentPathSegments.pathSegments();
            }
            return new ArgumentSubstitutionPattern(result);
        }

        @Override
        public Object visitConstSegmentRewritingPattern(UnificationPatternParser.ConstSegmentRewritingPatternContext ctx) {
            return new ConstSegmentRewritingPattern(ctx.getText());
        }

        @Override
        public String visitPathSegment(UnificationPatternParser.PathSegmentContext ctx) {
            return ctx.SimpleIdentifier().stream()
                .map(TerminalNode::getText)
                .collect(joining("/", "/", ""));
        }

        @Override
        public String visitPathFragment(UnificationPatternParser.PathFragmentContext ctx) {
            return ctx.SimpleIdentifier().stream()
                .map(TerminalNode::getText)
                .collect(joining("/", "/", ""));
        }

        @Override
        public Object visitCalculableSegment(UnificationPatternParser.CalculableSegmentContext ctx) {
            val pathFragments = ctx.pathFragment().stream().map(this::visitPathFragment).collect(toList());
            return new CalculableSegment(
                ctx.getText(),
                ctx.transformingFunctionName().getText(),
                pathFragments
            );
        }

        @Override
        public Object visitRelativeResolvablePath(UnificationPatternParser.RelativeResolvablePathContext ctx) {
            return new RelativePathSegment(visitPathFragment(ctx.pathFragment()));
        }


        @Override
        public Object visitAbsoluteResolvablePath(UnificationPatternParser.AbsoluteResolvablePathContext ctx) {
            return new AbsolutePathSegment(visitPathSegment(ctx.pathSegment()), ctx.pathFragment().getText());
        }

        @Override
        public ConstantPathSegment visitConstFragment(UnificationPatternParser.ConstFragmentContext ctx) {
            return new ConstantPathSegment(ctx.SimpleIdentifier().getText());
        }
    }
}
