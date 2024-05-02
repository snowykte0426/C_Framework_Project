#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <winsock.h> // socket �������  �߰�
#include <stdio.h>
#include <stdlib.h>
#pragma comment(lib, "ws2_32")  //socket ��� ���̺귯�� �߰�

#define BUFSIZE 512
int main() {
	int retval;   // ���� �ʱ�ȭ
	WSADATA wsa;  // ���� ������ ���� ����ü 
	// ���� ���� 2.2�� ����� �� �ִ��� OS�� Ȯ��
	// ��밡���ϸ� wsa�� ���� ä����
	// WSAStartup �Լ��� ws2_32.dll ��� ����

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return -1;

	//------------------socket() ���-----------------------
	//������ client ���� ���� ���

	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);

	//IPv4, TCP/IP���

	if (listen_sock == INVALID_SOCKET) {
		printf("���� ���� ����, �����ڵ�: %d\n", WSAGetLastError());
		WSACleanup();     //���� ����
		return 0;
	}
	printf("������ �����Ǿ����ϴ�.\n");

	//----------------------bind()���---------------------------
	//Ŭ���̾�Ʈ�� ������ ��ġ�� �˱� ���� IP�� Port ��� �۾�

	SOCKADDR_IN serveraddr;   //socket �ּҿ� port�� �Ҵ��ϱ� ����
	//SOCKETADDR_IN ����ü ���
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;    //IPv4 �ּ� ���

	//�� ��ǻ�Ϳ� �����ϴ� ��ī�� �� ��� ������ �� �ڵ� ����
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(9000);

	if (bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr)) == -1) {
		printf("bind() ���� ���� \n");
		exit(1);
	}
	else {
		printf("bind ����\n");
	}
	char check[10];
	//---------------------------listen()-------------------------------
	while (1) {
		listen(listen_sock, SOMAXCONN); //SOMAXCONN: �ƽø� ť ������
		printf("���� ��� ��....");
		//---------------------------------------------------------------
		//��ſ� ����� ����
		SOCKET client_sock;
		SOCKADDR_IN clientaddr;
		int addrlen;
		char buf[BUFSIZE + 1];

		//--------------------------accept()------------------------------
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
		printf("��û ����\n");

		//----------------------------recv()------------------------------
		retval = recv(client_sock, buf, BUFSIZE, 0);
		printf("�޼��� ����\n");

		//------------------------���� ������ ���------------------------------
		buf[retval] = '\0';
		printf("[TCP/%s:%d] %s\n",
			inet_ntoa(clientaddr.sin_addr),
			ntohs(clientaddr.sin_port), buf);

		//----------------------------close socket------------------------------
		closesocket(listen_sock);

		//----------------------------���� ����----------------------------------
	}
	WSACleanup();

	return 0;
}
