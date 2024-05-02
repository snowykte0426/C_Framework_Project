#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <winsock.h> // socket 헤더파일  추가
#include <stdio.h>
#include <stdlib.h>
#pragma comment(lib, "ws2_32")  //socket 통신 라이브러리 추가

#define BUFSIZE 512
int server() {
	int retval;   // 윈속 초기화
	WSADATA wsa;  // 윈속 정보를 담을 구조체 
	// 윈속 버전 2.2를 사용할 수 있는지 OS에 확인
	// 사용가능하면 wsa에 값을 채워줌
	// WSAStartup 함수는 ws2_32.dll 사용 시작

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return -1;

	//------------------socket() 사용-----------------------
	//서버가 client 연결 수신 대기

	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);

	//IPv4, TCP/IP통신

	if (listen_sock == INVALID_SOCKET) {
		printf("소켓 생성 실패, 에러코드: %d\n", WSAGetLastError());
		WSACleanup();     //윈속 해제
		return 0;
	}
	printf("소켓이 생성되었습니다.\n");

	//----------------------bind()사용---------------------------
	//클라이언트가 서버의 위치를 알기 위한 IP와 Port 등록 작업

	SOCKADDR_IN serveraddr;   //socket 주소와 port를 할당하기 위해
	//SOCKETADDR_IN 구조체 사용
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;    //IPv4 주소 사용

	//이 컴퓨터에 존재하는 랜카드 중 사용 가능한 걸 자동 선택
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(9000);

	if (bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr)) == -1) {
		printf("bind() 실행 에러 \n");
		exit(1);
	}
	else {
		printf("bind 성공\n");
	}

	//---------------------------listen()-------------------------------

	listen(listen_sock, SOMAXCONN); //SOMAXCONN: 맥시멈 큐 사이즈
	printf("연결 대기 중....");

	//---------------------------------------------------------------
	//통신에 사용할 변수
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;
	char buf[BUFSIZE + 1];

	//--------------------------accept()------------------------------
	addrlen = sizeof(clientaddr);
	client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
	printf("요청 수신\n");

	//----------------------------recv()------------------------------
	retval = recv(client_sock, buf, BUFSIZE, 0);
	printf("메세지 수신\n");

	//------------------------받은 데이터 출력------------------------------
	buf[retval] = '\0';
	printf("[TCP/%s:%d] %s\n",
		inet_ntoa(clientaddr.sin_addr),
		ntohs(clientaddr.sin_port), buf);

	//----------------------------close socket------------------------------
	closesocket(listen_sock);

	//----------------------------윈속 종료----------------------------------

	WSACleanup();

	return 0;
}