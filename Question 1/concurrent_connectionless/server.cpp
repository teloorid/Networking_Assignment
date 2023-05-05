#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>

struct ClientData {
    int server_socket;
    std::string filename;
    sockaddr_in client_address;
    socklen_t client_address_size;

    ClientData(int server_socket, std::string filename, sockaddr_in client_address, socklen_t client_address_size)
        : server_socket(server_socket), filename(filename), client_address(client_address), client_address_size(client_address_size) {}
};

void* handle_client(void* arg) {
    ClientData* client_data = (ClientData*)arg;
    int server_socket = client_data->server_socket;
    std::string filename = client_data->filename;
    sockaddr_in client_address = client_data->client_address;
    socklen_t client_address_size = client_data->client_address_size;

    std::string serial_number;
    std::string registration_number;
    std::string name;

    // Handle multiple requests from this client
    while (true) {
        // Receive request from client
        char buffer[1024];
        int bytes_received = recvfrom(server_socket, buffer, sizeof(buffer), 0,(sockaddr*)&client_address,&client_address_size);
        if (bytes_received == -1) {
            std::cerr << "Failed to receive data from client\n";
            break;
        }

        // Print received data
        std::cout << "Received data from client: " << std::string(buffer, bytes_received) << "\n";

        // Check if the client wants to exit
        if (std::string(buffer, bytes_received) == "exit") {
            break;
        }

        // Process request
        serial_number = std::string(buffer, bytes_received);
        bool exists = false;
        std::ifstream infile(filename);
        std::string line;
        while (getline(infile, line)) {
            if (line == serial_number) {
                exists = true;
                break;
            }
        }
        infile.close();
         if (!exists) {
            std::ofstream outfile(filename, std::ios_base::app);
            outfile << serial_number << "\n";
            outfile.close();
            sendto(server_socket, "Serial number added.\n", 20, 0,(sockaddr*)&client_address,client_address_size);
        } else {
            sendto(server_socket, "Serial number already exists.\n", 29, 0,(sockaddr*)&client_address,client_address_size);
            continue;
        }

        bytes_received = recvfrom(server_socket, buffer,sizeof(buffer),0,(sockaddr*)&client_address,&client_address_size);
        if (bytes_received == -1) {
            std::cerr << "Failed to receive data from client\n";
            break;
        }

        // Print received data
        std::cout << "Received data from client: " << std::string(buffer, bytes_received) << "\n";

        registration_number = std::string(buffer, bytes_received);
        exists = false;
        infile.open(filename);
        while (getline(infile, line)) {
            if (line == registration_number) {
                exists = true;
                break;
            }
        }
        infile.close();
        if (!exists) {
            std::ofstream outfile(filename, std::ios_base::app);
            outfile << registration_number << "\n";
            outfile.close();
            sendto(server_socket, "Registration number added.\n", 25, 0,(sockaddr*)&client_address,client_address_size);
        } else {
            sendto(server_socket, "Registration number already exists.\n", 34, 0,(sockaddr*)&client_address,client_address_size);
        }

        bytes_received = recvfrom(server_socket, buffer,sizeof(buffer),0,(sockaddr*)&client_address,&client_address_size);
        if (bytes_received == -1) {
            std::cerr << "Failed to receive data from client\n";
            break;
        }

        // Print received data
        std::cout << "Received data from client: " << std::string(buffer, bytes_received) << "\n";

        name = std::string(buffer, bytes_received);
        std::ofstream outfile(filename, std::ios_base::app);
                outfile << name << "\n";
        outfile.close();
        sendto(server_socket, "Name added.\n", 11, 0,(sockaddr*)&client_address,client_address_size);
    }

    delete client_data;
    return nullptr;
}

int main() {
    std::string filename = "serial_reg_and_name.txt";

    // Create a server-socket (listener) and bind it to a port (higher than 1024)
    int server_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_socket == -1) {
        std::cerr << "Failed to create server socket\n";
        return 1;
    }

    sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(3000);
    server_address.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, (sockaddr*)&server_address, sizeof(server_address)) == -1) {
        std::cerr << "Failed to bind server socket\n";
        return 1;
    }

    std::cout << "Server listening on port 3000\n";

    while (true) {
        // Receive request from client
        char buffer[1024];
        sockaddr_in client_address;
        socklen_t client_address_size = sizeof(client_address);
        int bytes_received = recvfrom(server_socket, buffer,sizeof(buffer),0,(sockaddr*)&client_address,&client_address_size);
         if (bytes_received == -1) {
            std::cerr << "Failed to receive data from client\n";
            continue;
        }

        // Print received data
        std::cout << "Received data from client: " << std::string(buffer, bytes_received) << "\n";

        // Check if the client wants to exit
        if (std::string(buffer, bytes_received) == "exit") {
            break;
        }

        // Create a new thread to handle this client
        pthread_t thread;
        ClientData* client_data = new ClientData(server_socket, filename, client_address, client_address_size);
        pthread_create(&thread, nullptr, handle_client, client_data);
    }

    // Close server socket
    close(server_socket);

    return 0;
}
