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

package com.intel.podm.rest;

import com.intel.podm.common.logger.Logger;
import com.intel.podm.rest.representation.json.errors.ErrorType;
import org.apache.commons.validator.routines.UrlValidator;

import javax.servlet.http.HttpServletRequest;
import java.io.UnsupportedEncodingException;

import static com.intel.podm.common.logger.LoggerFactory.getLogger;
import static com.intel.podm.rest.representation.json.errors.ErrorType.MALFORMED_URI;
import static com.intel.podm.rest.representation.json.errors.ErrorType.URI_TOO_LONG;
import static java.lang.String.format;
import static java.net.URLDecoder.decode;
import static java.nio.charset.StandardCharsets.UTF_8;
import static org.apache.commons.validator.routines.UrlValidator.ALLOW_LOCAL_URLS;

public class HttpServletRequestValidator {
    private static final Logger LOGGER = getLogger(HttpServletRequestValidator.class);
    /**
     * Request line should be maximum 8000 octets long due to RFC 7230: 3.1.1.
     * which recommends for all handling request lines of minimum 8000 octets.
     */
    private static final int URL_MAX_LENGTH_IN_BYTES = 8000;
    private static final String DEFAULT_ENCODING = "UTF-8";

    public void validateServletRequest(HttpServletRequest httpServletRequest) throws HttpServletRequestValidationException {
        validateRequestLineLength(getRequestLine(httpServletRequest));
        validateUrlSyntax(httpServletRequest.getRequestURL().toString());
        validateUrlDecodingIsPossible(httpServletRequest.getRequestURL().toString());
    }

    private void validateRequestLineLength(String requestLine) throws HttpServletRequestValidationException {
        if (requestLine.getBytes(UTF_8).length > URL_MAX_LENGTH_IN_BYTES) {
            String errorMessage = format("Requested URI exceeds maximum URI length which is set to %s octets", URL_MAX_LENGTH_IN_BYTES);
            LOGGER.w(errorMessage);
            throw new HttpServletRequestValidationException(errorMessage, URI_TOO_LONG);
        }
    }

    private String getRequestLine(HttpServletRequest servletRequest) {
        return format("%s %s HTTP/1.1\n\r", servletRequest.getMethod(), servletRequest.getRequestURL());
    }

    private void validateUrlSyntax(String urlString) throws HttpServletRequestValidationException {
        UrlValidator urlValidator = new UrlValidator(ALLOW_LOCAL_URLS);
        if (!urlValidator.isValid(urlString)) {
            String errorMessage = format("Requested URI %s is not valid", urlString);
            LOGGER.w(errorMessage);
            throw new HttpServletRequestValidationException(errorMessage, MALFORMED_URI);
        }
    }

    /**
     * Check if it is possible to decode URL. UTF-8 is a standard coding by W3C
     * URLDecode.decode() may throw exception "IllegalArgumentException" for bad encoded url
     */
    private void validateUrlDecodingIsPossible(String url) throws HttpServletRequestValidationException {
        try {
            decode(url, DEFAULT_ENCODING);
        } catch (UnsupportedEncodingException | IllegalArgumentException e) {
            String errorMessage = format("Requested URI %s is malformed", url);
            LOGGER.w(errorMessage);
            throw new HttpServletRequestValidationException(errorMessage, MALFORMED_URI);
        }
    }

    public class HttpServletRequestValidationException extends Exception {
        private final ErrorType errorType;

        public HttpServletRequestValidationException(String message, ErrorType errorType) {
            super(message);
            this.errorType = errorType;
        }

        public ErrorType getErrorType() {
            return errorType;
        }
    }
}
