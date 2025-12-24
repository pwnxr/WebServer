#include <bits/stdc++.h>
#include <sys/socket.h>
#include <netinet/in.h>
using namespace std;

int main(){
    // create socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    
    // bind (assign ip & port)
    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; 
    address.sin_port = htons(8080);
    bind(server_fd, (struct sockaddr*)&address, sizeof(address));
    
    // start listening on the port
    listen(server_fd, 3);

    while(true){
        // accept
        int addrlen = sizeof(address);
        int new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);

        // read request
        char buffer[1024] = {0};
        read(new_socket, buffer, 1024);
        cout << "Request received:\n" << buffer << '\n';

        // send response
        string response = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 13\n\nHello World!\n";
        send(new_socket, response.c_str(), response.length(), 0);

        // close the connection
        close(new_socket);
    }

    return 0;
}
