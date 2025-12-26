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

    cout << "Waiting for the POST requests...\n";

    while(true){
        // accept
        int addrlen = sizeof(address);
        int new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);

        // read request
        char buffer[2048] = {0};
        read(new_socket, buffer, 2047);
        cout << "Request received:\n" << buffer << '\n';

        // extract user & pass
        string request(buffer);
        size_t body_pos = request.find("\r\n\r\n");
        if(body_pos != string::npos){
            string body = request.substr(body_pos + 4);
            cout << "Extracted Body: " << body << '\n';

            bool loged_in = false;
            size_t user_start = body.find("user=");
            size_t pass_start = body.find("&pass=");
            if (user_start != string::npos && pass_start != string::npos) {
                string user_input = body.substr(user_start + 5, pass_start - (user_start + 5));
                string pass_input = body.substr(pass_start + 6);

                ifstream users("users.txt");
                ifstream passwords("pass.txt");
                string user, pass;
                while(users >> user && passwords >> pass){
                    // cout << user << " : " << pass << '\n';
                    if(user_input == user && pass_input == pass){
                        loged_in = true;
                        break;
                    }
                }

                if(loged_in){
                    // send the user's file
                    ifstream user_file(user + ".txt");
                    string file_content, line;
                    while(getline(user_file, line)) file_content += line + "\n";
                    string response = "HTTP/1.1 200 OK\nContent-Type: text/plain\n\nWelcome! Your data:\n" + file_content;
                    send(new_socket, response.c_str(), response.length(), 0);
                }else{
                    // login failed
                    string response = "HTTP/1.1 200 OK\nContent-Type: text/plain\n\nWrong password or username\n";
                    send(new_socket, response.c_str(), response.length(), 0); 
                }
            }else{
                // bad request
                string response = "HTTP/1.1 200 OK\nContent-Type: text/plain\n\nBad request\n";
                send(new_socket, response.c_str(), response.length(), 0); 
            }
        }else{
            // bad request
            string response = "HTTP/1.1 200 OK\nContent-Type: text/plain\n\nBad request\n";
            send(new_socket, response.c_str(), response.length(), 0);   
        }

        // close the connection
        close(new_socket);
    }

    return 0;
}