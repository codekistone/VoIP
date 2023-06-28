#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>

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

void proc_recv(int clientSocket, SSL* ssl) {
    // 서버의 통신 로직을 구현합니다.
    // 예: 메세지 송수신, 데이터 처리 등
    char buffer[PACKET_SIZE];
    while(true) {
        memset(buffer, 0, sizeof(buffer));

        size_t bytesRead;
        #ifdef USE_SSL
            bytesRead = SSL_read(ssl, (char *)buffer, PACKET_SIZE);
        #else
            bytesRead = recv(clientSocket, buffer, PACKET_SIZE, 0);
        #endif

        if (bytesRead == -1) {
            std::cerr << "Failed to receive response." << std::endl;
            break;
        }
        if (bytesRead == 0) {
            std::cout << "Disconnected to server." << std::endl;
            break;
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

    #ifdef USE_SSL
        // OpenSSL 초기화
        init_openssl();
        SSL_CTX* sslContext = SSL_CTX_new(TLS_client_method());
    #endif

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


    SSL* ssl;
    #ifdef USE_SSL
        // OpenSSL 소켓 초기화
        ssl = SSL_new(sslContext);
        SSL_set_fd(ssl, clientSocket);

        // SSL 핸드쉐이크
        if (SSL_connect(ssl) != 1) {
            std::cerr << "SSL handshake failed." << std::endl;
            return 1;
        }
    #endif

    // 연결이 성공적으로 수행됨
    std::cout << "Connected to server!" << std::endl;

    std::thread recvThread(proc_recv, clientSocket, ssl);

    // 서버의 통신 로직을 구현합니다.
    // 예: 메세지 송수신, 데이터 처리 등
    // char buffer[PACKET_SIZE];
    while (true)
    {
        std::cout << "Enter message: ";
        std::string message;
        std::getline(std::cin, message);

        ssize_t bytesSend;
        #ifdef USE_SSL
            bytesSend = SSL_write(ssl, message.c_str(), message.length());
        #else
            bytesSend = send(clientSocket, message.c_str(), message.length(), 0);
        #endif

        if (bytesSend == -1)
        {
            std::cerr << "Fail to send message" << std::endl;
            break;
        }

        if (message.empty()) {
            std::cout << "Terminate connection" << std::endl;
            pthread_cancel(recvThread.native_handle());
            break;
        }
    }
    recvThread.join();

    #ifdef USE_SSL
        // 연결 종료
        SSL_shutdown(ssl);
    #endif

    close(clientSocket);

    #ifdef USE_SSL
        // OpenSSL 정리
        SSL_CTX_free(sslContext);
        cleanup_openssl();
    #endif

    return 0;
}