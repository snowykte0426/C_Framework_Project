#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <time.h>
#pragma comment(lib, "ws2_32")
void WelcomeMessage() {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
    printf(" _____   _                            _   _         _         \n"
        "/  __ \\ | |                          | \\ | |       | |        \n"
        "| /  \\/ | |      __ _  _ __    __ _  |  \\| |  ___  | |_   ___ \n"
        "| |     | |     / _` || '_ \\  / _` | | . ` | / _ \\ | __| / _ \\\n"
        "| \\__/\\ | |____| (_| || | | || (_| | | |\\  || (_) || |_ |  __/\n"
        " \\____/ \\_____/ \\__,_||_| |_| \\__, | \\_| \\_/ \\___/  \\__| \\___|\n"
        "                               __/ |                          \n"
        "                              |___/                           \n");
    printf(
        "_______ _______  ______   _    _ _______  ______       _______ _____  __   _  ______ _______        _______\n"
        "|______ |______ |_____ /   \\  /  |______ |_____/       |      |     | | \\  | |______ |     | |      |______\n"
        "______| |______ |      \\_   \\/   |______ |     \\_      |_____ |_____| |  \\_| ______| |_____| |_____ |______\n\n\n\n");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
}
#define BUFSIZE 512
#define HTMFILE "index.htm"
SOCKET listen_sock;
DWORD WINAPI ListenThread(LPVOID arg) {
    time_t timer = time(NULL);
    SOCKADDR_IN clientaddr;
    int addrlen;
    SOCKET client_sock;
    char buf[BUFSIZE + 1];
    int retval;
    while (1) {
        addrlen = sizeof(clientaddr);
        client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
        if (client_sock == INVALID_SOCKET) {
            printf("accept() 실행 에러\n");
            continue;
        }
        printf("--------------------------------------\n요청 수신\n--------------------------------------\n");
        retval = recv(client_sock, buf, BUFSIZE, 0);
        if (retval == SOCKET_ERROR) {
            printf("recv() 실행 에러\n");
            closesocket(client_sock);
            continue;
        }
        struct tm* user_time;
        timer = time(NULL);
        user_time = localtime(&timer);
        buf[retval] = '\0';
        printf("<%d:%d:%d>[TCP/%s:%d]\n%s\n",user_time->tm_hour,user_time->tm_min,user_time->tm_sec, inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port), buf);
        if (strstr(buf, "GET / ") || strstr(buf, "GET /index.htm ")) {
            FILE* file = fopen(HTMFILE, "r");
            if (file) {
                fseek(file, 0, SEEK_END);
                long filesize = ftell(file);
                fseek(file, 0, SEEK_SET);
                char* filebuffer = (char*)malloc(filesize + 1);
                fread(filebuffer, filesize, 1, file);
                filebuffer[filesize] = '\0';
                char header[1024];
                sprintf(header, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: %ld\r\nConnection: close\r\n\r\n", filesize);
                send(client_sock, header, strlen(header), 0);
                send(client_sock, filebuffer, filesize, 0);
                free(filebuffer);
                fclose(file);
            }
            else {
                char response[] = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n<html><body><h1>404 Not Found</h1></body></html>";
                send(client_sock, response, strlen(response), 0);
            }
        }
        else {
            char response[] = "HTTP/1.1 400 Bad Request\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n<html><body><h1>400 Bad Request</h1></body></html>";
            send(client_sock, response, strlen(response), 0);
        }
        closesocket(client_sock);
    }
    return 0;
}
int main() {
    WelcomeMessage();
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        return -1;

    listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sock == INVALID_SOCKET) {
        printf("소켓 생성 실패, 에러코드: %d\n", WSAGetLastError());
        WSACleanup();
        return 0;
    }
    SOCKADDR_IN serveraddr;
    ZeroMemory(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(9000);
    if (bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr)) == -1) {
        printf("bind() 실행 에러\n");
        closesocket(listen_sock);
        WSACleanup();
        return 1;
    }
    if (listen(listen_sock, SOMAXCONN) == -1) {
        printf("listen() 실행 에러\n");
        closesocket(listen_sock);
        WSACleanup();
        return 1;
    }
    printf("연결 대기 중....\n");
    CreateThread(NULL, 0, ListenThread, NULL, 0, NULL);
    char cmd[10];
    do {
        printf("서버를 종료하려면 'exit'을 입력하세요.\n");
        scanf("%s", cmd);
    } while (strcmp(cmd, "exit") != 0);
    closesocket(listen_sock);
    WSACleanup();
    return 0;
}