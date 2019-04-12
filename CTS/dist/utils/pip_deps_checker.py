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

try:
    import pip
except ImportError:
    print "Please install pip"
    sys.exit(1)

try:  # for pip >= 10
    from pip._internal.req import parse_requirements
    from pip._internal.utils.misc import get_installed_version
except ImportError:  # for pip <= 9.0.3
    from pip.req import parse_requirements
    from pip.utils import get_installed_version


def get_required():
    required_dependencies = []
    for e in ['requirements.txt', 'requirements-no-deps.txt']:
        install_requirements = parse_requirements(e, session='hack')
        required_dependencies.extend([req.req for req in install_requirements])

    return required_dependencies


def show_user_cmd_for_manual_install(missing):
    print "\nPlease, install pip dependencies manually or try using proxy option in installer:\nsudo -H pip install {what}".format(
        what=' '.join(map(str, missing))
    )


def is_installed(requirement):
    try:
        req_name, req_ver = str(requirement).split('==')
    except ValueError:
        print "ERROR:: Requirement specifier not supported: {req}. Installer supports only specifiers with " \
              "'=='".format(req=str(requirement))
        return True

    installed_version = get_installed_version(req_name)
    return installed_version == req_ver


if __name__ == "__main__":
    required = get_required()
    not_met = [req for req in required if not is_installed(req)]
    if not_met:
        show_user_cmd_for_manual_install(not_met)
        sys.exit(1)
