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
import re
import string
import json
from hashlib import md5
from collections import defaultdict

from cts_core.commons.odata_id_generalized_mapper import OdataIdGeneralizedMapper

def compute_error_code(input_string):
    hash_code = md5(input_string.encode())
    hex_string = hash_code.hexdigest()
    return hex_string


class ErrorMessageFormatter(string.Formatter):
    def __init__(self, severity):
        self.severity = severity

    def format_field(self, value, format_spec):
        if format_spec == 'id':
            return 'url='+str(value)
        elif format_spec == 'exception':
            return "{type}:{message}".format(type=str(type(value)), message=value.message)
        elif format_spec == 'stacktrace':
            return '{}::'.format(self.severity) + value.replace('\n', '\n{}::'.format(self.severity))
        elif format_spec == 'response_body':
            return ("\n" + json.dumps(value, indent=4) + "\n").replace('\n', '\n{}::'.format(self.severity))
        elif format_spec == 'ignore':
            return str(value)
        else:
            return super(ErrorMessageFormatter, self).format_field(value, format_spec)


class ErrorIdFormatter(string.Formatter):
    def format_field(self, value, format_spec):
        if format_spec == 'id':
            return self.escape_url(str(value))
        elif format_spec == 'exception':
            return repr(type(value))
        elif format_spec == 'stacktrace':
            return "" # do not use stacktrace when computing error id
        elif format_spec == 'response_body':
            return "" # do not use response body when computing error id
        elif format_spec == 'ignore':
            return "" # ignore when computing hash code
        else:
            return super(ErrorIdFormatter, self).format_field(value, format_spec)


    def escape_url(self, value):
        """
        :type value: basestring
        :rtype: basestring
        """
        return OdataIdGeneralizedMapper.get_generalized_url(value)


def build_message(severity, message, arguments):
    fmt = ErrorMessageFormatter(severity)
    try:
        return "{}::".format(severity) + fmt.vformat(message, {}, arguments)
    except Exception as error:
        return "WARNING::Unable to format message <%s>; error: %s" \
            % (message, repr(error))


def build_message_id(severity, message, arguments):
    fmt = ErrorIdFormatter()
    try:
        code_input = sanitize("{}:".format(severity) + fmt.vformat(message, {}, arguments))
        return compute_error_code(code_input)
    except:
        return ""


def sanitize(message):
    return re.sub(r'\s+', '_', message)


def cts_log(severity, fmt, **kwargs):
    if ErrorMute.mute:
        return

    arguments = defaultdict(lambda: '[UNKNOWN]', **kwargs)

    message_for_user = build_message(severity, fmt, arguments)
    message_id = build_message_id(severity, fmt, arguments)

    print "{message_for_user}; [#Id={message_id}]".format(**locals())


def cts_error(fmt, **kwargs):
    cts_log("ERROR", fmt, **kwargs)


def cts_warning(fmt, **kwargs):
    cts_log("WARNING", fmt, **kwargs)


def cts_message(fmt, **kwargs):
    if ErrorMute.mute:
        return
    msg = fmt.format(**kwargs)
    print "MESSAGE::{msg}".format(msg=msg)


def cts_color_message(fmt, font_color="DEFAULT", **kwargs):
    font_effect = {
        "DEFAULT": '\033[0m',
        "HEADER": '\033[95m',
        "BLUE": '\033[94m',
        "WARNING": '\033[93m',
        "GREEN": '\033[92m',
        "FAIL": '\033[91m',
        "YELLOW": '\033[33m',
        "LIGHT_BLUE": '\033[34m',
        "BOLD": '\033[1m',
    }

    if ErrorMute.mute:
        return
    msg = fmt.format(**kwargs)
    print "MESSAGE::{color}{msg}{end_of_color}".format(
        color=font_effect[font_color],
        msg=msg,
        end_of_color=font_effect["DEFAULT"]
    )


def get_ok():
    return "\033[92m[ OK ]\033[0m"


def get_fail():
    return "\033[91m[FAIL]\033[0m"


def get_warning():
    return "\033[93m[WARN]\033[0m"


def get_info():
    return "\033[94m[INFO]\033[0m"


class ErrorMute:
    mute = False

    def __enter__(self):
        ErrorMute.mute = True

    def __exit__(self, exc_type, exc_val, exc_tb):
        ErrorMute.mute = False
