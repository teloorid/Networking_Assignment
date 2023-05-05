#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int main() {
    // Create a socket
    int client_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (client_socket == -1) {
        std::cerr << "Failed to create client socket\n";
        return 1;
    }

    sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(3000);
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");

    std::string serial_number;
    std::string registration_number;
    std::string name;

    // Loop until the user chooses to exit
    while (true) {
        std::cout << "Enter a serial number (or 'exit' to quit): ";
        std::getline(std::cin, serial_number);

        // Check if the user wants to exit
        if (serial_number == "exit") {
            sendto(client_socket, serial_number.c_str(), serial_number.size(), 0,(sockaddr*)&server_address,sizeof(server_address));
            break;
        }

        // Send the serial number to the server
        int bytes_sent = sendto(client_socket, serial_number.c_str(), serial_number.size(), 0,(sockaddr*)&server_address,sizeof(server_address));
        if (bytes_sent == -1) {
            std::cerr << "Failed to send data to server\n";
            continue;
        }

        // Print sent data
        std::cout << "Sent data to server: " << serial_number << "\n";

        // Wait for response from server
        char buffer[1024];
        socklen_t server_address_size = sizeof(server_address);
        int bytes_received = recvfrom(client_socket, buffer, sizeof(buffer), 0,(sockaddr*)&server_address,&server_address_size);
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
            bytes_sent = sendto(client_socket, registration_number.c_str(), registration_number.size(), 0,(sockaddr*)&server_address,sizeof(server_address));
            if (bytes_sent == -1) {
                std::cerr << "Failed to send data to server\n";
                break;
            }

            // Print sent data
            std::cout << "Sent data to server: " << registration_number << "\n";

            // Wait for response from server
            bytes_received = recvfrom(client_socket, buffer, sizeof(buffer), 0,(sockaddr*)&server_address,&server_address_size);
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
        bytes_sent = sendto(client_socket, name.c_str(), name.size(), 0,(sockaddr*)&server_address,sizeof(server_address));
        if (bytes_sent == -1) {
            std::cerr << "Failed to send data to server\n";
            continue;
        }

        // Print sent data
        std::cout << "Sent data to server: " << name << "\n";

        // Wait for response from server
        bytes_received = recvfrom(client_socket, buffer, sizeof(buffer), 0,(sockaddr*)&server_address,&server_address_size);
        if (bytes_received == -1) {
            std::cerr << "Failed to receive data from server\n";
            continue;
        }
        response = std::string(buffer, bytes_received);
        std::cout << response << std::endl;
    }

    // Close the socket
    close(client_socket);

    return 0;
}



