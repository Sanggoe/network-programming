//--------------------------------------------------------------------
// 파일명 : tcp_chatserv1.c
// 기  능 : 채팅 참가자 관리, 채팅 메시지 수신 및 방송
// 수  정 : 자신에게는 메시지 보내지 않기, maxfdp1 필요한 곳에만 호출
// 컴파일 : cc -o tcp_chatserv1 tcp_chatserv1.c
// 사용법 : tcp_chatserv 5008
//-------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <strings.h>
#include <fcntl.h>
#include <netdb.h>
// #include <sys/file.h>
//#include <unistd.h>

#define MAXLINE 511
#define MAX_SOCK 1024

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
	
	maxfdp1 = listen_sock+1; // maxfdp1 초기값 대입을 위해 호출
	while(1) {
		FD_ZERO(&read_fds);
		FD_SET(listen_sock, &read_fds);
		for(i=0; i<num_chat; i++) {
			FD_SET(clisock_list[i], &read_fds);
	       	}
		puts("wait for client");
		// 여기까지 하면 client 기다릴 준비 완료
		if(select(maxfdp1, &read_fds, NULL, NULL, NULL) < 0) { // block
			errquit("select fail");
		} // select return시 어떤 소켓에서 변화가 생겼다는 뜻!
	
		// listen_sock의 변화이면 새 클라이언트 추가
		if(FD_ISSET(listen_sock, &read_fds)) {	
			accp_sock = accept(listen_sock, (struct sockaddr*)&cliaddr, &addrlen);
			if(accp_sock == -1) {
				errquit("accept fail");
			}
			addClient(accp_sock, &cliaddr);
			send(accp_sock, START_STRING, strlen(START_STRING), 0);
			printf("%d번째 사용자 추가.\n", num_chat);
		}

		// 클라이언트가 보낸 메시지를 모든 클라이언트에게 방송
		for(i=0; i<num_chat; i++) {
			// 특정 클라이언트의 소켓에 변화가 있으면
			if(FD_ISSET(clisock_list[i], &read_fds)) {
				nbyte = recv(clisock_list[i], buf, MAXLINE, 0);
				if(nbyte <= 0) { // 메시지 문제 발생시
					removeClient(i); // 클라이언트 중료
					continue;
				}
				buf[nbyte] = 0;
				// 종료문자 처리
				if(strstr(buf, EXIT_STRING) != NULL) {
					removeClient(i); // 클라이언트 종료
					continue;
				}
				// 모든 채팅 참가자에게 메시지 방송
				for(j=0; j<num_chat; j++) {
					if(j == i) // 내가 보낸 메시지는 수신하면 안되지! loopback 방지
						continue;
					send(clisock_list[j], buf, nbyte, 0);
				}
				printf("%s\n", buf);
			}
		}
	} // end of while
	return 0;
}


// 새로운 채팅 참가자 처리
void addClient(int s, struct sockaddr_in *newcliaddr) {
	char buf[20];
	inet_ntop(AF_INET, &newcliaddr->sin_addr, buf, sizeof(buf));
	printf("new client: %s\n", buf); //, %d\n", buf, ntohs(newcliaddr->sin_port));
	// 채팅 클라이언트 목록에 추가
	clisock_list[num_chat] = s;
	num_chat++;
	maxfdp1 = getmax() + 1; // maxfdp1 재계산
}

// 최대 소켓번호 찾기
int getmax() {
	// MInimum 소켓 번호는 가장 먼저 생성된 listen_sock
	int max = listen_sock;
	int i;
	for (i=0; i<num_chat; i++) {
		if(clisock_list[i] > max) {
			max = clisock_list[i];
		}
	}
	return max;
}

// 채팅 탈퇴 처리 함수
void removeClient(int s) {
	close(clisock_list[s]);
	if(s != num_chat-1) {
		clisock_list[s] = clisock_list[num_chat-1];
	} else { // s가 마지막에 들어온 소켓이었을 경우
		maxfdp1 = getmax() + 1; // maxfdp1 재계산
	}
	num_chat--;
	printf("채팅 참가자 1명 탈퇴. 현재 참가자 수 = %d\n", num_chat);
}

// 소켓 생성 및 listen
int tcp_listen(int host, int port, int backlog){
	int sd;
	struct sockaddr_in servaddr;

	sd = socket(AF_INET, SOCK_STREAM, 0);
	if (sd == -1) {
		errquit("socket fail");
	}
	// servaddr 구조체의 내용 셋팅
	bzero((char *)&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(host);
	servaddr.sin_port = htons(port);

	if (bind(sd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
		errquit("bind fail");
	}

	// 클라이언트로부터 연결 요청을 기다림
	listen(sd, backlog);
	return sd;
}
