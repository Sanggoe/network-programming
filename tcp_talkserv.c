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
int recv_and_print(int sd);	// 상대로부터 메시지 수신 후 화면 출력
int input_and_send(int sd);	// 키보드 입력 받고 상대에게 메시지 전달

#define MAXLINE 511

int main(int argc, char *argv[]) {
	struct sockaddr_in cliaddr, servaddr;
	int listen_sock, accp_sock,	// 소켓 번호
		addrlen = sizeof(cliaddr);
	pid_t pid;

	if(argc != 2) {
		printf("Usage: %s port_number\n", argv[0]);
		exit(0);
	}

	// 소켓 생성
	if((listen_sock = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket fail");
		exit(1);
	}
	// 서버의 소켓주소 구조체 초기화
	bzero((char*)&servaddr, sizeof(servaddr));

	// servaddr 셋팅
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(atoi(argv[1]));

	// bind() 호출
	if (bind(listen_sock, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
		perror("bind fail");
	}	
	
	puts("서버가 클라이언트를 기다리고 있습니다.");
	// 소켓 번호를 수동적 소켓으로 listen()
	listen(listen_sock, 5);


	// 클라이언트 연결요청 수락
	if ((accp_sock = accept(listen_sock, (struct sockaddr *)&cliaddr, &addrlen)) < 0) {
		perror("accept fail");
		exit(0);
	}	
	
	puts("클라이언트가 연결되었습니다.");
	
	if ( (pid = fork()) > 0) {
		input_and_send(accp_sock);	// 키보드 입력받고 상대에게 메시지 전달
	} else if (pid == 0) {
		recv_and_print(accp_sock);
	}
	close(listen_sock);
	close(accp_sock);
	return 0;
}

// 키보드 입력받고 상대에게 메시지 전달
int input_and_send(int sd) {
	char buf[MAXLINE+1];
	int nbyte;
	while(fgets(buf, sizeof(buf), stdin) != NULL) {
		nbyte = strlen(buf);
		write(sd, buf, strlen(buf));
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
int recv_and_print(int sd) {
	char buf[MAXLINE+1];
	int nbyte;
	while(1) {
		if ((nbyte = read(sd, buf, MAXLINE)) < 0) {
			perror("read fail");
			close(sd);
			exit(0);
		}
		buf[nbyte] = 0;

		// 종료문자열 수신시 종료
		if (strstr(buf, EXIT_STRING) != NULL) {
			puts("Good bye, server! Client out.");
			kill(getppid(), SIGINT);
                        exit(0);			
		}
		printf("%s", buf); // 화면 출력
	}
	return 0;
}
