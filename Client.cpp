#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>

#define PACKET_SIZE 1024

void proc_recv(int clientSocket) {

    // 서버의 통신 로직을 구현합니다.
    // 예: 메세지 송수신, 데이터 처리 등
    char buffer[PACKET_SIZE];
    while(true) {
        memset(buffer, 0, sizeof(buffer));
        ssize_t byteRead = recv(clientSocket, buffer, PACKET_SIZE, 0);
        if (byteRead == -1) {
            std::cerr << "Failed to receive response." << std::endl;
            break;
        }
        if (byteRead == 0) {
            std::cout << "Disconnected to server." << std::endl;
            // break;
            exit(0);
        }

        std::cout << "Received message from server: " << buffer << std::endl;
    }
}

int main() {
    // 서버 정보
    char serverIP[100];
    int serverPort;
    // std::string serverIP = "127.0.0.1";
    // int serverPort = 12345;
    std::cout << "Input serverIP: ";
    std::cin >> serverIP;
    std::cout << "input serverPort: ";
    std::cin >> serverPort;
    std::cin.ignore();

    // 소켓 생성
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        std::cerr << "Failed to create socket." << std::endl;
        return 1;
    }

    // 서버 주소 설정
    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(serverPort);
    if (inet_pton(AF_INET, serverIP, &(serverAddress.sin_addr)) <= 0) {
        std::cerr << "Invalid serverIP address." << std::endl;
        close(clientSocket);
        return 1;
    }

    // 서버에 연결
    if (connect(clientSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1) {
        std::cerr << "Failed to connect server." << std::endl;
        close(clientSocket);
        return 1;
    }

    // 연결이 성공적으로 수행됨
    std::cout << "Connected to server!" << std::endl;

    std::thread recvThread(proc_recv, clientSocket);

    // 서버의 통신 로직을 구현합니다.
    // 예: 메세지 송수신, 데이터 처리 등
    // char buffer[PACKET_SIZE];
    while (true)
    {
        std::cout << "Enter message: ";
        std::string message;
        std::getline(std::cin, message);

        if (send(clientSocket, message.c_str(), message.length(), 0) == -1)
        {
            std::cerr << "Fail to send message" << std::endl;
            break;
        }

        if (message.empty()) {
            std::cout << "Terminate connection" << std::endl;
            break;
        }

        // memset(buffer, 0, sizeof(buffer));
        // ssize_t byteRead = recv(clientSocket, buffer, PACKET_SIZE, 0);
        // if (byteRead == -1) {
        //     std::cerr << "Failed to receive response." << std::endl;
        //     break;
        // }
        // if (byteRead == 0) {
        //     std::cout << "Disconnected to server." << std::endl;
        //     break;
        // }

        // std::cout << "Received message from server: " << buffer << std::endl;
    }
    recvThread.join();

    // 연결 종료
    close(clientSocket);

    return 0;
}