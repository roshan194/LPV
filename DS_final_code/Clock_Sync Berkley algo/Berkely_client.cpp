#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <stdlib.h>
#include <cstdlib>
#include <ctime>
#include <vector>

#define PORT 8080

using namespace std;

vector<string> split(string s, string delimiter)
{
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    string token;
    vector<string> res;

    while ((pos_end = s.find(delimiter, pos_start)) != string::npos)
    {
        token = s.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back(token);
    }
    res.push_back(s.substr(pos_start));
    return res;
}

int main(int argc, char const *argv[])
{

    srand((unsigned int)time(NULL));
    float client_local_clock = rand() % 10;
    printf("Client starts. Client pid is %d \n", getpid());
    printf("Client local clock is %f \n\n", client_local_clock);

    int client_socket_fd, valread;
    char client_read_buffer[1024] = {0};

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    if ((client_socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Client: Socket creation error \n");
        return -1;
    }

    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0)
    {
        printf("\nClient: Invalid address/ Address not supported \n");
        return -1;
    }

    if (connect(client_socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        printf("\nClient: Connection Failed \n");
        return -1;
    }

    char server_ip[INET_ADDRSTRLEN] = "";
    inet_ntop(AF_INET, &server_addr.sin_addr, server_ip, INET_ADDRSTRLEN);
    printf("Client: connected server(%s:%d). \n", server_ip, ntohs(server_addr.sin_port));
    printf("\n\n");

    valread = read(client_socket_fd, client_read_buffer, 1024);
    printf("Client: read: '%s'\n", client_read_buffer);

    string recv_msg = string(client_read_buffer);

    if (strcmp(client_read_buffer, "Hello from server, please tell me your local clock value.") == 0)
    {
        string msg_str = "Hello from client, my local clock value is " + to_string(client_local_clock);
        char msg_char_array[msg_str.length() + 1];
        strcpy(msg_char_array, msg_str.c_str());

        send(client_socket_fd, &msg_char_array, strlen(msg_char_array), 0);
        printf("Client: sent message: '%s'\n", msg_char_array);
    }

    valread = read(client_socket_fd, client_read_buffer, 1024);
    printf("Client: read: '%s'\n", client_read_buffer);

    recv_msg = string(client_read_buffer);

    if (recv_msg.find("From server, your clock adjustment offset is") != string::npos)
    {
        string substr_after_lastbutone_space;
        string substr_after_last_space;
        vector<string> split_str = split(recv_msg, " ");
        substr_after_lastbutone_space = split_str[split_str.size() - 2];
        substr_after_last_space = split_str[split_str.size() - 1];

        cout << "Client: received local clock adjustment offset (string) is " << substr_after_lastbutone_space << " " << substr_after_last_space << endl;
        float substr_after_last_space_f = stof(substr_after_last_space);
        cout << "Client: received local clock adjustment offset (float) is " << substr_after_lastbutone_space << " " << substr_after_last_space_f << endl;

        char oper_char_array[substr_after_lastbutone_space.length() + 1];
        strcpy(oper_char_array, substr_after_lastbutone_space.c_str());
        if (strcmp(oper_char_array, "add") == 0)
        {
            client_local_clock += substr_after_last_space_f;
        }
        else if (strcmp(oper_char_array, "minus") == 0)
        {
            client_local_clock -= substr_after_last_space_f;
        }

        printf("Client local clock is %f \n\n", client_local_clock);
    }
    close(client_socket_fd);
    return 0;
}