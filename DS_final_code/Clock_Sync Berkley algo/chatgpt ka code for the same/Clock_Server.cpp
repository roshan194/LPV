#include <iostream>
#include <ctime>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 12345

int main() {
    // Create socket
    int serverSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (serverSocket < 0) {
        std::cerr << "Failed to create socket." << std::endl;
        return 1;
    }

    // Prepare server address structure
    struct sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(PORT);

    // Bind socket to address
    if (bind(serverSocket, (struct sockaddr*) &serverAddress, sizeof(serverAddress)) < 0) {
        std::cerr << "Failed to bind socket to address." << std::endl;
        return 1;
    }

    std::cout << "Server started. Listening for client requests..." << std::endl;

    while (true) {
        // Receive client request
        struct sockaddr_in clientAddress{};
        socklen_t clientAddressLength = sizeof(clientAddress);
        char buffer[1024]{};
        if (recvfrom(serverSocket, buffer, sizeof(buffer), 0, (struct sockaddr*) &clientAddress, &clientAddressLength) < 0) {
            std::cerr << "Failed to receive client request." << std::endl;
            break;
        }

        // Get current time
        auto now = std::chrono::system_clock::now();
        auto currentTime = std::chrono::system_clock::to_time_t(now);

        // Send current time to client
        std::string timeStr = std::ctime(&currentTime);
        sendto(serverSocket, timeStr.c_str(), timeStr.length(), 0, (struct sockaddr*) &clientAddress, clientAddressLength);
    }

    // Close socket
    close(serverSocket);
    return 0;
}
