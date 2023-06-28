#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <vector>
#include <thread>
#include <string>
#include <map>

#define USE_SSL

#include <openssl/ssl.h>
#include <openssl/err.h>

void init_openssl()
{
    SSL_load_error_strings();
    SSL_library_init();
    OpenSSL_add_all_algorithms();
}

void cleanup_openssl()
{
    ERR_free_strings();
    EVP_cleanup();
}

#define PACKET_SIZE 1024
const int MAX_CLIENTS = 10;  // 최대 클라이언트 수

#ifdef USE_SSL
    std::map<std::string, SSL*> clientMap;

    void GetClientName(SSL* ssl, int clientSocket, char* displayName)
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

        clientMap.insert({displayName, ssl});
    }
#else
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
#endif

void HandleClient(int clientSocket, SSL_CTX* sslContext) {
    // 클라이언트와의 통신 처리
    // 예: 메시지 송수신, 데이터 처리 등

    #ifdef USE_SSL
        // OpenSSL 소켓 초기화
        SSL* ssl = SSL_new(sslContext);
        SSL_set_fd(ssl, clientSocket);

        // SSL 핸드쉐이크
        if (SSL_accept(ssl) != 1) {
            std::cerr << "SSL handshake failed." << std::endl;
            return;
        }
    #endif
   
    char *displayName = new char[30];

    #ifdef USE_SSL
        GetClientName(ssl, clientSocket, displayName);
    #else
        GetClientName(clientSocket, displayName);
    #endif

    std::cout << "Connected to client! " << displayName << std::endl;

    char buffer[PACKET_SIZE];
    while(true) {
        memset(buffer, 0, sizeof(buffer));
        // 클라이언트로부터 메세지 수신
        ssize_t bytesRead;
        #ifdef USE_SSL
            bytesRead = SSL_read(ssl, (char *)buffer, PACKET_SIZE);
        #else
            bytesRead = recv(clientSocket, buffer, PACKET_SIZE, 0);
        #endif

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
        // #ifdef USE_SSL
        //     for (auto& iter : clientMap) {
        //         SSL_write(iter.second, sendBuffer, PACKET_SIZE);
        //     }
        // #else
        //     for (auto& iter : clientMap) { 
        //         send(iter.second, sendBuffer, PACKET_SIZE, 0);
        //     }
        // #endif
    }

    clientMap.erase(displayName);

    // Client 연결종료
    #ifdef USE_SSL
        SSL_shutdown(ssl);
    #endif
    close(clientSocket);

    #ifdef USE_SSL
        // OpenSSL 소켓 정리
        SSL_free(ssl);
    #endif
}

int main() {
    int serverPort = 443;

    #ifdef USE_SSL
        // OpenSSL 초기화
        init_openssl();
    #endif
    SSL_CTX *sslContext = SSL_CTX_new(TLS_server_method());

    #ifdef USE_SSL
        // 인증서 및 개인 키 로드
        if (SSL_CTX_use_certificate_file(sslContext, "./key/cert.crt", SSL_FILETYPE_PEM) != 1) {
            std::cerr << "Failed to load certificate file" << std::endl;
            return 1;
        }
        if (SSL_CTX_use_PrivateKey_file(sslContext, "./key/private.key", SSL_FILETYPE_PEM) != 1) {
            std::cerr << "Failed to load private key file" << std::endl;
            return 1;
        }
    #endif

    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Failed to create socket." << std::endl;
        return -1;
    }

    // 서버 주소 설정
    sockaddr_in serverAddress = {};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(serverPort);

    // 소켓과 서버 주소 바인딩
    if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1) {
        /* data */
        std::cerr << "Failed to bind." << std::endl;
        close(serverSocket);
        return 1;
    };

    // 소켓을 수신 대기 상태로 전환
    if (listen(serverSocket, MAX_CLIENTS) == -1) {
        std::cerr << "Failed to listen." << std::endl;
    }

    std::cout << "Waiting for client..." << std::endl;
    std::vector<std::thread> clientThread;
    while (true) {
        // 클라이언트 연결 요청 수락
        sockaddr_in clientAddress{};
        socklen_t clientAddressLength = sizeof(clientAddress);
        int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressLength);
        if (clientSocket == -1) {
            std::cerr << "Failed to accept client connection." << std::endl;
            continue;
        }

        // 클라이언트와의 통신을 별도 스레드로 처리
        std::thread clientThread(HandleClient, clientSocket, sslContext);
        clientThread.detach();
    }

    // 서버 소켓 닫기
    close(serverSocket);

    #ifdef USE_SSL
        // OpenSSL 정리
        SSL_CTX_free(sslContext);
        cleanup_openssl();
    #endif

    return 0;
}