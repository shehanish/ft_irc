
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        std::cerr << "Socket creation failed." << std::endl;
        return 1;
    }

    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(6667);
    address.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        std::cerr << "Bind failed." << std::endl;
        return 1;
    }

    if (listen(server_fd, 1) < 0) {
        std::cerr << "Listen failed." << std::endl;
        return 1;
    }

    std::cout << "Server listening on port 6667..." << std::endl;

    int addrlen = sizeof(address);
    int client_fd = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
    if (client_fd < 0) {
        std::cerr << "Accept failed." << std::endl;
        return 1;
    }

    char buffer[1024] = {0};
    int valread = read(client_fd, buffer, 1024);
	if (valread > 0) {
		std::cout << "Received from client: " << buffer << std::endl;
	} else {
		std::cerr << "No data received or read failed." << std::endl;
	}

    std::cout << "Received from client: " << buffer << std::endl;

    close(client_fd);
    close(server_fd);
    return 0;
}
