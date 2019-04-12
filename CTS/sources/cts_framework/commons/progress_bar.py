# -*- coding: utf-8 -*-
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
import sys

MAX_LABEL_WIDTH=40

class ProgressBar:
    def __init__(self, width, num_steps):
        self.width = width
        self.num_steps = num_steps
        self.pos = 0
        self.proc = 0
        self.column = 0
        self.label = ""

    def make_progress(self):
        self.pos = min(self.pos+1, self.num_steps)
        self.proc = float(self.pos) / self.num_steps
        self.column = int(min(self.width * self.proc, self.width))
        self.draw()

    def set_label(self, label):
        self.label = label if len(label) <= MAX_LABEL_WIDTH else label[:MAX_LABEL_WIDTH-3] + '...'

    def draw(self):
        sys.stderr.write(("[{done}{notdone}] {proc:3.0f}%\t{label:"+str(MAX_LABEL_WIDTH)+"}").format(
            done='█' * self.column,
            notdone=" " * (self.width - self.column),
            proc=self.proc*100,
            label=self.label[:MAX_LABEL_WIDTH]))
        sys.stderr.flush()
        sys.stderr.write("\r")
