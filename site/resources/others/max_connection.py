import socket, time

req = b"GET / HTTP/1.1\r\nHost: localhost\r\nConnection: keep-alive\r\n\r\n"

conns = []
for i in range(230):
    s = socket.socket()
    s.connect(("127.0.0.1", 8090))
    s.sendall(req)  # envoie une requête HTTP valide
    conns.append(s)
    print("Opened", i+1)
    time.sleep(0.01)

input("Press Enter to close...")
