import sys
import socket

if len(sys.argv) != 3:
    exit(sys.argv[0] + ": <bind ip> <bind port>")

try:
    sys.argv[2] = int(sys.argv[2])
except:
    exit("The port to bind on must be an valid integer.")

server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_socket.bind((sys.argv[1], sys.argv[2]))
server_socket.listen(1)

print(f"Listening on {sys.argv[1]}:{sys.argv[2]} ...")

ps4_client, origin = server_socket.accept()

print(f"Incoming connection from {origin[0]}:{origin[1]}")

try:
    while True:
        sys.stdout.write(ps4_client.recv(1).decode())
        sys.stdout.flush()
except:
    exit()