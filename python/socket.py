

import socket

#create socket
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

#server socket
s.bind(( socket.gethostname(), 1234))
s.listen(5)

#accept connections
while 1:
	(clientsocket, address) = s.accept()


#client socket
s.connect(( "remoteip", port))
