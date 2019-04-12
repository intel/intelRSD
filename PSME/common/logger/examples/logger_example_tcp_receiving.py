#!/usr/bin/env python
''' logger_udp_receiving
'''

__license__ = '''\
Copyright (c) 2015-2019 Intel Corporation

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
'''

import socket
import sys

TCP_IP = "127.0.0.1"
TCP_PORT = 8889

def main():
    ''' Main function '''
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.bind((TCP_IP, TCP_PORT))
    sock.listen(1)
    conn, _ = sock.accept()

    data = ""

    while True:
        data = conn.recv(4096)
        sys.stdout.write(data)

if __name__ == '__main__':
    main()
