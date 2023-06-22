#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <vector>
#include <thread>
#include <string>
#include <map>

#define PACKET_SIZE 1024
const int MAX_CLIENTS = 10;  // 최대 클라이언트 수
std::map<std::string, int> clientMap;

void GetClientName(int clientSocket, char* displayName)
{
    struct sockaddr_in clientAddress;
    socklen_t clientAddressLength = sizeof(clientAddress);
    getpeername(clientSocket, (struct sockaddr *)&clientAddress, &clientAddressLength);

    char clientIP[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(clientAddress.sin_addr), clientIP, INET_ADDRSTRLEN);
    int clientPort = ntohs(clientAddress.sin_port);
    std::string portStr = std::to_string(clientPort);

    strcpy(displayName, "(");
    strcat(displayName, clientIP);
    strcat(displayName, ":");
    strcat(displayName, portStr.c_str());
    strcat(displayName, ")");

    clientMap.insert({displayName, clientSocket});
}

void HandleClient(int clientSocket) {
    // 클라이언트와의 통신 처리
    // 예: 메시지 송수신, 데이터 처리 등
   
    char *displayName = new char[30];
    GetClientName(clientSocket, displayName);

    std::cout << "Connected to client! " << displayName << std::endl;

    char buffer[PACKET_SIZE];
    while(true) {
        memset(buffer, 0, sizeof(buffer));

        // 클라이언트로부터 메세지 수신
        ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead == -1) {
            std::cerr << "Failed to receive message" << displayName <<std::endl;
            break;
        }
        if (bytesRead == 0) {
            std::cout << "disconnected to client." << displayName << std::endl;
            break;
        }

        std::cout << "Received message from client" << displayName << ": " <<buffer << std::endl;

        // 클라이언트에게 응답 전송
        char* sendBuffer = new char[PACKET_SIZE];
        strcpy(sendBuffer, displayName);
        strcat(sendBuffer, ": ");
        strcat(sendBuffer, buffer);
        for (auto& iter : clientMap) {
            send(iter.second, sendBuffer, PACKET_SIZE, 0);
        }

        // const char *response = "this is server.";
        // if(send(clientSocket, response, strlen(response), 0) == -1) {
        // if(send(clientSocket, buffer, PACKET_SIZE, 0) == -1) {
        //     std::cerr << "Failed to send response" << displayName << std::endl;
        //     break;
        // }
    }

    clientMap.erase(displayName);
    close(clientSocket);
}

int main() {
    int serverPort = 12345;

    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Failed to create socket." << std::endl;
        return -1;
    }

    sockaddr_in serverAddress = {};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(serverPort);

    if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1)
    {
        /* data */
        std::cerr << "Failed to bind." << std::endl;
        close(serverSocket);
        return 1;
    };

    if (listen(serverSocket, MAX_CLIENTS) == -1) {
        std::cerr << "Failed to listen." << std::endl;
    }

    std::cout << "Waiting for client..." << std::endl;
    std::vector<std::thread> clientThread;
    while (true) {
        sockaddr_in clientAddress{};
        socklen_t clientAddressLength = sizeof(clientAddress);
        int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressLength);
        if (clientSocket == -1) {
            std::cerr << "Failed to accept client connection." << std::endl;
            continue;
        }

        // 클라이언트와의 통신을 별도 스레드로 처리
        std::thread clientThread(HandleClient, clientSocket);
        clientThread.detach();
    }

    // 서버 소켓 닫기
    close(serverSocket);

    return 0;
}