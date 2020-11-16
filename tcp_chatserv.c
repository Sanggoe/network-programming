//--------------------------------------------------------
// 파일명 : tcp_chatserv.c
// 기  능 : 채팅 참가자 관리, 채팅 메시지 수신 및 방송
// 컴파일 : cc -o chat_server tcp_chatserv.c
// 사용법 : chat_server 4001
//--------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <strings.h>
#include <fcntl.h>
#include <netdb.h>
// #include <sys/file.h>
//#include <unistd.h>

#define MAXLINE 511
#define MAX_SOCK 1024	// 솔라리스의 경우 64

char *EXIT_STRING = "exit"; // 클라이언트의 종료 요청 문자열
char *START_STRING = "Connected to chat_server \n"; // 클라이언트 환영 메시지
int maxfdp1;			// 최대 소켓번호 +1
int num_chat = 0;		// 채팅 참가자 수
int clisock_list[MAX_SOCK];	// 채팅 참가자 소켓 번호 목록
int listen_sock;		// 서버의 리슨 소켓

void addClient(int s, struct sockaddr_in *newcliaddr); // 새로운 채팅 참가자 처리
int getmax();			// 최대 소켓번호 찾기
void removeClient(int s);	// 채팅 탈퇴 처리 함수
int tcp_listen(int host, int port, int backlog); // 소켓 생성 및 listen
void errquit(char *mesg) {	// 에러시 종료 함수
	perror(mesg);
	exit(1);
}

int main(int argc, char* argv[]) {
	struct sockaddr_in cliaddr;
	char buf[MAXLINE+1];
	int i, j, nbyte, accp_sock, addrlen = sizeof(struct sockaddr_in);
	fd_set read_fds;	// 읽기를 감지할 fd_set 구조체

	if(argc != 2) {
		printf("사용법 : %s port\n", argv[0]);
		exit(0);
	}

	// tcp_listen(host, port, backlog) 함수 호출
	listen_sock = tcp_listen(INADDR_ANY, atoi(argv[1]), 5);
	

	close(listen_sock);
	return 0;
}


// 새로운 채팅 참가자 처리
void addClient(int s, struct sockaddr_in *newcliaddr) {

}

// 최대 소켓번호 찾기
int getmax() {

}

// 채팅 탈퇴 처리 함수
void removeClient(int s) {

}

// 소켓 생성 및 listen
int tcp_listen(int host, int port, int backlog){
	int sd;
	struct sockaddr_in servaddr;

	sd = socket(AF_INET, SOCK_STREAM, 0);
	if (sd == -1) {
		perror("socket fail");
		exit(1);
	}
	// servaddr 구조체의 내용 셋팅
	bzero((char *)&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(host);
	servaddr.sin_port = htons(port);

	if (bind(sd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
		perror("bind fail");
		exit(1);
	}

	// 클라이언트로부터 연결 요청을 기다림
	listen(sd, backlog);
	return sd;
}
