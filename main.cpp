#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>
#include <arpa/inet.h>
#include <zconf.h>
#include <cstring>

#define PORT 8888
#define MAX_CONNECTIONS 5
#define INTERNAL_SERVER_ERROR_CODE 500

using namespace std;

int main() {

    auto serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in address;

    bzero(&address, sizeof(address));

    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    address.sin_family = AF_INET;

    if (bind(serverSocket, (sockaddr *) &address, sizeof(address)) < 0) {
        cerr << "Error while binding socket" << endl;
        exit(INTERNAL_SERVER_ERROR_CODE);
    }

    listen(serverSocket, MAX_CONNECTIONS);

    cout << "Server: successfully launched" << endl;

    char buffer[BUFSIZ];

    while (true) {
        sockaddr_in clientAddress;
        unsigned int clientLength = sizeof(clientAddress);

        auto clientSocket = accept(serverSocket, (sockaddr *) &clientAddress, &clientLength);

        if (clientSocket < 0) {
            cerr << "Error while connecting to client / client disconnected" << endl;
            exit(0);
        }

        auto address = inet_ntoa(clientAddress.sin_addr);
        auto port = ntohs(clientAddress.sin_port);

        cout << "New connection: " << address << ":" << port << endl;

        if (fork() == 0) {

            while (true) {
                int bytes = read(clientSocket, buffer, sizeof(buffer));

                if (bytes == 0 || strcasecmp("exit",buffer) == 0) {
                    cout << "connection closed" << endl;
                    break;
                }

                cout << "client: " << buffer << endl;
                send(clientSocket, buffer,strlen(buffer),0);
            }
        }
    }
}