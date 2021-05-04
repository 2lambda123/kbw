#  Copyright 2020, 2021 Evandro Chagas Ribeiro da Rosa <evandro.crr@posgrad.ufsc.br>
#  Copyright 2020, 2021 Rafael de Santiago <r.santiago@ufsc.br>
# 
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
# 
#      http://www.apache.org/licenses/LICENSE-2.0
# 
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.

from multiprocessing.context import Process
from .kbw import kbw, set_plugin_path, set_seed 
from enum import Enum
from math import ceil
from os.path import dirname
from random import randint
from struct import pack, unpack
from os import environ
from multiprocessing import Process
import argparse
import socket
from sys import argv, exit

ACK = pack('<b', 0)

class Command(Enum):
    EXIT = 0
    GET = 1
    DUMP = 2

def client(client, address):
    print('\tConnected by', address, sep='\t')

    ##### Set plugin path ####
    set_plugin_path(plugin_path)
    environ['KET_PYCALL'] = plugin_path+'/ket_pycall_interpreter'
    ##########################

    #### Set seed ####
    seed = randint(0, 2**31)
    set_seed(seed)
    print('\tSeed', seed, address, sep='\t')
    ##################

    print('\tWaiting KQASM...', address, sep='\t')
    
    #### Get KQASM size ####
    file_size, =  unpack('<I', client.recv(4))
    client.sendall(ACK)
    print('\tKQASM size\t', str(file_size)+"B" ,address, sep='\t')
    ########################

    #### Get KQASM ####
    kqasm_buffer = bytearray()
    for _ in range(ceil(file_size/buffer_size)):
        data = client.recv(buffer_size)
        kqasm_buffer += data
    kqasm_file = kqasm_buffer.decode()
    ###################

    #### Run ####
    print('\tRunning KQASM...', address, sep='\t')
    quantum_execution = kbw(kqasm_file)
    quantum_execution.run()

    client.sendall(ACK)
    ############
    
    while True:
        print('\tWaiting command...', address, sep='\t')

        #### Get command ####
        command = Command(*unpack('<b', client.recv(1)))
        client.sendall(ACK)
        print('\t\tProcessing', command, address, sep='\t')
        #####################

        if command == Command.GET: # Get measurement
            idx, = unpack('<Q', client.recv(8))
            result = quantum_execution.get_result(idx)

            print('\t\tSending result', idx, result, address, sep='\t')
            client.sendall(pack('<q', result))

        elif command == Command.DUMP: # Get dump
            #### Get dump index ####
            idx, = unpack('<Q', client.recv(8))
            dump = quantum_execution.get_dump(idx)
            print('\t\tSending dump', idx, str(len(dump)/2**20)+"MB", address, sep='\t')
            client.sendall(pack('<Q', len(dump)))
            ##################

            client.sendall(dump)
            
        elif command == Command.EXIT:
            break

    client.close()
    print("Connection", address, "closed\n")
    exit(0)

def main():
    description = 'Ket Bitwise Simulator server'
    print(description)
    print('============================\n')
    
    parser_args = argparse.ArgumentParser(prog='kbw', description=description)
    parser_args.add_argument('-b', metavar='', type=str, default='', help='Server bind')
    parser_args.add_argument('-p', metavar='', type=int, default=4242, help='Server port')
    parser_args.add_argument('-l', metavar='', type=str, help='Extra plugin path')
    args = parser_args.parse_args() 

    print('Setting up:') 
    global buffer_size
    buffer_size = 1024

    global plugin_path 
    plugin_path = dirname(__file__)
    if args.l:
        plugin_path += ':' + args.l
    print('\tPlugin PATH', plugin_path, sep='\t')

    addr = (args.b, args.p)
    try:
        server = socket.create_server(addr, family=socket.AF_INET6, dualstack_ipv6=True)
    except:
        server = socket.create_server(addr)
    
    addr = server.getsockname()
    addr_str = addr[0]+':'+str(addr[1])

    print('\tAddress\t\t', addr_str, sep='')

    print("\nUse Ctrl+c to stop the server\n")

    server.listen()

    while True:
        try:
            print('Waiting connection...')
            Process(target=client, args=server.accept()).start()
        except KeyboardInterrupt:
            break

    server.close()
    print("Server stopped")
        
if __name__ == "__main__":
    if '' in argv:
        argv.remove('')
    main() 