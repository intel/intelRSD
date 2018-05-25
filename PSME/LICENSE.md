# Copyrights #

Copyright (c) 2015-2018 Intel Corporation

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

# Bundled libraries #

## Optional library ##

This product bundles Andrzej Krzemieński Optional implementation for C++11,
which is available under  Boost Software License, Version 1.0.
For details, see [common/agent-framework/include/agent-framework/module/utils/optional/optional-lib/LICENSE_1_0.txt]

## JSON-CXX library ##

This product bundles json-cxx library, which is available under a BSD 3-Clause license.
For details, see [common/json-cxx/LICENSE].

## JSON RPC ##
This product bundles JSON RPC library, which is available under MIT license.
For details, see [common/json-rpc/LICENSE] and [common/agent-framework/include/agent-framework/validators/LICENSE].

## MD5 library ##

This product bundles md5 library, which is available under the following license:

@verbatim
This is an OpenSSL-compatible implementation of the RSA Data Security, Inc.
MD5 Message-Digest Algorithm (RFC 1321).

Homepage:
http://openwall.info/wiki/people/solar/software/public-domain-source-code/md5

Author:
Alexander Peslyak, better known as Solar Designer <solar at openwall.com>

This software was written by Alexander Peslyak in 2001.  No copyright is
claimed, and the software is hereby placed in the public domain.
In case this attempt to disclaim copyright and place the software in the
public domain is deemed null and void, then the software is
Copyright (c) 2001 Alexander Peslyak and it is hereby released to the
general public under the following terms:

Redistribution and use in source and binary forms, with or without
modification, are permitted.

There's ABSOLUTELY NO WARRANTY, express or implied.


This differs from Colin Plumb's older public domain implementation in that
no exactly 32-bit integer data type is required (any 32-bit or wider
unsigned integer data type will do), there's no compile-time endianness
configuration, and the function prototypes match OpenSSL's.  No code from
Colin Plumb's implementation has been reused; this comment merely compares
the properties of the two independent implementations.

The primary goals of this implementation are portability and ease of use.
It is meant to be fast, but not as fast as possible.  Some known
optimizations are not included to reduce source code size and avoid
compile-time configuration.
@endverbatim

For details, see [common/md5/src/md5.c].

## Rest Server library ##

This product bundles rest server library, which is available under the following license:

@verbatim
Copyright (C) 2014 MediaSift Ltd.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
@endverbatim

For details, see [application/include/psme/rest/server/multiplexer.hpp].

## OOSP UUID ##
This product bundles OSSP UUID library, which is available under MIT license.
For details, see [common/uuid/uuid-1.6.2/README]
