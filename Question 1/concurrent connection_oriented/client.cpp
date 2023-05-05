#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

// Serial Number Program
int main() {
    // Create a socket and connect to the server
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        std::cerr << "Failed to create client socket\n";
        return 1;
    }

    sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(3000);
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(client_socket, (sockaddr*)&server_address, sizeof(server_address)) == -1) {
        std::cerr << "Failed to connect to server\n";
        return 1;
    }

    std::cout << "Connected to server\n";

    std::string serial_number;
    std::string registration_number;
    std::string name;

    // Loop until the user chooses to exit
    while (true) {
        std::cout << "Enter a serial number (or 'exit' to quit): ";
        std::getline(std::cin, serial_number);

        // Check if the user wants to exit
        if (serial_number == "exit") {
            send(client_socket, serial_number.c_str(), serial_number.size(), 0);
            break;
            }

        // Send the serial number to the server
        int bytes_sent = send(client_socket, serial_number.c_str(), serial_number.size(), 0);
        if (bytes_sent == -1) {
            std::cerr << "Failed to send data to server\n";
            continue;
        }

        // Wait for response from server
        char buffer[1024];
        int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytes_received == -1) {
            std::cerr << "Failed to receive data from server\n";
            continue;
        }
        std::string response(buffer, bytes_received);
        std::cout << response << std::endl;
        if (response == "Serial number already exists.\n") {
            continue;
        }

        while (true) {
            std::cout << "Enter a Registration Number (P15/...): ";
            std::getline(std::cin, registration_number);

            // Send the registration number to the server
            bytes_sent = send(client_socket, registration_number.c_str(), registration_number.size(), 0);
            if (bytes_sent == -1) {
                std::cerr << "Failed to send data to server\n";
                break;
            }

            // Wait for response from server
            bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
            if (bytes_received == -1) {
                std::cerr << "Failed to receive data from server\n";
                break;
            }
            response = std::string(buffer, bytes_received);
            std::cout << response << std::endl;
            if (response == "Registration number already exists.\n") {
                continue;
            } else {
                break;
            }
        }

        std::cout << "Enter a name: ";
        std::getline(std::cin, name);

        // Send the name to the server
        bytes_sent = send(client_socket, name.c_str(), name.size(), 0);
        if (bytes_sent == -1) {
            std::cerr << "Failed to send data to server\n";
            continue;
        }

        // Wait for response from server
        bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytes_received == -1) {
            std::cerr << "Failed to receive data from server\n";
            continue;
        }
        response = std::string(buffer, bytes_received);
        std::cout << response << std::endl;
    }

    // Close the connection
    close(client_socket);

    return 0;
}

