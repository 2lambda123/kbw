import socket
from random import randint
from math import ceil
from enum import Enum
from .kbw import kbw, set_plugin_path, get_plugin_path, set_seed

class Command(Enum):
    EXIT = 0
    GET = 1

def wait_command(client, quantum_execution): 
    while True:
        print('Waiting command...')
        command = client.recv(4)
        command = Command(int.from_bytes(command, byteorder='little'))
        print('Processing', command)
        client.sendall((0).to_bytes(1, byteorder='little'))
        if command == Command.GET:
            idx = int.from_bytes(client.recv(8), byteorder='little')
            print('Sending result of', idx)
            client.sendall(quantum_execution.get_result(idx).to_bytes(8, byteorder='little', signed=True))
        elif command == Command.EXIT:
            break
    client.close()
    print("Connection closed")

def main():
    buffer_size = 1024

    set_seed(randint(0, 2048))

    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    server.bind((socket.gethostname(), 4242))

    server.listen(1)

    while True:
        print('Waiting connection...')
        client, address = server.accept()
        print('Connected by', address)

        print('Waiting KQASM...')
        
        file_size = int.from_bytes(client.recv(4), byteorder='little')
        client.sendall((0).to_bytes(1, byteorder='little'))
        
        print('KQASM size:', file_size)
        
        kqasm_buffer = bytearray()
        for _ in range(ceil(file_size/buffer_size)):
            data = client.recv(buffer_size)
            kqasm_buffer += data
        
        kqasm_file = kqasm_buffer.decode()

        print('Running KQASM')
        print(kqasm_file)
        quantum_execution = kbw(kqasm_file)
        quantum_execution.run()

        print('Execution results:')
        print(quantum_execution.get_results(), end='') 
        
        client.sendall((0).to_bytes(1, byteorder='little'))
        
        wait_command(client, quantum_execution)

if __name__ == "__main__":
    main() 