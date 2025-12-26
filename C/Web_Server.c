#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdbool.h>

int main() {
    // 1. Create socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    
    // 2. Bind
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; 
    address.sin_port = htons(8080);
    bind(server_fd, (struct sockaddr*)&address, sizeof(address));
    
    // 3. Listen
    listen(server_fd, 3);

    printf("Waiting for the POST requests...\n");

    while(1) {
        int addrlen = sizeof(address);
        int new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);

        // 4. Read request
        char buffer[2048] = {0};
        read(new_socket, buffer, 1024);
        printf("Request received:\n%s\n", buffer);

        // 5. Extract user & pass 
        char *body_pos = strstr(buffer, "\r\n\r\n");
        if(body_pos != NULL) {
            char *body = body_pos + 4;
            printf("Extracted Body: %s\n", body);

            bool logged_in = false;
            char *user_start = strstr(body, "user=");
            char *pass_start = strstr(body, "&pass=");

            if (user_start != NULL && pass_start != NULL) {
                char user_input[100] = {0};
                char pass_input[100] = {0};

                int u_len = pass_start - (user_start + 5);
                strncpy(user_input, user_start + 5, u_len);
                strcpy(pass_input, pass_start + 6);

                FILE *users_f = fopen("users.txt", "r");
                FILE *passs_f = fopen("pass.txt", "r");
                char user[100], pass[100];

                if(users_f && passs_f) {
                    while(fscanf(users_f, "%s", user) != EOF && fscanf(passs_f, "%s", pass) != EOF) {
                        if(strcmp(user_input, user) == 0 && strcmp(pass_input, pass) == 0) {
                            logged_in = true;
                            break;
                        }
                    }
                    fclose(users_f);
                    fclose(passs_f);
                }

                if(logged_in) {
                    char filename[110];
                    sprintf(filename, "%s.txt", user_input);
                    FILE *user_file = fopen(filename, "r");
                    
                    char file_content[1024] = {0};
                    if(user_file) {
                        char line[100];
                        while(fgets(line, sizeof(line), user_file)) {
                            strcat(file_content, line);
                        }
                        fclose(user_file);
                    }
                    
                    char response[2048];
                    sprintf(response, "HTTP/1.1 200 OK\nContent-Type: text/plain\n\nWelcome! Your data:\n%s\n", file_content);
                    send(new_socket, response, strlen(response), 0);
                } else {
                    char *resp = "HTTP/1.1 200 OK\nContent-Type: text/plain\n\nWrong password or username\n";
                    send(new_socket, resp, strlen(resp), 0);
                }
            }else{
                char *resp = "HTTP/1.1 200 OK\nContent-Type: text/plain\n\nBad request\n";
                send(new_socket, resp, strlen(resp), 0);
            }
        }
        close(new_socket);
    }
    return 0;
}
