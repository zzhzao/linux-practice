.PHONY:all
all:client_udp server_udp
server_udp:ChatServerMain.cc
	g++ -o $@ $^ -std=c++17
client_udp:ChatClient.cc
	g++ -o $@ $^ -std=c++17
.PHONY:clean
clean:
	rm -rf client_udp server_udp
	