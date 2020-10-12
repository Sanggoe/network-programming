//-----------------------------------------------
// 파일명 : tcp_echoserv.c
// 기  능 : 에코 서비스를 수행하는 서버
// 컴파일 : cc -o tcp_echoserv tcp_echoserv.c
// 사용법 : ./tcp_echoserv.c 3008
//-----------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define MAXLINE 127

int main(int argc, char *argv[]) {
	struct sockaddr_in servaddr, cliaddr;	// 소켓 두개
	int listen_sock, accp_sock,		// 소켓 번호
		addrlen=sizeof(cliaddr),	// 소켓주소 구조체 길이
		nbyte,
		count=0;
	char buf[ MAXLINE + 1 ];

	if(argc != 2) {
		printf("usage: %s port\n", argv[0]);
		exit(0);
	}
	// 소켓 생성
	if((listen_sock = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket fail");
		exit(0);
	}

	// servaddr을 '\0'으로 초기화
	bzero((char *)&servaddr, sizeof(servaddr));
	// servaddr 세팅
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(atoi(argv[1]));	// 문자열 포트번호

	// bind() 호출
	if(bind(listen_sock, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
		perror("bind fail");
		exit(0);
	}
	// 소켓을 수동 대기모드로 세팅
	listen(listen_sock, 5);
	// 여기까지 하면 준비 완료!
	
	// iterative 에코 서비스 수행
	while(1) {
		puts("서버가 요청을 기다림");
		// 연결 요청을 기다림. 여기서 block 된다!
		accp_sock = accept(listen_sock, (struct sockaddr *)&cliaddr, &addrlen);
		if(accp_sock < 0) {
			perror("accept fail");
			exit(0);
		}
		printf("%d 번째 클라이언트가 연결됨..\n", ++count);
		nbyte = read(accp_sock, buf, MAXLINE);
		buf[nbyte] = 0;
		printf("%s", buf);
		write(accp_sock, buf, nbyte);
		close(accp_sock);
	}
	close(listen_sock);
	return 0;
}
