//---------------------------------------------
// 파일명 : tcp_talkcli.c
// 기  능 : 토크 서버와 1:1 통신을 한다
// 컴파일 : cc -o tcp_talkcli tcp_talkcli.c
// 사용법 : tcp_talkcli 127.0.0.1 3008
//---------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <signal.h>

#define MAXLINE 511

char *EXIT_STRING = "exit";	// 종료문자
int recv_and_print(int sd, struct sockaddr_in servaddr, int* addrlen);	// 상대로부터 메시지 수신 후 화면 출력
int input_and_send(int sd, struct sockaddr_in servaddr, int addrlen);	// 키보드 입력 받고 상대에게 메시지 전달

int main(int argc, char *argv[]) {
	pid_t pid;
	struct sockaddr_in servaddr;
	int s, nbyte, addrlen = sizeof(servaddr);

	// 명령문 입력 인자 처리
	if(argc != 3) {
		printf("Usage: %s server_ip port_number\n", argv[0]);
		exit(0);
	}

	// 소켓 생성
	if((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
		printf("Client: Can't open dgram socket. \n");
		exit(1);
	}
	// 서버의 소켓주소 구조체 초기화
	bzero((char*)&servaddr, sizeof(servaddr));

	// servaddr 셋팅
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(argv[1]);
	servaddr.sin_port = htons(atoi(argv[2]));

	if ((pid = fork()) > 0) { // 부모 프로세스
		input_and_send(s, servaddr, addrlen);
	} else if (pid == 0) { // 자식 프로세스
		recv_and_print(s, servaddr, &addrlen);
	}
	close(s);
	return 0;
}

// 키보드 입력받고 상대에게 메시지 전달
int input_and_send(int sd, struct sockaddr_in servaddr, int addrlen) {
	char buf[MAXLINE+1];
	int nbyte;
	
	while(fgets(buf, sizeof(buf), stdin) != NULL) {
		nbyte = strlen(buf);
		if (sendto(sd, buf, strlen(buf), 0, (struct sockaddr*)&servaddr, addrlen) < 0) {
			perror("sendto fail");
			close(sd);
			exit(1);
                }
		// 종료 문자열 처리
		if (strstr(buf, EXIT_STRING) != NULL) {
			puts("Good bye.");
			close(sd);
			exit(0);
		}
	}
	return 0;
}

// 상대로부터 메시지 수신 후 화면 출력
int recv_and_print(int sd, struct sockaddr_in servaddr, int* addrlen) {
	char buf[MAXLINE+1];
	int nbyte;
	
	while(1) {
		if ((nbyte = recvfrom(sd, buf, MAXLINE, 0, (struct sockaddr*)&servaddr, addrlen)) < 0) {
			perror("recvfail fail");
			close(sd);
			exit(0);
		}
		buf[nbyte] = 0;

		// 종료문자열 수신시 종료
		if (strstr(buf, EXIT_STRING) != NULL) {
			puts("Good bye, client! Server out.");
			close(sd);
			kill(getppid(), SIGINT);
			exit(0);
		}
		printf("server: %s", buf); // 화면 출력
	}
	return 0;
}
