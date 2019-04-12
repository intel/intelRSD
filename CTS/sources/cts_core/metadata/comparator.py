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
import itertools
from cStringIO import StringIO
from tempfile import NamedTemporaryFile

THIS = False
OTHER = True


class Comparator:
    EQUAL = 0
    NON_EQUAL = 1
    COLUMN_WIDTH = 60
    FMT = "{0:%s}|  {1:%s}\n" % (COLUMN_WIDTH, COLUMN_WIDTH)

    def __init__(self, level=0):
        self.left_file = StringIO()
        self.right_file = StringIO()
        self._result = Comparator.EQUAL
        self._level = level

        self.left_ext_file = StringIO()
        self.right_ext_file = StringIO()

    @property
    def result(self):
        return self._result

    def set_not_equal(self):
        self._result = Comparator.NON_EQUAL
        return self

    def flush_to_disk(self):
        left = NamedTemporaryFile()
        left.write(self.left_file.getvalue())
        left.flush()

        right = NamedTemporaryFile()
        right.write(self.right_file.getvalue())
        right.flush()

        return left, right

    @staticmethod
    def _format_row(left, right):
        left = "" if left is None else left
        right = "" if right is None else right
        return Comparator.FMT.format(left, right)

    def get_side_by_side(self, label=None, label_other=None):
        left = self.left_file.getvalue().split('\n')
        right = self.right_file.getvalue().split('\n')

        out = self._format_row(label, label_other)
        for line in itertools.izip_longest(left, right):
            out += self._format_row(*line)
        return out

    def message_left(self, fmt, **kwargs):
        self.message(THIS, fmt, **kwargs)

    def message_right(self, fmt, **kwargs):
        self.message(OTHER, fmt, **kwargs)

    def message_both(self, fmt, **kwargs):
        self.message_left(fmt, **kwargs)
        self.message_right(fmt, **kwargs)

    def separator(self):
        self.message_both('-' * Comparator.COLUMN_WIDTH)

    @property
    def indent(self):
        return "  " * self._level

    def message(self, column, fmt, **kwargs):
        extended = self.left_ext_file if column == THIS else self.right_ext_file
        extended.write(fmt + "\n")

        outlet = self.left_file if column == THIS else self.right_file
        try:
            msg = (self.indent + fmt.format(**kwargs))[:Comparator.COLUMN_WIDTH]
        except IndexError:
            msg = (self.indent + fmt)[:Comparator.COLUMN_WIDTH]
        outlet.write(msg + "\n")

    def merge_result(self, *other_comparators):
        for comparator in other_comparators:
            if comparator.result == Comparator.NON_EQUAL:
                self._result = Comparator.NON_EQUAL
                self.left_file.write(comparator.left_file.getvalue())
                self.right_file.write(comparator.right_file.getvalue())

                self.left_ext_file.write(comparator.left_ext_file.getvalue())
                self.right_ext_file.write(comparator.right_ext_file.getvalue())
        return self
