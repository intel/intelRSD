/*
 * Copyright (c) 2016-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.rest;

import com.intel.rsd.redfish.RedfishError;
import lombok.Getter;
import lombok.extern.slf4j.Slf4j;
import lombok.val;
import org.apache.commons.validator.routines.UrlValidator;

import javax.servlet.http.HttpServletRequest;
import java.io.UnsupportedEncodingException;

import static com.intel.rsd.redfish.RedfishErrors.createRedfishErrorWithCustomMessage;
import static java.lang.String.format;
import static java.net.URLDecoder.decode;
import static java.nio.charset.StandardCharsets.UTF_8;
import static org.apache.commons.validator.routines.UrlValidator.ALLOW_LOCAL_URLS;
import static org.springframework.http.HttpStatus.BAD_REQUEST;
import static org.springframework.http.HttpStatus.URI_TOO_LONG;

@Slf4j
class HttpServletRequestValidator {
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
            val redfishError = createRedfishErrorWithCustomMessage(
                URI_TOO_LONG,
                format("Requested URI exceeds maximum URI length which is set to %s octets", URL_MAX_LENGTH_IN_BYTES)
            );
            log.warn("{}", redfishError);
            throw new HttpServletRequestValidationException(redfishError);
        }
    }

    private String getRequestLine(HttpServletRequest servletRequest) {
        return format("%s %s HTTP/1.1\n\r", servletRequest.getMethod(), servletRequest.getRequestURL());
    }

    private void validateUrlSyntax(String urlString) throws HttpServletRequestValidationException {
        UrlValidator urlValidator = new UrlValidator(ALLOW_LOCAL_URLS);
        if (!urlValidator.isValid(urlString)) {
            val redfishError = createRedfishErrorWithCustomMessage(BAD_REQUEST, format("Requested URI %s is not valid", urlString));
            log.warn("{}", redfishError);
            throw new HttpServletRequestValidationException(redfishError);
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
            val redfishError = createRedfishErrorWithCustomMessage(BAD_REQUEST, format("Requested URI %s is malformed", url));
            log.warn("{}", redfishError);
            throw new HttpServletRequestValidationException(redfishError);
        }
    }

    @Getter
    class HttpServletRequestValidationException extends Exception {
        private static final long serialVersionUID = 3689831056061145429L;
        private final RedfishError redfishError;

        HttpServletRequestValidationException(RedfishError redfishError) {
            super(redfishError.getError().getMessage());
            this.redfishError = redfishError;
        }
    }
}
