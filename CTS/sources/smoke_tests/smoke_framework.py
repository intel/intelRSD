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

from subprocess import Popen, PIPE


class SmokeFramework():

    @staticmethod
    def get_output_from_commnad(cmd):
        """
        This function allow to run subprocess with specific cmd + args
        :param cmd: CMD + ARGS as String. Will be split() inside
        :return: output, error and return code
        """
        output, err, rc = None, None, None

        try:
            p = Popen(cmd.split(), stdin=PIPE, stdout=PIPE, stderr=PIPE)
            output, err = p.communicate()
            rc = p.returncode
        except IOError as e:
            print "ERROR:: Something went wrong with external subprocess\n%s" % e

        return output, err, rc
