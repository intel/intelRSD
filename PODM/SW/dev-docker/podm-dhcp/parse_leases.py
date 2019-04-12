# Copyright (c) 2015-2019 Intel Corporation
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

#!/usr/bin/env python3


import os
import re
import grp
import pwd
import sys
import logging
import logging.handlers
import argparse
import datetime

LEASES_FILE = '/var/lib/dhcp/leases' #TODO: get this from ENV?
LOG_FILE = '/tmp/parse_leases.log' #TODO: get this from ENV?
LOG_FILE_MAX_SIZE_BYTES = 10000000  # 10 MB
LOG_FILE_BACKUP_COUNT = 5


def parse_arguments():
    parser = argparse.ArgumentParser("parse_leases.py")
    parser.add_argument("-i", "--ip-address", dest='ip', required=True, help="IP address.")
    parser.add_argument('-e', '--dhcp-event', dest='dhcp_event', required=True,
        choices=['commit', 'release', 'expiry'], help='DHCP event type')
    parser.add_argument("-d", "--hardware", type=parse_hardware, dest='hardware', required=True, help="Hardware variable passed from dhcp.conf file")
    parser.add_argument("-n", "--host-name", dest='hostname', help='Hostname')

    args = parser.parse_args()
    args.mac_addr = args.hardware

    return args


def parse_hardware(hardware):
    mac_regex = re.compile('^' + '[\:]'.join(['([0-9A-F]{1,2})']*6) + '$', re.IGNORECASE)
    if not mac_regex.match(hardware):
        raise argparse.ArgumentTypeError(hardware + ' is not a valid MAC address.')

    return hardware


def on_commit(arguments, logger):
    leases = get_leases()
    predicate = lambda lease: lease['mac'] == arguments.mac_addr
    lease_to_update = next((lease for lease in leases if predicate(lease)), None)

    commit_date = datetime.datetime.now().isoformat()
    if lease_to_update:
        logger.info('Changing lease from ' + lease_to_update['ip'] + ' to ' + arguments.ip + ' for ' + lease_to_update['mac'])
        lease_to_update['date'] = commit_date
        lease_to_update['ip'] = arguments.ip
        lease_to_update['hostname'] = arguments.hostname
    else:
        new_lease = {'mac' : arguments.mac_addr, 'ip' : arguments.ip, 'hostname' : arguments.hostname, 'date' : commit_date}
        leases.append(new_lease)
        logger.info('Commited ' + arguments.ip + ' on ' + arguments.mac_addr)

    return leases


def on_expiry(arguments, logger):
    logger.info('Expiry ' + arguments.ip + ' on ' + arguments.mac_addr)
    # temporary workaround for isc-dhcp-server issue : ISC-Bugs #24586
    predicate = lambda lease: lease['ip'] != arguments.ip
    return on_expiry_or_release(predicate)


def on_release(arguments, logger):
    logger.info('Release ' + arguments.ip + ' on ' + arguments.mac_addr)
    predicate = lambda lease: lease['mac'] != arguments.mac_addr
    return on_expiry_or_release(predicate)


def on_expiry_or_release(predicate):
    leases = get_leases()
    updated_leases = [lease for lease in leases if predicate(lease)]
    return updated_leases


def get_leases():
    leases_list = list()
    names = ['mac', 'ip', 'hostname', 'date']
    with open(LEASES_FILE, 'r') as leases:
        for line in leases:
            leases_list.append(dict(zip(names, line.split())))

    return leases_list


def save_leases(leases_list):
    with open(LEASES_FILE, 'w+') as leases:
        for lease in leases_list:
            leases.write(str(lease['mac']) + ' ' + lease['ip'] + ' ' + lease['hostname'] + ' ' + lease['date'] + '\n')


def create_file_if_necessary():
    open(LEASES_FILE, "a").close()


def assure_permissions():
    DHCPD = 'dhcpd'
    ROOT_USER_UID = 0
    if os.getuid() == ROOT_USER_UID:
        dhcpd_uid = pwd.getpwnam(DHCPD).pw_uid
        dhcpd_gid = grp.getgrnam(DHCPD).gr_gid
        os.chown(LOG_FILE, dhcpd_uid, dhcpd_gid)
        os.chown(LEASES_FILE, dhcpd_uid, dhcpd_gid)


def create_logger():
    # allows to redirect stderr to specified logger
    class StreamLoggerProxy:
        def __init__(self, level):
            self.level = level
        def write(self, message):
            self.level(message)

    logger = logging.getLogger('parse_leases')
    handler = logging.handlers.RotatingFileHandler(filename=LOG_FILE, maxBytes=LOG_FILE_MAX_SIZE_BYTES, backupCount=LOG_FILE_BACKUP_COUNT)
    formatter = logging.Formatter('%(asctime)s %(levelname)s %(message)s')
    handler.setFormatter(formatter)
    logger.addHandler(handler)
    logger.setLevel(logging.INFO)
    sys.stderr = StreamLoggerProxy(logger.error)

    return logger


def main():
    logger = create_logger()
    try:
        args = parse_arguments()
        create_file_if_necessary()
        actions = {'commit': on_commit, 'release': on_release, 'expiry': on_expiry}
        updated_leases = actions[args.dhcp_event](args, logger)
        save_leases(updated_leases)
        assure_permissions()
        sys.exit(0)
    except Exception as e:
        logger.exception(e)
        sys.exit(-1)

if __name__ == '__main__':
    main()
