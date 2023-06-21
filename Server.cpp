#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <vector>
#include <thread>

#define PACKET_SIZE 1024
const int MAX_CLIENTS = 5;  // 최대 클라이언트 수

void HandleClient(int clientSocket) {
    // 클라이언트와의 통신 처리
    // 예: 메시지 송수신, 데이터 처리 등
    std::cout << "Connected to client!" << std::endl;

    char buffer[PACKET_SIZE];
    while(true) {
        memset(buffer, 0, sizeof(buffer));

        // 클라이언트로부터 메세지 수신
        ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead == -1) {
            std::cerr << "Failed to receive message" << std::endl;
            break;
        }
        if (bytesRead == 0) {
            std::cout << "disconnected to client." << std::endl;
            break;
        }

        std::cout << "Received message from client: " << buffer << std::endl;

        const char *response = "From Server: Received message.";

        // 클라이언트에게 응답 전송
        if(send(clientSocket, response, strlen(response), 0) == -1) {
            std::cerr << "Failed to send response" << std::endl;
            break;
        }
    }

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