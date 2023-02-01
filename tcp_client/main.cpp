#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

class Client {
private:
    std::fstream file;
    uint port;
    int general_socket_descriptor;
    struct sockaddr_in address;
    int address_length;
public:
    Client(int sock_port)
    {
        create_socket();
        port = sock_port;

        address.sin_family = AF_INET;
        address.sin_port = htons(port);
        address_length = sizeof(address);

        if (inet_pton(AF_INET, "127.0.0.1", &address.sin_addr) <= 0) {
            std::cerr << "[ERR] : Invalid address\n";
        }

        create_connection();

        file.open("file.txt", std::ios::out | std::ios::binary);
        if (file.is_open()) {
            std::cout << "[LOG] : File created\n";
        } else {
            std::cerr << "[ERR] : File creating failed. Shutting down\n";
            exit(EXIT_FAILURE);
        }
    }

    void create_socket()
    {
        if ((general_socket_descriptor = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            std::cerr << "[ERR] : Socket failed\n";
            exit(EXIT_FAILURE);
        }
        std::cout << "[LOG] : Socket created successfully\n";
    }

    void create_connection()
    {
        if (connect(general_socket_descriptor, (struct sockaddr*)&address, address_length) < 0) {
            std::cerr << "[ERR] : Connection failed\n";
            exit(EXIT_FAILURE);
        }
        std::cout << "[LOG] : Connection successfully\n";
    }

    void get_file()
    {
        char buffer[1024] = "";
        int valread = read(general_socket_descriptor, buffer, 1024);
        std::cout << "[LOG] : File size is " << valread << " bytes\n";
        std::cout << "[LOG] : Saving data to txt file\n";

        file << buffer;
        std::cout << "[LOG] : File saved\n";
    }
};

int main(int argc, char* argv[])
{
    Client client(8001);
    client.get_file();
    return 0;
}
