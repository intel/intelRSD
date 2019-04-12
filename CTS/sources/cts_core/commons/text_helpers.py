"""
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2019 Intel Corporation
 *
 * @copyright
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * @copyright
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * @copyright
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @section DESCRIPTION
"""
from difflib import SequenceMatcher


def split_at_column(text, column):
    res = []
    while text:
        idx = -1
        if len(text) > column:
            idx = text.rfind(' ', 0, column)
            if idx < 0:
                idx = text.find(' ', column)

        if idx > 0:
            res.append(text[:idx+1])
            text = text[idx+1:]
        else:
            res.append(text)
            break
    return res


def get_parent_odata_id(odata_id):
    if odata_id.endswith("/"):
        odata_id = odata_id[:-1]
    return odata_id.rsplit("/", 1)[0]


def to_multiline(text, column, join_string):
    return join_string.join(split_at_column(text, column))


def are_similar(a, b):
    return SequenceMatcher(None, a.lower(), b.lower()).ratio() > 0.9


def try_suggest_other_property_names(name, candidates):
    similar_names = set([unused for unused in candidates if are_similar(name, unused)])
    if len(similar_names):
        print 'DEBUG::Hint: Schema defines other, not used, but similar properties: [{other}]' \
            .format(other=', '.join(similar_names))
