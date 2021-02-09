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

    set_plugin_path(plugin_path)
    environ['KET_PYCALL'] = plugin_path+'/ket_pycall_interpreter'
    seed = randint(0, 2**31)
    set_seed(seed)
    print('\tseed', seed, sep='\t')

    print('\tWaiting KQASM...', address, sep='\t')
    
    file_size, =  unpack('<I', client.recv(4))
    client.sendall(ACK)
    print('\tKQASM size\t', file_size,address, sep='\t')
    
    kqasm_buffer = bytearray()
    for _ in range(ceil(file_size/buffer_size)):
        data = client.recv(buffer_size)
        kqasm_buffer += data
    
    kqasm_file = kqasm_buffer.decode()

    print('\tRunning KQASM...', address, sep='\t')
    quantum_execution = kbw(kqasm_file)
    quantum_execution.run()

    client.sendall(ACK)
    
    while True:
        print('\tWaiting command...', address, sep='\t')

        command = Command(*unpack('<b', client.recv(1)))
        client.sendall(ACK)
        print('\t\tProcessing', command, address, sep='\t')

        if command == Command.GET:
            idx, = unpack('<Q', client.recv(8))
            result = quantum_execution.get_result(idx)

            print('\t\tSending result', idx, result, address, sep='\t')
            client.sendall(pack('<Q', result))

        elif command == Command.DUMP:
            idx, = unpack('<Q', client.recv(8))
            result = quantum_execution.get_dump_states(idx)

            print('\t\tSending dump', idx, result, address, sep='\t')
            client.sendall(pack('<Q',  len(result)))
            
            client.recv(1)

            for i in result:
                amplitude = quantum_execution.get_dump_amplitude(idx, i)
                client.sendall(pack('<QQ', i, len(amplitude)))
                client.recv(1)                
                
                print('\t\t\tSending amplitude', i, amplitude, address, sep='\t')

                for a in amplitude:
                    cx = pack('<dd', a.real, a.imag)
                    client.sendall(cx)
                    client.recv(1)
            
        elif command == Command.EXIT:
            break

    client.close()
    print("Connection", address, "closed\n")
    exit(0)

def main():
    description = 'Ket Biswise Simulator server'
    print(description)
    print('============================\n')
    
    parser_args = argparse.ArgumentParser(prog='kbw', description=description)
    parser_args.add_argument('-b', metavar='127.0.1.1 ', type=str, default='127.0.1.1', help='Server bind')
    parser_args.add_argument('-p', metavar='4242', type=int, default=4242, help='Server port')
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

    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server.bind((socket.gethostbyname(args.b), args.p))
    print('\tBind\t\t', socket.gethostbyname(args.b), ':', args.p, sep='')

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