#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define PORT 12345
#define BUFFER_SIZE 1024

int main() {
    // Create socket
    int clientSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (clientSocket < 0) {
        std::cerr << "Failed to create socket." << std::endl;
        return 1;
    }

    // Prepare server address structure
    struct sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);
    if (inet_pton(AF_INET, SERVER_IP, &(serverAddress.sin_addr)) <= 0) {
        std::cerr << "Failed to set server address." << std::endl;
        return 1;
    }

    // Send request to server
    const char* request = "Send me the time!";
    if (sendto(clientSocket, request, strlen(request), 0, (struct sockaddr*) &serverAddress, sizeof(serverAddress)) < 0) {
        std::cerr << "Failed to send request to server." << std::endl;
        return 1;
    }

    // Receive response from server
    char buffer[BUFFER_SIZE];
    socklen_t serverAddressLength = sizeof(serverAddress);
    ssize_t bytesRead = recvfrom(clientSocket, buffer, sizeof(buffer) - 1, 0, (struct sockaddr*) &serverAddress, &serverAddressLength);
    if (bytesRead < 0) {
        std::cerr << "Failed to receive response from server." << std::endl;
        return 1;
    }

    // Null-terminate the received data
    buffer[bytesRead] = '\0';

    // Display the received time from the server
    std::cout << "Server time: " << buffer << std::endl;

    // Close socket
    close(clientSocket);
    return 0;
}
