#include <iostream>
#include <string>
#include <winsock2.h>
#include <WS2tcpip.h>
#include <thread>
using namespace std;

#define PACKET_SIZE 1024

void proc_recv(int clientSocket) {
    char buf[PACKET_SIZE];
    while(true) {
        memset(buf, 0, sizeof(buf));
        SSIZE_T byteRead = recv(clientSocket, buf, PACKET_SIZE, 0);
        if (byteRead == -1) {
            std::cerr << "Failed to receive response." << std::endl;
            break;
        }
        if (byteRead == 0) {
            std::cout << "Disconnected to server." << std::endl;
            // break;
            exit(0);
        }

        std::cout << "Received message from server: " << buf << std::endl;
    }
}

void openSocket(char IP[], int PORT) { //IP�ּ�,��Ʈ��ȣ ����!
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa)) {
		cout << "WSA error";
		WSACleanup();
		return;
	}

	SOCKET skt;
	skt = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (skt == INVALID_SOCKET) {
		cout << "socket error";
		closesocket(skt);
		WSACleanup();
		return;
	}

	SOCKADDR_IN addr = {};
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT); //��Ʈ��ȣ
	//addr.sin_addr.s_addr = inet_addr(IP); // IP�ּ�
	inet_pton(AF_INET, IP, &(addr.sin_addr.s_addr));

	while (connect(skt, (SOCKADDR*)&addr, sizeof(addr)));

	cout << "Connected to server!" << endl;
	
	thread recvThread(proc_recv, skt);

	// char buf[PACKET_SIZE];

	while (!WSAGetLastError()) {
		cout << "Enter message: ";
		string message;
		getline(cin, message);

		if (send(skt, message.c_str(), message.length(), 0) == -1) {
			std::cerr << "Fail to send message" << std::endl;
			break;
		}
		if (message.empty()) {
			std::cout << "Terminate connection" << std::endl;
			break;
		}

		//memset(buf, 0, sizeof(buf));
		//SSIZE_T byteRead = recv(skt, buf, PACKET_SIZE, 0);
		//if (byteRead == -1) {
		//	std::cerr << "Failed to receive response." << std::endl;
		//	break;
		//}
		//if (byteRead == 0) {
		//	std::cout << "Disconnected to server." << std::endl;
		//	break;
		//}

		//std::cout << "Received message from server: " << buf << std::endl;
	}
	recvThread.join();

	closesocket(skt);
	WSACleanup();
}

int main() {
	SetConsoleOutputCP(CP_UTF8);
	char IP[100];
	int PORT;
	cout << "Input serverIP: ";
	cin >> IP; // ������ �ּ�
	cout << "input serverPort: ";
	cin >> PORT; // ��Ʈ ��ȣ
	cin.ignore();

	openSocket(IP, PORT); //���� ����!
	return 0;
}