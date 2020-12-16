from .kbw import kbw, set_plugin_path, set_seed 
from enum import Enum
from math import ceil
from os.path import dirname
from random import randint
from struct import pack, unpack
from threading import Thread
import argparse
import socket
from sys import argv
from os import getenv

ACK = pack('<b', 0)

class Command(Enum):
    EXIT = 0
    GET = 1
    DUMP = 2

class Client(Thread):
    def __init__(self, client, address):
        Thread.__init__(self)
        self.client = client
        self.address = address
        print('\tConnected by', address, sep='\t')
    
    def run(self):
        print('\tWaiting KQASM...', self.address, sep='\t')
        
        file_size, =  unpack('<I', self.client.recv(4))
        self.client.sendall(ACK)
        print('\tKQASM size\t', file_size,self.address, sep='\t')
        
        kqasm_buffer = bytearray()
        for _ in range(ceil(file_size/buffer_size)):
            data = self.client.recv(buffer_size)
            kqasm_buffer += data
        
        kqasm_file = kqasm_buffer.decode()

        print('\tRunning KQASM...', self.address, sep='\t')
        quantum_execution = kbw(kqasm_file)
        quantum_execution.run()

        self.client.sendall(ACK)
        
        while True:
            print('\tWaiting command...', self.address, sep='\t')

            command = Command(*unpack('<b', self.client.recv(1)))
            self.client.sendall(ACK)
            print('\t\tProcessing', command, self.address, sep='\t')

            if command == Command.GET:
                idx, = unpack('<Q', self.client.recv(8))
                result = quantum_execution.get_result(idx)

                print('\t\tSending result', idx, result, self.address, sep='\t')
                self.client.sendall(pack('<Q', result))

            elif command == Command.DUMP:
                idx, = unpack('<Q', self.client.recv(8))
                result = quantum_execution.get_dump_states(idx)

                print('\t\tSending dump', idx, result, self.address, sep='\t')
                self.client.sendall(pack('<Q',  len(result)))
                
                self.client.recv(1)

                for i in result:
                    amplitude = quantum_execution.get_dump_amplitude(idx, i)
                    self.client.sendall(pack('<QQ', i, len(amplitude)))
                    self.client.recv(1)                
                    
                    print('\t\t\tSending amplitude', i, amplitude, self.address, sep='\t')

                    for a in amplitude:
                        cx = pack('<dd', a.real, a.imag)
                        self.client.sendall(cx)
                        self.client.recv(1)
                
            elif command == Command.EXIT:
                break

        self.client.close()
        print("Connection", self.address, "closed\n")

def main():
    description = 'Ket Biswise Simulator server'
    print(description)
    print('============================\n')
    
    parser_args = argparse.ArgumentParser(prog='kbw', description=description)
    parser_args.add_argument('-b', metavar='127.0.1.1 ', type=str, default='127.0.1.1', help='Server bind')
    parser_args.add_argument('-p', metavar='4242', type=int, default=4242, help='Server port')
    parser_args.add_argument('-s', metavar='random', type=int, default=randint(0, 2048), help='Seed for the PRNG')
    parser_args.add_argument('-l', metavar='', type=str, help='Extra plugin path')
    args = parser_args.parse_args() 

    print('Setting up:') 
    global buffer_size
    buffer_size = 1024

    set_seed(args.s)
    print('\tSeed\t', args.s, sep='\t')

    plugin_path = dirname(__file__)
    if args.l:
        plugin_path += ':' + args.l
    set_plugin_path(plugin_path)
    print('\tPlugin PATH', plugin_path, sep='\t')

    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server.bind((socket.gethostbyname(args.b), args.p))
    print('\tBind\t\t', socket.gethostbyname(args.b), ':', args.p, sep='')

    print("\nUse Ctrl+c to stop the server\n")

    server.listen(5)

    while True:
        try:
            print('Waiting connection...')
            client = Client(*server.accept())
            client.start()
        except KeyboardInterrupt:
            break

    server.close()
    print("Server stopped")
        
if __name__ == "__main__":
    if '' in argv:
        argv.remove('')
    main() 