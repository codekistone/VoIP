#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <vector>
#include <thread>
#include <string>
#include <map>
#include <cstring>

#define PACKET_SIZE 1024

WSADATA wsa;
const int MAX_CLIENTS = 10;  // 최대 클라이언트 수
std::map<std::string, int> clientMap;

std::string GetClientName(int clientSocket)
{
    struct sockaddr_in clientAddress;
    socklen_t clientAddressLength = sizeof(clientAddress);

    getpeername(clientSocket, (struct sockaddr*)&clientAddress, &clientAddressLength);

    char clientIP[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(clientAddress.sin_addr), clientIP, INET_ADDRSTRLEN);
    int clientPort = ntohs(clientAddress.sin_port);

    std::string ip(clientIP);
    std::string portStr = std::to_string(clientPort);
    std::string displayName = "(" + ip + ":" + portStr + ")";

    clientMap.insert({ displayName, clientSocket });
    return displayName;
}

void HandleClient(int clientSocket) {
    // 클라이언트와의 통신 처리
    // 예: 메시지 송수신, 데이터 처리 등

    std::string displayName = GetClientName(clientSocket);

    std::cout << "Connected to client! " << displayName << std::endl;

    char buffer[PACKET_SIZE];
    while (true) {
        memset(buffer, 0, sizeof(buffer));

        // 클라이언트로부터 메세지 수신
        SSIZE_T bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead == -1) {
            std::cerr << "Failed to receive message" << displayName << std::endl;
            break;
        }
        if (bytesRead == 0) {
            std::cout << "disconnected to client." << displayName << std::endl;
            break;
        }

        std::cout << "Received message from client" << displayName << ": " << buffer << std::endl;

        // 클라이언트에게 응답 전송
        char sendBuffer[PACKET_SIZE];
        strcpy_s(sendBuffer, PACKET_SIZE, (displayName + ": ").c_str());
        strcat_s(sendBuffer, sizeof(sendBuffer), buffer);
        for (auto& iter : clientMap) {
            send(iter.second, sendBuffer, PACKET_SIZE, 0);
        }
    }

    clientMap.erase(displayName);
    closesocket(clientSocket);
}

int main() {
    int serverPort = 5555;

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Failed to initialize Winsock." << std::endl;
        return -1;
    }

    SOCKET serverSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == -1) {
        std::cerr << "Failed to create socket." << std::endl;
        return -1;
    }

    SOCKADDR_IN serverAddress = {};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(serverPort);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1)
    {
        /* data */
        std::cerr << "Failed to bind." << std::endl;
        closesocket(serverSocket);
        return 1;
    };

    if (listen(serverSocket, MAX_CLIENTS) == -1) {
        std::cerr << "Failed to listen." << std::endl;
    }

    std::cout << "Waiting for client..." << std::endl;
    std::vector<std::thread> clientThread;
    while (true) {
        SOCKADDR_IN clientAddress{};
        int clientAddressLengh = sizeof(clientAddress);
        SOCKET clientSocket = accept(serverSocket, (SOCKADDR*)&clientAddress, &clientAddressLengh);
        if (clientSocket == -1) {
            std::cerr << "Failed to accept client connection." << std::endl;
            continue;
        }

        // 클라이언트와의 통신을 별도 스레드로 처리
        std::thread clientThread(HandleClient, clientSocket);
        clientThread.detach();
    }

    // 서버 소켓 닫기
    closesocket(serverSocket);

    return 0;
}