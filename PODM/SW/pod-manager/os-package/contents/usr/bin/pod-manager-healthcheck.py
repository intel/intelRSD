#!/usr/bin/env python3

# Copyright (c) 2016-2018 Intel Corporation
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

import argparse
import datetime
import logging
import os
import shutil
import subprocess
import tarfile

SETTINGS = {
    'DB_NAME': 'podm',
    'DB_USER': 'podm',
    'OUTPUT_DIR':
        '/tmp/pod_manager_health_check_%s' % datetime.datetime.now().strftime('%Y-%m-%d-%H-%M-%S-%f')[:-3]
}

DIR_CONTENT_TO_COPY = {
    '/etc/pod-manager': 'full',
    '/tmp/pod-manager/config': 'full',
    '/opt/pod-manager/wildfly/standalone/log': 'full',
    '/var/log/pod-manager': 'full',
    '/var/log/postgresql': 'full',
    '/var/log/wildfly': 'full',
}

FILES_TO_COPY = {
    '/opt/pod-manager/wildfly/standalone/configuration/standalone.xml': 'full',
    '/tmp/leases': 'both',
    '/tmp/services.list': 'both',
    '/var/lib/dhcp/dhcpd.leases': 'both',
    '/var/lib/pod-manager/service-root-uuid.json': 'full',
    '/var/log/boot.log': 'full',
    '/var/log/dmesg': 'full',
    '/var/log/syslog': 'full'
}

COMMANDS_TO_EXECUTE = {
    'ifconfig -a': ('ifconfig-a.txt', 'both'),
    'cat /proc/cpuinfo': ('cpuinfo.txt', 'both'),
    'cat /proc/meminfo': ('meminfo.txt', 'both'),
    'lshw': ('lshw.txt', 'both'),
    'top -n 1 -b': ('top.txt', 'both'),
    'dpkg-query -s pod-manager': ('dpkg-query-s-pod-manager.txt', 'full'),
    'dpkg-query -s pod-manager-networking': ('dpkg-query-s-pod-manager-networking.txt', 'full'),
    '/usr/lib/pod-manager/crawler/crawler.py full-crawl --log-file-name {crawler_log_file_name} > {crawler_output_file_name}'.format(
        crawler_output_file_name=os.path.join(SETTINGS['OUTPUT_DIR'], 'resources.json'),
        crawler_log_file_name=os.path.join(SETTINGS['OUTPUT_DIR'], 'pod-manager-crawler.log')
    ): ('', 'both')
}


VAULT_COMMAND = 'java -jar /usr/lib/pod-manager/vault-decrypter.jar --password-type database'
DB_DUMP_COMMAND = 'pg_dump -U {0} -h localhost -f {1} -F plain --no-password {2}'
GET_JAVA_PIDS_COMMAND = 'pgrep -f [j]boss-modules.jar'
THREAD_DUMP_COMMAND = 'kill -3 {0}'


def parse_arguments():
    parser = argparse.ArgumentParser(description='Creates health check for PodManager.')
    parser.add_argument('-o', '--output', dest='output_file', required=False,
                        default=''.join([SETTINGS['OUTPUT_DIR'], '.tar.gz']), help='Name of output archive.')
    parser.add_argument('--short', action='store_true', dest='short', help='Run short version of this script.')

    args = parser.parse_args()
    SETTINGS['OUTPUT_FILE'] = args.output_file
    SETTINGS['SCRIPT_MODE'] = 'short' if args.short else 'full'


def initialize():
    try:
        os.makedirs(SETTINGS['OUTPUT_DIR'], mode=0o777, exist_ok=True)
    except OSError:
        logging.error('Could not create health check output directory: %s' % SETTINGS['OUTPUT_DIR'])
        raise

    logging.basicConfig(filename=os.path.join(SETTINGS['OUTPUT_DIR'], 'health_check.log'),
                        level=logging.DEBUG, format='%(asctime)s %(levelname)s %(message)s')

    logging.debug('Health check output directory: %s' % SETTINGS['OUTPUT_DIR'])
    logging.debug('Health check output archive: %s' % SETTINGS['OUTPUT_FILE'])


def should_be_run_for_script_mode(script_mode):
    if script_mode == 'both' or script_mode == SETTINGS['SCRIPT_MODE']:
        return True

    return False


def dump_database():
    logging.debug('Acquiring database password...')
    try:
        password = subprocess.check_output(VAULT_COMMAND.split(),
                                           universal_newlines=True, stderr=subprocess.STDOUT)
        SETTINGS['DB_PASSWORD'] = password.strip()
    except subprocess.CalledProcessError as e:
        logging.exception('Could not acquire database password: %s' % e.output)

    if 'DB_PASSWORD' in SETTINGS:
        logging.info('Dumping database...')
        os.putenv('PGPASSWORD', SETTINGS['DB_PASSWORD'])
        dump_command = DB_DUMP_COMMAND.format(
            SETTINGS['DB_USER'],
            os.path.join(SETTINGS['OUTPUT_DIR'], 'pod_manager_db.sql'),
            SETTINGS['DB_NAME'])
        logging.debug('Dump command: %s' % dump_command)
        try:
            subprocess.check_output(dump_command.split(), universal_newlines=True, stderr=subprocess.STDOUT)
        except subprocess.CalledProcessError as e:
            logging.exception('Dumping database failed: %s' % e.output)
        else:
            logging.info('Dumping database finished.')
    else:
        logging.warning('Ignoring database dump - database password was not acquired.')


def thread_dump():
    pids = []
    logging.info('Getting PIDs of Java processes...')
    try:
        processes = subprocess.check_output(GET_JAVA_PIDS_COMMAND.split(),
                                            universal_newlines=True, stderr=subprocess.STDOUT)
        for process in processes.splitlines():
            parts = process.split()
            try:
                pids.append(parts[0])
            except IndexError:
                pass
    except subprocess.CalledProcessError as e:
        logging.exception('Getting PIDs of Java processes failed: %s' % e.output)
    else:
        logging.info('Getting PIDs of Java processes finished.')
        for pid in pids:
            td_command = THREAD_DUMP_COMMAND.format(pid)
            logging.debug('Thread dump command: %s' % td_command)
            try:
                subprocess.check_output(td_command.split(), universal_newlines=True, stderr=subprocess.STDOUT)
            except subprocess.CalledProcessError as e:
                logging.exception('Thread dump command for process: %s failed: %s' % (pid, e.output))
            else:
                logging.info('Thread dump command for process: %s finished.' % pid)


def copy_files_from_dir(directory):
    if os.path.isdir(directory):
        files = os.listdir(directory)
        logging.info('Copying files from: %s started...' % directory)
        for file in files:
            full_file_path = os.path.join(directory, file)
            copy_file(full_file_path)
        logging.info('Copying files from: %s finished.' % directory)
    else:
        logging.warning('Ignoring files copy from %s - not a valid directory.' % directory)


def copy_file(full_file_path):
    if os.path.isfile(full_file_path):
        logging.debug('Copying file: %s' % full_file_path)
        try:
            shutil.copy2(full_file_path, SETTINGS['OUTPUT_DIR'])
        except PermissionError:
            logging.exception('Copying file: %s failed.' % full_file_path)
    else:
        logging.warning('Ignoring file copy: %s - not an existing, regular file.' % full_file_path)


def execute_command(command_to_execute):
    logging.info("Executing command: %s" % command_to_execute)
    os.system(command_to_execute)


def create_archive():
    logging.info('Packing files to: %s' % SETTINGS['OUTPUT_FILE'])
    try:
        with tarfile.open(SETTINGS['OUTPUT_FILE'], 'w:gz') as tar:
            for dirpath, dirnames, filenames in os.walk(SETTINGS['OUTPUT_DIR']):
                for filename in filenames:
                    full_file_path = os.path.join(dirpath, filename)
                    os.chmod(full_file_path, 0o777)
                    tar.add(full_file_path, arcname=filename)
        os.chmod(SETTINGS['OUTPUT_FILE'], 0o777)
    except Exception as e:
        print('Packing files to: %s failed.' % SETTINGS['OUTPUT_FILE'])
        print(e)


def remove_output_dir():
    try:
        shutil.rmtree(SETTINGS['OUTPUT_DIR'])
    except Exception as e:
        print('Removing: %s failed.' % SETTINGS['OUTPUT_DIR'])
        print(e)


def main():
    parse_arguments()
    initialize()

    if should_be_run_for_script_mode('both'):
        dump_database()

    if should_be_run_for_script_mode('full'):
        thread_dump()

    for directory, script_mode in DIR_CONTENT_TO_COPY.items():
        if should_be_run_for_script_mode(script_mode):
            copy_files_from_dir(directory)

    for file, script_mode in FILES_TO_COPY.items():
        if should_be_run_for_script_mode(script_mode):
            copy_file(file)

    for command, (output_file, script_mode) in COMMANDS_TO_EXECUTE.items():
        if should_be_run_for_script_mode(script_mode):
            if not output_file:
                execute_command(command)
            else:
                execute_command(' > '.join([command, os.path.join(SETTINGS['OUTPUT_DIR'], output_file)]))

    create_archive()
    remove_output_dir()

    if os.path.isfile(SETTINGS['OUTPUT_FILE']):
        print('Health check created: %s' % SETTINGS['OUTPUT_FILE'])
    else:
        print('Health check was not created due to errors.')

if __name__ == '__main__':
    main()
