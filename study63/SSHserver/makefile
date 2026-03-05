.PHONY:all
all:client_ssh server_ssh

server_ssh:SSH_Server.cc
	g++ -o $@ $^ -std=c++17 -lpthread
client_ssh:SSH_Client.cc
	g++ -o $@ $^ -std=c++17 -static

.PHONY:clean
clean:
	rm -f client_ssh server_ssh