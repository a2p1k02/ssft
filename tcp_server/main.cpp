#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

class Server {
private:
    std::fstream file;

    uint port;

    int general_socket_descriptor;
    int new_socket_descriptor;

    struct sockaddr_in address;
    int address_length;

public:
    Server(int address_port, std::string file_path)
    {
        create_socket();
        port = address_port;

        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(port);
        address_length = sizeof(address);

        bind_socket();
        listen_socket();
        accept_connection();

        file.open(file_path);
        if (file.is_open()) {
            std::cout << "[LOG] : File is ready to transmit\n";
        } else {
            std::cerr << "[ERR] : File loading failed. Shutting down\n";
            exit(EXIT_FAILURE);
        }
    }

    void create_socket()
    {
        if ((general_socket_descriptor = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
            std::cerr << "[ERR] : Socket failed\n";
            exit(EXIT_FAILURE);
        }
        std::cout << "[LOG] : Socket created successfully\n";
    }

    void bind_socket()
    {
        if (bind(general_socket_descriptor, (struct sockaddr*)&address, address_length) < 0) {
            std::cerr << "[ERR] : Bind failed\n";
            exit(EXIT_FAILURE);
        }
        std::cout << "[LOG] : Bind successfully\n";
    }

    void listen_socket()
    {
        if (listen(general_socket_descriptor, 3) < 0) {
            std::cerr << "[ERR] : Listen failed\n";
            exit(EXIT_FAILURE);
        }
        std::cout << "[LOG] : Socket in listen state\n";
    }

    void accept_connection()
    {
        if ((new_socket_descriptor = accept(general_socket_descriptor, (struct sockaddr* )&address, (socklen_t*)&address_length)) < 0) {
            std::cerr << "[ERR] : Accept failed\n";
            exit(EXIT_FAILURE);
        }
        std::cout << "[LOG] : Connected to Client\n";
    }

    void send_file()
    {
        std::string contents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        std::cout << "[LOG] : File size is " << contents.size() << " bytes\n";

        std::cout << "[LOG] : Sending file...\n";

        int bytes_sent = send(new_socket_descriptor, contents.c_str(), contents.length(), 0);
        std::cout << "[LOG] : File's size sent " << bytes_sent << " bytes\n";
        std::cout << "[LOG] : File transfer completed\n";

    }
};

int main(int argc, char* argv[])
{       
    Server server(8001, argv[1]);
    server.send_file();
    return 0;
}
