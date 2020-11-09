//---------------------------------------------
// 파일명 : tcp_talkserv.c
// 기  능 : 토크 클라이언트와 1:1 통신을 한다
// 컴파일 : cc -o tcp_talkserv tcp_talkserv.c
// 사용법 : tcp_talkserv 3008
//---------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <signal.h>

char *EXIT_STRING = "exit";	// 종료문자
int recv_and_print(int sd, struct sockaddr_in cliaddr, int* addrlen);	// 상대로부터 메시지 수신 후 화면 출력
int input_and_send(int sd, struct sockaddr_in cliaddr, int addrlen);	// 키보드 입력 받고 상대에게 메시지 전달

#define MAXLINE 511

int main(int argc, char *argv[]) {
	struct sockaddr_in cliaddr, servaddr;
	int s, addrlen = sizeof(struct sockaddr);
	pid_t pid;
	char buf[MAXLINE+1];
	int nbyte;

	if(argc != 2) {
		printf("Usage: %s port_number\n", argv[0]);
		exit(0);
	}

	// 소켓 생성
	if((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("socket fail");
		exit(1);
	}
	// 서버의 소켓주소 구조체 초기화
	bzero((char*)&servaddr, sizeof(servaddr));
	bzero((char*)&cliaddr, sizeof(cliaddr));

	// servaddr 셋팅
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(atoi(argv[1]));

	// bind() 호출
	if (bind(s, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
		perror("bind fail");
		exit(0);
	}
	
	puts("server waiting...");

	// 클라이언트로부터 첫 번째 메시지 수신
	if ((nbyte = recvfrom(s, buf, MAXLINE, 0, (struct sockaddr*)&cliaddr, &addrlen)) < 0) {
		perror("recvfrom fail");
		close(s);
		exit(0);
	}
	buf[nbyte] = 0;
		
	// 종료문자열 수신시 종료
	if (strstr(buf, EXIT_STRING) != NULL) {
		puts("Good bye, server! Client out.");
		close(s);
		exit(0);
	}
	printf("%s", buf); // 화면 출력


	if ((pid = fork()) > 0) {
		input_and_send(s, cliaddr, addrlen);  // 키보드 입력받고 상대에게 메시지 전달
	} else if (pid == 0) {
		recv_and_print(s, cliaddr, &addrlen); // 상대에게 받은 메시지 수신, 출력
	}
	close(s);
	return 0;
}

// 키보드 입력받고 상대에게 메시지 전달
int input_and_send(int sd, struct sockaddr_in cliaddr, int addrlen) {
	char buf[MAXLINE+1];
	int nbyte;
	
	while(fgets(buf, sizeof(buf), stdin) != NULL) {
		nbyte = strlen(buf);
		if (sendto(sd, buf, strlen(buf), 0, (struct sockaddr*)&cliaddr, addrlen) < 0) {
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
int recv_and_print(int sd, struct sockaddr_in cliaddr, int* addrlen) {
	char buf[MAXLINE+1];
	int nbyte;

	while(1) {
		if ((nbyte = recvfrom(sd, buf, MAXLINE, 0, (struct sockaddr*)&cliaddr, addrlen)) < 0) {
			perror("recvfrom fail");
			close(sd);
			exit(0);
		}
		buf[nbyte] = 0;

		// 종료문자열 수신시 종료
		if (strstr(buf, EXIT_STRING) != NULL) {
			puts("Good bye, server! Client out.");
			close(sd);
			kill(getppid(), SIGINT);
			exit(0);
		}
		printf("client: %s", buf); // 화면 출력
	}
	return 0;
}
