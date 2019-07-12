import json
import os
import sys

from cts_framework.build_information import BuildInformation


def get_version():
    dir_path = os.path.dirname(os.path.realpath(__file__))
    with open(dir_path + '/main/VERSION', 'r') as v:
        return str(v.read())


def get_version_from_build_information():
    return BuildInformation.version


def get_detailed_information():
    major, minor, build = get_version().split('.')
    return major, minor, build


def bump_build_version(version):
    major, minor, build = version

    dir_path = os.path.dirname(os.path.realpath(__file__))
    with open(dir_path + '/main/VERSION', 'w') as v:
        i_build = int(build) + 1
        new_version = '.'.join((major, minor, str(i_build)))
        v.write(new_version)


def get_latest_from_artifactory(artifactory_input):
    software_versions = [get_version()]

    response = json.loads("{" + ' '.join(artifactory_input))
    for element in response['children']:
        if "CTS" in element['uri']:
            software_versions.append(element['uri'].replace("/CTS-", "").replace("-py2-none-any.whl", ""))
    dir_path = os.path.dirname(os.path.realpath(__file__))

    with open(dir_path + "/main/VERSION", 'w') as w:
        version = _get_highest_version_of_software(software_versions)

        version = "1.0.0" if version == "1.0" else version
        w.write(version)


def _get_highest_version_of_software(version_of_software=[]):
    version_of_software.sort(key=lambda soft_ver: [int(version) for version in soft_ver.split('.')])
    return version_of_software[-1]


if __name__ == "__main__":
    get_latest_from_artifactory(sys.argv[2:])
    bump_build_version(get_detailed_information())